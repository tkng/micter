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

#include <string>
#include <vector>
#include <limits.h>

int
string_split(const std::string &s, const char needle, int max_num,
             std::vector<std::string> *result);

std::string
string_join(const std::vector<std::string> &ss);

std::vector<std::string>
string_to_chars(const std::string &s);

// return start positions of each character.
std::vector<std::size_t>
string_start_poss(const std::string &s);

size_t
string_utf8_next_pos(const std::string &s, const size_t pos);

size_t
string_utf8_length(const std::string &s);

class feature {
private:

public:
  char *str_;
  int ftype_;
  size_t len_;
  bool copy_;

  feature(int ftype, const char *str, int len):
    ftype_(ftype),
    len_(len),
    copy_(false)
  {
    if (copy_) {
      str_ = strndup(str, len);
    } else {
      str_ = const_cast<char *>(str);
    }      
  }
#define UNUSED_ARGUMENT(x) (void)x
  feature(int ftype, char *str, int len, bool copy):
    ftype_(ftype),
    len_(len),
    copy_(true)
  {
    UNUSED_ARGUMENT(copy);
    str_ = strndup(str, len);
  }
  
  ~feature()
  {
    if (copy_) {
      free(str_);
    }
  }
  
};


struct feature_equal_to
{
  bool
  operator()(const feature x, const feature y) const
  {
    if (x.ftype_ == y.ftype_ && x.len_ == y.len_) {
      if (memcmp(x.str_, y.str_, x.len_) == 0) {
        return true;
      }
    }
    return false;
  }
};

struct feature_hash
{
  size_t
  operator()(feature val) const
  {
    size_t __length = val.len_;
    const char *__first = val.str_;
    
    size_t __result = static_cast<size_t>(2166136261UL);
    __result ^= static_cast<size_t>(val.ftype_);
    __result *= static_cast<size_t>(16777619UL);
    
    for (; __length > 0; --__length)
      {
        __result ^= static_cast<size_t>(*__first++);
        __result *= static_cast<size_t>(16777619UL);
      }
    return __result;
  }
  
};
