// Copyright 2010. All Rights Reserved.
// Author: shunpingye@gmail.com (Shunping Ye)

#ifndef css_segmenter_token_h
#define css_segmenter_token_h

#include <string>
#include "csr_typedefs.h"

namespace css {
class SegmentedToken {
 public:
  SegmentedToken() {
  }
  SegmentedToken(const int begin, const int end, const std::string &str)
    : begin_(begin),
      end_(end),
      word_(str) {
  }
  void Clear() {
    begin_ = 0;
    end_ = 0;
    word_.clear();
  }

  int begin() const {
    return begin_;
  }
  int end() const {
    return end_;
  }
  const std::string& word() const {
    return word_;
  }
  const std::string& token_type() const {
    return token_type_;
  }
 protected:
   int begin_;
   int end_;
   std::string word_;
   // TODO: replace with enum
   std::string token_type_;
 private:
   friend class Segmenter;
};

};  // namespace css

#endif  // css_segmenter_token_h
