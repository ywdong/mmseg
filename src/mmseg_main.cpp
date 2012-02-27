/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: GPL 2.0
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License. You should have
 * received a copy of the GPL license along with this program; if you
 * did not, you can find it at http://www.gnu.org/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Coreseek.com code.
 *
 * Copyright (C) 2007-2008. All Rights Reserved.
 *
 * Author:
 *	Li monan <li.monan@gmail.com>
 *
 * ***** END LICENSE BLOCK ***** */

#include <fstream>
#include <string>
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <map>
#include  <stdlib.h>
#include <time.h>

#ifdef WIN32
#include "bsd_getopt_win.h"
#else
#include "bsd_getopt.h"
#endif

//#include "UnigramCorpusReader.h"
#include "UnigramDict.h"
#include "SynonymsDict.h"
#include "ThesaurusDict.h"
#include "SegmenterManager.h"
#include "Segmenter.h"
#include "csr_utils.h"

using namespace std;
using namespace css;

#define SEGMENT_OUTPUT 1

void usage(const char* argv_0) {
  printf("Coreseek COS(tm) MM Segment 1.0\n");
  printf("Copyright By Coreseek.com All Right Reserved.\n");
  printf("Usage: %s <option> <file>\n", argv_0);
  printf("-u <unidict>           Unigram Dictionary\n");
  printf(
      "-r           Combine with -u, used a plain text build Unigram Dictionary, default Off\n");
  printf("-b <Synonyms>           Synonyms Dictionary\n");
  printf("-t <thesaurus>          Thesaurus Dictionary\n");
  printf("-q          quiet, DO NOT print log\n");
  printf("-h            print this help and exit\n");
  printf("-p            performance test time\n");
  return;
}

int segment(const char* file, Segmenter* seg, int repeated_time, bool bQuite);
/*
 Use this program
 Usage:
 1)
 ./ngram [-d dict_path] [file] [outfile]
 Do segment. Will print segment result to stdout
 -d the path with contains unidict & bidict
 file: the file to be segment, must encoded in UTF-8 format. [*nix only] if file=='-', read data from stdin
 2)
 ./ngram -u file [outfile]
 Build unigram dictionary from corpus file.
 file: the unigram corpus. Use \t the separate each item.
 eg.
 item	3
 n:2	a:1
 if outfile not assigned , file.uni will be used as output
 3)
 ./ngram -u unidict -b file [outfile]
 Build bigram
 if outfile not assigned , file.bi will be used as output
 */

int main(int argc, char **argv) {
  int c;
  const char* corpus_file = NULL;
  const char* uni_corpus_file = NULL;
  const char* thesaurus_file = NULL;
  const char* out_file = NULL;
  const char* dict_path = NULL;
  const char* target_file = NULL;
  char out_buf[512];

  if (argc < 2) {
    usage(argv[0]);
    exit(0);
  }
  u1 bPlainText = 0;
  bool bQuite = false;
  int repeated_time = 1;
  u1 bUcs2 = 0;
  while ((c = getopt(argc, argv, "t:b:u:d:o:p:rqU")) != -1) {
    switch (c) {
    case 'o':
      target_file = optarg;
      break;
    case 'u':
      uni_corpus_file = optarg;
      break;
    case 'b':
      corpus_file = optarg;
      break;
    case 'd':
      dict_path = optarg;
      break;
    case 't':
      thesaurus_file = optarg;
      break;
    case 'r':
      bPlainText = 1;
      break;
    case 'q':
      printf("quiet mode\n");
      bQuite = true;
      break;
    case 'p':
      repeated_time = atoi(optarg);
      break;
    case 'U':
      bUcs2 = 1;
      break;
    case 'h':
      usage(argv[0]);
      exit(0);
    default:
      fprintf(stderr, "Illegal argument \"%c\"\n", c);
      return 1;
    }
  }

  if (optind < argc) {
    out_file = argv[optind];
  }

  if (thesaurus_file) {
    ThesaurusDict tdict;
    tdict.import(thesaurus_file, target_file);
    //ThesaurusDict ldict;
    //ldict.load("thesaurus.lib");
    return 0;
  }

  if (corpus_file) {
    //build Synonyms dictionary
    SynonymsDict dict;
    dict.import(corpus_file);
    if (target_file)
      dict.save(target_file);
    else
      dict.save("synonyms.dat");
    //debug use
    //dict.load("synonyms.dat");
    //printf("%s\n", dict.exactMatch("c#"));
    return 0;
  }

  if (!corpus_file && !dict_path) {
    //build unigram
    if (!out_file) {
      //build the output filename
      size_t len = strlen(uni_corpus_file);
      memcpy(out_buf, uni_corpus_file, len);
      memcpy(&out_buf[len], ".uni\0", 5);
      out_file = out_buf;
    }

    if (target_file) {
      out_file = target_file;
    }

    if (!bUcs2) {
      UnigramDict ud;
      printf("begin to import\n");
      int ret = ud.import(uni_corpus_file, out_file);
    } else {
      printf("UCS2 used as inner encoding, is unsupported\n");
    }
    return 0;
  } else if (!dict_path) { //not segment mode.
    //build bigram
    if (!out_file) {
      //build the output filename
      size_t len = strlen(corpus_file);
      memcpy(out_buf, corpus_file, len);
      memcpy(&out_buf[len], ".bi\0", 4);
      out_file = out_buf;
    }
    printf("Bigram build unsupported.\n");
  }//end if(!corpus_file)

  //Segment mode
  {
    SegmenterManager* mgr = new SegmenterManager();
    int nRet = 0;
    if (dict_path)
      nRet = mgr->init(dict_path);
    else {
      usage(argv[0]);
      exit(0);
    }
    if (nRet == 0) {
      //init ok, do segment.
      setlocale(LC_ALL, ""); //fix in chinese filename for win32
      Segmenter* seg = mgr->getSegmenter();
      segment(out_file, seg, repeated_time, bQuite);
    }
    delete mgr;
  }

  return 0;
}

bool ReadFileToString(const string& filename,
                      string* out) {
  FILE* fp = fopen(filename.c_str(), "rb");
  if (fp == NULL) {
    printf("fail to open file:%s\n", filename.c_str());
    return false;
  }
  char buf[1 << 16];
  size_t len = 0;
  while ((len = fread(buf, 1, sizeof(buf), fp)) > 0) {
      out->append(buf, len);
  }
  fclose(fp);
  return true;
}

int segment(const char* file, Segmenter* seg, int repeated_time, bool bQuite) {
  printf("repeated_time:%d\n", repeated_time);
  unsigned long begin_time = currentTimeMillis();
  string content;
  if (!ReadFileToString(file, &content)) {
    printf("fail to read file:%s\n", file);
    return -1;
  }
  for (int i = 0; i < repeated_time; ++i) {
    seg->setBuffer((u1*) content.c_str(), content.length());
    SegmentedToken word;
    while (seg->GetNextToken(&word)) {
      if (i == 0 && !bQuite) {
        printf("%s", word.word().c_str());
        for (int k = 0; k < word.token_type().size(); ++k) {
          printf("\t%s", TokenTypeToString(word.token_type()[k]));
        }
        printf("\n");
      }
      word.Clear();
    }
  }
  unsigned long total_time = currentTimeMillis() - begin_time;
  printf("\n\nWord Splite took: %lu ms.\n", total_time);
  printf("total size: %lu(bytes):\n", repeated_time * content.length());
  printf("performance: %f(KB/s)\n",
      double(repeated_time * content.length()) / total_time);

  return 0;
}
