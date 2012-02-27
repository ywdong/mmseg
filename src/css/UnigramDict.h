#ifndef UNIGRAM_DICT_h
#define UNIGRAM_DICT_h

#include <cstring>
#include <string>
#include <vector>

#include "csr_mmap.h"
#include "csr.h"
#include "darts.h"
#include "UnigramRecord.h"
#include "segmenter_token.h"

namespace css {

class UnigramDict {

public:
  typedef Darts::DoubleArray::result_pair_type result_pair_type;
  UnigramDict();
  ~UnigramDict();

  int load(const char* filename);
  bool isLoad();
  //  Return all offsets
  bool FindHits(const char* key, std::vector<int>* ret);
  int findHits(const char* buf,
               result_pair_type *result = NULL,
               size_t result_len = 0,
               int keylen = 0);
  //the return string buffer might contains 0, end with \0\0
  int FindTypeByOffset(int offset);
  int FindFreqByOffset(int offset);
  int exactMatch(const char* key, int *id = NULL);
  bool FindTypes(const string& key, std::vector<TokenType::type>* ret);
  int import(const char* filename, const char* target_file = NULL);
  bool old_version() {
    return m_old_version;
  }
protected:
  _csr_mmap_t* m_file;
  int* m_stringpool;
  Darts::DoubleArray m_da;
  bool m_old_version;
};

} /* End of namespace css */
#endif  // UNIGRAM_DICT_h
