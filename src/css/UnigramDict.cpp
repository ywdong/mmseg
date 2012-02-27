#include <algorithm>
#include <string>
#include <vector>

#include "UnigramDict.h"
#include "simple_line_reader.h"
#include "csr_mmap.h"

using std::string;
using std::vector;

namespace css {

const char unigram_head_mgc[] = "UNIG";

typedef struct _csr_UnigramDict_fileheader_tag {
  char mg[4];
  short version;
  short reserve;
  int darts_size;
  int pool_size;
} _csr_UnigramDict_fileheader;


UnigramDict::UnigramDict()
  :  m_file(NULL), m_stringpool(NULL), m_old_version(false) {
}
UnigramDict::~UnigramDict() {
  if (m_file) {
    csr_munmap_file(m_file);
  }
}

int UnigramDict::load(const char* filename) {
  m_file = csr_mmap_file(filename, 1); //load all into memory
  if (!m_file)
    return -1; //can not load dict.
  csr_offset_t tm_size = csr_mmap_size(m_file);
  u1* ptr = (u1*) csr_mmap_map(m_file);
  u1* ptr_end = ptr + tm_size;

  _csr_UnigramDict_fileheader* head_ = (_csr_UnigramDict_fileheader_tag*) ptr;
  if (strncmp(head_->mg, unigram_head_mgc, 4) == 0) {
    if (head_->version == 1) {
      ptr += sizeof(_csr_UnigramDict_fileheader_tag);
      if (ptr >= ptr_end)
        return -4; //file broken
      m_da.clear();
      m_da.set_array(ptr, head_->darts_size);
      ptr += m_da.unit_size() * head_->darts_size;
      if (ptr >= ptr_end)
        return -4; //file broken.
      m_stringpool = (int*)ptr;
      ptr += head_->pool_size;
      if (ptr > ptr_end)
        return -4; //file broken.
    } else {
      return -2;
    }
  } else {
    //  try to load old format file
    m_da.clear();
    m_da.set_array(ptr, tm_size);
    m_old_version = true;
  }

  return 0;
}

bool Cmp(const UnigramRecord& p1, const UnigramRecord& p2) {
  char i = 0;
  while (1) {
    unsigned char pu1 = p1.key[i];
    unsigned char pu2 = p2.key[i];
    if (pu1 == pu2) {
      if (pu1 == 0)
        break;
      i++;
    } else {
      return pu1 < pu2;
    }
  }
  return true;
}

bool UnigramDict::isLoad() {
  return m_da.array() != NULL;
}

int UnigramDict::import(const char* filename, const char* target_file) {
  std::vector<UnigramRecord> items;

  SimpleLineReader reader(filename);
  vector<string> lines;
  reader.ReadLines(&lines);
  for (size_t i = 0; i < lines.size(); ++i) {
    vector<string> vec;
    SplitString(lines[i], '\t', &vec);
    if (vec.size() != 3) {
      printf("bad line:%s\n", lines[i].c_str());
      continue;
    }
    UnigramRecord record(vec[0], atoi(vec[1].c_str()), atoi(vec[2].c_str()));
    items.push_back(record);
  }

  int buff_size = sizeof(int) * items.size() * 2;
  u1* total_buf = (u1*) malloc(buff_size);
  memset((void*) total_buf, 0, buff_size);
  u1* total_buf_ptr = total_buf;
  //  read complete, try make dict
  std::sort(items.begin(), items.end(), Cmp);
  {
    std::vector<Darts::DoubleArray::key_type *> key;

    size_t i = 0;
    for (i = 0; i < items.size(); i++) {
      const UnigramRecord& rec = items[i];
      char* ptr = (char*)&rec.key[0];
      key.push_back(ptr);
      memcpy(total_buf_ptr, &rec.freq, sizeof(int));
      total_buf_ptr += sizeof(int);
      memcpy(total_buf_ptr, &rec.token_type, sizeof(int));
      total_buf_ptr += sizeof(int);
    }
    m_da.clear();
    int nRet = m_da.build(key.size(), &key[0], 0);
    //should check the nRet value
    //try save file
    std::string dest_file = "uni.lib";
    size_t size_ = m_da.size();
    const void* iArray = m_da.array();
    _csr_UnigramDict_fileheader_tag head;
    memcpy(&head, unigram_head_mgc, sizeof(unigram_head_mgc));
    head.darts_size = size_;
    head.version = 1;
    head.reserve = 0;
    head.pool_size = buff_size;

    std::FILE *fp = NULL;
    if (target_file)
      fp = std::fopen(target_file, "wb");
    else
      fp = std::fopen(dest_file.c_str(), "wb");

    std::fwrite(&head, sizeof(_csr_UnigramDict_fileheader), 1, fp);
    std::fwrite(iArray, m_da.unit_size(), size_, fp);
    std::fwrite(total_buf, sizeof(u1), buff_size, fp);
    std::fclose(fp);
  }

  //free it
  free(total_buf);
  return 0;
}

bool UnigramDict::FindHits(const char* key, vector<int>* vec) {
  if (!m_da.array()) {
    return false;
  }
  Darts::DoubleArray::result_pair_type rs[1024];
  int num = m_da.commonPrefixSearch(key, rs, sizeof(rs));
  for (int i = 0 ;i < num; ++i) {
    vec->push_back(rs[i].value);
  }
  return true;
}

int UnigramDict::findHits(const char* buf,
                             result_pair_type *result,
                             size_t result_len,
                             int keylen) {
  if(!m_da.array())
    return 0;
  int num = m_da.commonPrefixSearch(buf, result, result_len, keylen);
  return num;
}

int UnigramDict::FindFreqByOffset(int offset) {
  return *(m_stringpool + 2* offset);
}

int UnigramDict::FindTypeByOffset(int offset) {
  return *(m_stringpool + 2* offset + 1);
}

bool UnigramDict::FindTypes(const string& key, vector<TokenType::type>* ret) {
  Darts::DoubleArray::result_pair_type  rs;
  m_da.exactMatchSearch(key.c_str(),rs);
  if(rs.pos) {
    //  printf ("m_old_version:%d\n", m_old_version);
    if (m_old_version) {
      return false;
    } else {
      //  printf("value:%d\ttype:%d\n", rs.value, FindTypeByOffset(rs.value));
      ParseTypesFromInt(FindTypeByOffset(rs.value), ret);
      return true;
    }
  }
}

int UnigramDict::exactMatch(const char* key, int *id) {
  Darts::DoubleArray::result_pair_type  rs;
  m_da.exactMatchSearch(key,rs);
  if(id)
    *id = rs.pos;
  if(rs.pos) {
    if (m_old_version) {
      return rs.value;
    } else {
      return FindFreqByOffset(rs.value);
    }
  }
  ///FIXME: this totaly a mixture. some single char's id > 0 if it in unigram input text, while other's id < 0 if not in ungram text.
  ///so you can not just simply use UCS2 code as a char's id.
  ///FIXED in prof. version by changing unigram-dictionary format.
  //check is single char.
  int len = strlen(key);
  if(len<4){
    const char* tm_pCur = key;
    char v = key[0];
    //might be single cjk char.
    if ( v<128 && len == 1 && id)
      *id =  -1*(int)v;
    // get number of bytes
    int iBytes = 0, iBytesLength = 0;
    while ( v & 0x80 )    {     iBytes++;     v <<= 1;    }
    if(iBytes == len && len != 1){
      //single char
      tm_pCur ++;
      int iCode = 0;
      iCode = ( v>>iBytes );
      iBytes--;
      do
      {
        if ( !(*tm_pCur) )
          break;
        if ( ((*tm_pCur) & 0xC0)!=0x80 ) {
          iCode = 0;
          break;
        }
        iCode = ( iCode<<6 ) + ( (*tm_pCur) & 0x3F );
        iBytes--;
        tm_pCur++;
      } while ( iBytes );
      if(iCode && id)
        *id = -1*iCode;
    }
  }

  if (m_old_version) {
    return rs.value;
  } else {
    return FindFreqByOffset(rs.value);
  }
}
} //end css
