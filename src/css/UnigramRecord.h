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

#ifndef css_UnigramRecord_h
#define css_UnigramRecord_h

#include <string>
#include <vector>
#include "csr.h"

using std::string;
namespace css {

struct UnigramRecord {
public:
  UnigramRecord(const std::string& k, int f, int type);
  std::string key;
  int freq;
  int token_type;
};

inline void SplitString(
    const std::string& str,
    char s,
    std::vector<std::string>* r) {
  size_t last = 0;
  size_t i;
  size_t c = str.size();
  for (i = 0; i <= c; ++i) {
    if (i == c || str[i] == s) {
      size_t len = i - last;
      string tmp = str.substr(last, len);
      r->push_back(tmp);
      last = i + 1;
    }
  }
}
} /* End of namespace css */
#endif

