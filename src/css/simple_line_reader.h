// Copyright 2010. All Rights Reserved.
// Author: shunpingye@gmail.com (Shunping Ye)

#ifndef FILE_SIMPLE_LINE_READER_H_
#define FILE_SIMPLE_LINE_READER_H_

#include <stdio.h>
#include <vector>
#include <string>

namespace css {
class SimpleLineReader {
 public:
  SimpleLineReader(const std::string& filename,
                   bool skip_empty_line);
  explicit SimpleLineReader(const std::string& filename);
  ~SimpleLineReader();
  bool ReadLines(std::vector<std::string>* out);

 private:
  FILE* fp_;
  bool skip_empty_line_;
  bool error_;
  SimpleLineReader(const SimpleLineReader&);
  void operator=(const SimpleLineReader&);
};
}  // namespace css
#endif  // FILE_SIMPLE_LINE_READER_H_
