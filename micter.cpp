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

#include "micter.hpp"
#include "util.hpp"

#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;
using namespace tr1;

namespace micter {

  SVM::SVM(const float eta_, const float lambda_) :
    exampleN(0),
    eta(eta_),
    lambda(lambda_)
  {}
  
  SVM::SVM(const char *filename) :
  exampleN(0)
  {
    load(filename);
  }

  float SVM::clip_by_zero(float a, float b) const {
    if (a > 0.0) {
      if (a > b) {
        return a - b;
      } else {
        return 0.0;
      }
    } else {
      if (a < - b) {
        return a + b;
      } else {
        return 0.0;
      }
    }
  }

  float SVM::margin(const fv_t &fv, const int y) {
    return dotproduct(fv) * y;
  }
  
  // dot product of weight vector and feature vector.
  float SVM::dotproduct(const fv_t &fv) {
    float m = 0.0;
    size_t fv_size = fv.size();
    for (size_t i = 0; i < fv_size; i++) {
      string key = fv[i].first;
      float x_i = fv[i].second;
      unordered_map<string, float>::iterator wit = w.find(key);
      if (wit != w.end()) {
        //       m +=  x_i * w[key];
        m +=  x_i * wit->second;
      }
    }
    return m;
  }

  void SVM::muladd(const fv_t &fv, const int y, float scale) {
    for (size_t i = 0; i < fv.size(); i++) {
      string key = fv[i].first;
      float x_i = fv[i].second;
      //      unordered_map<string, float>::iterator wit = w.find(key);      

      if (w.find(key) != w.end()) {
        w[key] += y * x_i * scale;
      } else {
        w[key] = y * x_i * scale;
      }
    }
  }
  
  void SVM::l1_regularize(const fv_t& fv) {
    
    for (size_t i = 0; i < fv.size(); i++) {
      string key = fv[i].first;
      unordered_map<string, int>::iterator uit = last_update.find(key);
      if (uit != last_update.end()) {
        int c = exampleN - uit->second;
        uit->second = exampleN;
        
        unordered_map<string, float>::iterator wit = w.find(key);
        wit->second = clip_by_zero(wit->second, lambda * c);
      } else {
        int c = exampleN;
        last_update[key] = exampleN;
        w[key] = clip_by_zero(w[key], lambda * c);    
      }
    }
  }
  
  void SVM::trainExample(const fv_t &fv, const int y) {
    //    b = + 10.0;

    if (margin(fv, y) < 1.0) {
      muladd(fv, y, eta);
      
      l1_regularize(fv);
    }
    exampleN++;
  }

  int SVM::load(const char *filename) {
    ifstream ifs(filename);
    if (!ifs) {
      return -1;
    }
    
    string line;
    while (getline(ifs, line)) {
      vector<string> a;
      string_split(line, '\t', 2, &a);
      double val = strtod(a[1].c_str(), NULL);
      w[a[0]] = val;
    }
    return 0;
  }
  
  int SVM::save(const char *filename) {
    ofstream ofs(filename);
    if (!ofs) {
      return -1;
    }

    unordered_map<string, float>::iterator it;
    for (it = w.begin(); it != w.end(); it++) {
      string key = it->first;
      if (last_update.find(key) != last_update.end()) {
        int c = exampleN - last_update[key];
        last_update[key] = exampleN;
        w[key] = clip_by_zero(w[key], lambda * c);
      }

      if (fabsf(it->second) > 0.00000001) {
        ofs << it->first << "\t" << setprecision(3) << it->second << endl;
      }
    }
    return 0;
  }

  micter::micter() :svm(2.0, 0.0000001) {
  }

  vector<size_t>
  calc_cut_pos(const vector<string> &words) {
    vector<size_t> cuts;
    size_t pos = 0;

    for (size_t i = 0; i < words.size(); i++) {
      size_t len = string_utf8_length(words[i]);
      pos += len;
      cuts.push_back(pos);
    }
    return cuts;
  }
  
  const char feature_prefix[] = {'a', 'b', 'c', 'd', 'e', 'f'};
  
  bool is_numeric(const string &a) {
    if (a[0] >= '0' && a[0] <= '9') {
      return true;
    }
    return false;
  }

  bool is_hiragana(const string &a) {
    unsigned char c1 = a[0];
    unsigned char c2 = a[1];
    unsigned char c3 = a[2];

    if (c1 == 0xE3) {
      if (c2 == 0x81) {
        if (c3 >= 0x81)
          return true;
      } else if (c2 == 0x82) {
        if (c3 <= 0x9F)
          return true;
      }
    }
    return false;
  }
  
  bool is_katakana(const string &a) {
    unsigned char c1 = a[0];
    unsigned char c2 = a[1];
    unsigned char c3 = a[2];

    if (c1 == 0xE3) {
      if (c2 == 0x82) {
        if (c3 >= 0xA1)
          return true;
      } else if (c2 >= 0x83 && c2 <= 0x86) {
        return true;
      } else if (c2 == 0x87) {
        if (c3 <= 0xBF)
          return true;
      }
    }
    return false;
  }

  bool is_symbol(const string &a) {
    if (a == "、")
      return true;
    if (a == "。")
      return true;
    if (a == "！")
      return true;
    if (a == "？")
      return true;
    if (a == ",")
      return true;
    if (a == ".")
      return true;
    if (a == "!")
      return true;
    if (a == "?")
      return true;
    return false;
  }

  vector<char>
  chars_to_types(const vector<string> &chars) {
    vector<char> ret;
    //    vector<string>::iterator it = chars.begin();
    //for (it = chars.begin(); it != chars.end(); it++) {
    for (size_t i = 0; i < chars.size(); i++) {
      string a = chars[i];
      char c;
      if (is_numeric(a)) {
        c = '1';
      } else if (is_hiragana(a)) {
        c = '2';
      } else if (is_katakana(a)) {
        c = '3';
      } else if (is_symbol(a)) {
        c = '4';
      } else {
        c = '5';
      }
      ret.push_back(c);
    }
    return ret;
  }

  void
  generate_bigram_fv(const vector<string> &chars, size_t center, fv_t *fv) {
    string f = string("A");
    if (center >= 1) {
      f += chars[center-1];
    }
    f += chars[center];
    fv->push_back(make_pair(f, 1.0));
  }

  // type = letter type like hiragana, katakana, alphabets.
  void
  generate_type_bigram_fv(const vector<char> &char_types, size_t center, fv_t *fv) {
    string f = string("B");
    if (center >= 2) {
      f += char_types[center-2];
    }
    if (center >= 1) {
      f += char_types[center-1];
    }
    f += char_types[center];
    fv->push_back(make_pair(f, 1.0));
  }

  // type = letter type like hiragana, katakana, alphabets.
  void
  generate_char_type_bigram_fv(const vector<string> &chars, const vector<char> &char_types,
                               size_t center, fv_t *fv) {
    string f1 = string("C");
    string f2 = string("D");
    if (center >= 1) {
      f1 += char_types[center-1];
      f2 += chars[center-1];
    }
    f1 += chars[center];
    f2 += char_types[center];
    fv->push_back(make_pair(f1, 1.0));
    fv->push_back(make_pair(f2, 1.0));
  }


  void
  generate_trigram_fv(const vector<string> &chars, size_t center, fv_t *fv) {
    string f = string("E");

    if (center >= 2) {
      f += chars[center-2];
    }

    if (center >= 1) {
      f += chars[center-1];
    }
    f += chars[center];
    fv->push_back(make_pair(f, 1.0));
  }

  
  fv_t
  generate_fv(const vector<string> &chars, const vector<char> &char_types, size_t center) {
    fv_t fv;
    size_t cs_size = chars.size();
    int j = 0;
    for (size_t i = max((size_t)0, center - 2); i < min(cs_size, center+2); i++) {
      string f;
      f += feature_prefix[j];
      f += chars[i];
      fv.push_back(make_pair(f, 1.0));
      j++;
    }
    generate_bigram_fv(chars, center, &fv);
    generate_char_type_bigram_fv(chars, char_types, center, &fv);
    generate_type_bigram_fv(char_types, center, &fv);
    //    generate_trigram_fv(chars, center, &fv);

    return fv;
  }

  vector<string>
  chars_to_words(const vector<string> &chars, vector<int> &cut_pos) {
    vector<string> words;
    size_t i = 0;
    size_t j = 0;

    while (cut_pos.size() > 0) {
      j = cut_pos[0];
      if (i < j) {
        string word = string_join(vector<string>(chars.begin() + i, chars.begin() + j));
        words.push_back(word);
      }
      
      i = j;
      cut_pos.erase(cut_pos.begin());
    }

    if (j < chars.size()) {
      string word = string_join(vector<string>(chars.begin() + j, chars.begin()+chars.size()));
      words.push_back(word);
    }

    return words;
  }

  int micter::split(const string &line, vector<string> *result) {
    vector<string> chars = string_to_chars(line);
    vector<char> char_types = chars_to_types(chars);
    vector<int> cut_pos;

    size_t cs_size = chars.size();
    for (size_t i = 0; i < cs_size; i++) {
      fv_t fv = generate_fv(chars, char_types, i);
      if (svm.dotproduct(fv) >= 0.0) {
        cut_pos.push_back(i);
      }
    }
    
    vector<string> tmp = chars_to_words(chars, cut_pos);
    *result = tmp;
    return 0;
  }

  int micter::train_sentence(const vector<string> &words) {
    string sentence = string_join(words);
    vector<string> chars = string_to_chars(sentence);
    vector<char> char_types = chars_to_types(chars);
    vector<size_t> cuts = calc_cut_pos(words);
    
    size_t cs_size = chars.size();
    for (size_t i = 0; i < cs_size; i++) {
      int y;
      fv_t fv;
      fv = generate_fv(chars, char_types, i);
      
      if (cuts.size() > 0 && cuts[0] == i) {
        y = 1;
        // cuts should be reversed to use pop_back()
        // if combination of erase() and begin() is too slow.
        cuts.erase(cuts.begin());
      } else {
        y = -1;
      }

      // cout << "y: "<< y;
      // for (size_t j = 0; j < fv.size(); j++) {
      //   cout << " " << fv[j].first;
      // }
      // cout << endl;

      svm.trainExample(fv, y);
    }
    return 0;
  }

  int micter::test_sentence(const vector<string> &words,
                            int *tp, int *tn, int *fp, int *fn) {
    string sentence = string_join(words);
    vector<string> chars = string_to_chars(sentence);
    vector<char> char_types = chars_to_types(chars);
    vector<size_t> cuts = calc_cut_pos(words);

    size_t cs_size = chars.size();
    for (size_t i = 0; i < cs_size; i++) {
      int y;
      fv_t fv;
      fv = generate_fv(chars, char_types, i);
      
      if (cuts.size() > 0 && cuts[0] == i) {
        y = 1;
        // cuts should be reversed to use pop_back()
        // if combination of erase() and begin() is too slow.
        cuts.erase(cuts.begin());
      } else {
        y = -1;
      }

      if (svm.dotproduct(fv) > 0.0) {
        if (y > 0) {
          (*tp)++;
        } else {
          (*fp)++;
        }
      } else {
        if (y > 0) {
          (*fn)++;
        } else {
          (*tn)++;
        }
      }
    }
    
    return 0;

  }

  int micter::load(const char *filename) {
    return svm.load(filename);
  }

  int micter::save(const char *filename) {
    return svm.save(filename);
  }
}
