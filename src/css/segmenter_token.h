// Copyright 2010. All Rights Reserved.
// Author: shunpingye@gmail.com (Shunping Ye)

#ifndef css_segmenter_token_h
#define css_segmenter_token_h

#include <string>
#include <vector>

#include "csr_typedefs.h"

namespace css {

struct TokenType {
enum type {
  COMMON = 0,
  ORG  = 1 << 0,
  PERSONAL_NAME = 1 << 1,
  PLACE = 1 << 2,
  // TODO: add more types if needed
};
};

inline const char* TokenTypeToString(TokenType::type type) {
#define TOKEN_TYPE_TO_STRING(type_str) \
  case TokenType::type_str: \
    return #type_str

  switch (type) {
  TOKEN_TYPE_TO_STRING(COMMON);
  TOKEN_TYPE_TO_STRING(ORG);
  TOKEN_TYPE_TO_STRING(PERSONAL_NAME);
  TOKEN_TYPE_TO_STRING(PLACE);
  default:
    return "NOT_IMPLEMENT";
  }
}

inline bool ParseTypesFromInt(int type, std::vector<TokenType::type>* ret) {
  if (type & 0x0001 ) {
    ret->push_back(TokenType::ORG);
  } else if (type & 0x0002) {
    ret->push_back(TokenType::PERSONAL_NAME);
  } else if (type & 0x0004) {
    ret->push_back(TokenType::PLACE);
  }
  return true;
  // TODO(yesp) : add more types
}

inline unsigned int ParseTypesToInt(const std::vector<TokenType::type>& types) {
  unsigned int ret = 0;
  for (size_t i = 0; i < types.size(); ++i) {
    ret += int (types[i]);
  }
  return ret;
}

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
    token_type_.clear();
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
  const std::vector<TokenType::type>& token_type() const {
    return token_type_;
  }
  unsigned int types() const {
    return ParseTypesToInt(token_type_);
  }
 protected:
   int begin_;
   int end_;
   std::string word_;
   // TODO: replace with enum
   std::vector<TokenType::type> token_type_;
 private:
   friend class Segmenter;
};

};  // namespace css

#endif  // css_segmenter_token_h
