// Copyright 2010. All Rights Reserved.
// Author: shunpingye@gmail.com (Shunping Ye)

#include "simple_line_reader.h"

#include <stdlib.h>

using namespace std;

namespace css {
SimpleLineReader::SimpleLineReader(const string& filename,
                                   bool skip_empty_line)
    : error_(false) {
  fp_ = fopen(filename.c_str(), "r");
  skip_empty_line_ = skip_empty_line;
  if(fp_ == NULL) {
    printf("fail to open file:%s\n", filename.c_str());
    error_ = true;
  }
}

SimpleLineReader::SimpleLineReader(const string& filename)
    : error_(false) {
  fp_ = fopen(filename.c_str(), "r");
  skip_empty_line_ = true;
  if(fp_ == NULL) {
    printf("fail to open file:%s\n", filename.c_str());
    error_ = true;
  }
}

SimpleLineReader::~SimpleLineReader() {
  if (fp_) {
    fclose(fp_);
  }
}

bool SimpleLineReader::ReadLines(std::vector<std::string>* out) {
  if (error_) {
    return false;
  }
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  while ((read = getline(&line, &len, fp_)) != -1) {
    if (read == 0 || (read == 1 && *line == '\n')) {
      if (!skip_empty_line_) {
        out->push_back(line);
      }
    } else {
      string str = line;
      str.resize(str.length() - 1);
      out->push_back(str);
    }
  }
  free(line);
  return true;
}
}
