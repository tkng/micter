/* 
 *  Copyright (c) 2010 TOKUNAGA Hiroyuki
 * 
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 * 
 *   1. Redistributions of source code must retain the above Copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above Copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   3. Neither the name of the authors nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 */

#pragma once

#include <cstring>

#include <stdlib.h>
#include <limits.h>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <ostream>

int
string_split(const std::string &s, const char needle, int max_num,
             std::vector<std::string> *result);

std::string
string_join(const std::vector<std::string> &ss);

std::vector<std::string>
string_to_chars(const std::string &s);

std::vector<std::string>
string_split_at(const std::string &s, const std::vector<size_t> &cut_poss, const std::vector<size_t> &poss);

// return start positions of each character.
std::vector<size_t>
string_start_poss(const std::string &s);

size_t
string_utf8_next_pos(const std::string &s, const size_t pos);

size_t
string_utf8_length(const std::string &s);

class feature {
private:

public:
  char *str_;
  size_t len_;
  char ftype_;
  bool copy_;

  friend std::ostream& operator<<(std::ostream &out, const feature &x);

  feature(char ftype, const char *str, int len):
    len_(len),
    ftype_(ftype),
    copy_(false)
  {
    if (copy_) {
      str_ = strndup(str, len);
    } else {
      str_ = const_cast<char *>(str);
    }
  }
  
  // quiet g++
  #define UNUSED_ARGUMENT(x) (void)x

  feature(char ftype, const char *str, int len, bool copy):
    len_(len),
    ftype_(ftype),
    copy_(true)
  {
    UNUSED_ARGUMENT(copy);
    str_ = strndup(str, len);
  }

  feature(const feature &a):
    len_(a.len_),
    ftype_(a.ftype_),
    copy_(a.copy_)
  {
    if (copy_) {
      str_ = strndup(a.str_, a.len_);
    } else {
      str_ = const_cast<char *>(a.str_);
    }
  }
  
  ~feature()
  {
    if (copy_) {
      free(str_);
    }
  }
  
  bool
  operator==(const feature x) const
  {
    if (x.ftype_ == ftype_ && x.len_ == len_) {
      if (memcmp(x.str_, str_, x.len_) == 0) {
        return true;
      }
    }
    return false;
  }
};



namespace std {
  namespace tr1 {
    
    template<class T>
    struct hash {
    };
    
    template <>
    struct hash<feature>
    {
      size_t
      operator()(feature val) const
      {
        size_t __length = val.len_;
        const char *__first = val.str_;
        
        size_t __result = static_cast<size_t>(2166136261UL);
        __result ^= static_cast<size_t>(val.ftype_);
        //    __result += static_cast<size_t>(val.ftype_ << 8);
        __result *= static_cast<size_t>(16777619UL);
        
        for (; __length > 0; --__length)
          {
            __result ^= static_cast<size_t>(*__first++);
            __result *= static_cast<size_t>(16777619UL);
          }
        return __result;
      }
    };
  }
}

class feature_iterator
{
  size_t i;
  const char *str_;
  const std::vector<size_t> &sposs_;
  size_t n_;
  feature_iterator(const char *str, const std::vector<size_t> &sposs, size_t n):
    i(0), str_(str), sposs_(sposs), n_(n) {}

  size_t
  size() {
    return 3;
  }
};
