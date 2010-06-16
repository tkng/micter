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

size_t
string_utf8_next_pos(const std::string &s, const size_t pos);

size_t
string_utf8_length(const std::string &s);
