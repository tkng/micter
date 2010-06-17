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

#include <string>
#include <vector>
#include <limits.h>
#include "util.hpp"

using namespace std;

const int utf8_len_data[] = {
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
  // length of 0xfe and 0xff should be 1 because they may be used
  // as a Byte Order Mark
};

int
string_split(const string &s, const char needle, int max_num, vector<string> *result)
{
  size_t s_size = s.size();
  size_t needle_pos = 0;
  int result_num = 0;

  if (max_num <= 0) {
    max_num = INT_MAX;
  }

  if (max_num != 1) {
    for (size_t i = 0; i < s_size; i++) {
      if (s[i] == needle) {
        string s_ = s.substr(needle_pos, i - needle_pos);
        result->push_back(s_);
        needle_pos = i + 1;
        result_num++;
        if (result_num == max_num -1) {
          break;
        }
      }
    }
  }

  if (needle_pos != s_size) {
    string s_ = s.substr(needle_pos, s_size - needle_pos);
    result->push_back(s_);
  }
  return 0;
}


string
string_join(const vector<string> &ss)
{
  string result;
  size_t ss_size = ss.size();
  for (size_t i = 0; i < ss_size; i++) {
    result += ss[i];
  }
  return result;
}

#include <iostream>

// vector<wchar_t> maybe appropriate for return value.
vector<string>
string_to_chars(const string &s)
{
  vector<string> chars;
  size_t s_size = s.size();
  size_t i = 0;

  while (i < s_size) {
    unsigned char c = s[i];
    int char_len = utf8_len_data[c];
    string s1 = s.substr(i, char_len);
    chars.push_back(s1);
    i += char_len;
  }

  return chars;
}

vector<size_t>
string_start_poss(const string &s)
{
  vector<size_t> positions;
  size_t s_size = s.size();
  size_t i = 0;

  while (i < s_size) {
    unsigned char c = s[i];
    int char_len = utf8_len_data[c];
    positions.push_back(i);
    i += char_len;
  }

  // need to know length of last character.
  positions.push_back(i);

  return positions;
}

size_t
string_utf8_next_pos(const string &s, const size_t pos)
{
  unsigned char c = s[pos];
  int char_len = utf8_len_data[c];

  return pos + char_len;
}


size_t
string_utf8_length(const string &s)
{
  size_t len = 0;
  size_t i = 0;
  size_t s_size = s.size();
  while (i < s_size) {
    i = string_utf8_next_pos(s, i);
    len++;
  }
  return len;
}

