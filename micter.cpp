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
      feature key = fv[i].first;
      float x_i = fv[i].second;
      unordered_map<feature, float>::iterator wit = w.find(key);
      if (wit != w.end()) {
        //       m +=  x_i * w[key];
        m +=  x_i * wit->second;
      }
    }
    return m;
  }

  void SVM::muladd(const fv_t &fv, const int y, float scale) {
    for (size_t i = 0; i < fv.size(); i++) {
      feature key = fv[i].first;
      float x_i = fv[i].second;

      unordered_map<feature, float>::iterator wit = w.find(key);
      if (wit != w.end()) {
        wit->second += y * x_i * scale;
      } else {
        feature key_ = feature(key.ftype_, key.str_, key.len_, true);
        w[key_] = y * x_i * scale;
      }
    }
  }
  
  void SVM::l1_regularize(const fv_t& fv) {
    
    for (size_t i = 0; i < fv.size(); i++) {
      feature key = fv[i].first;
      unordered_map<feature, int>::iterator uit = last_update.find(key);
      if (uit != last_update.end()) {
        int c = exampleN - uit->second;
        uit->second = exampleN;
        
        unordered_map<feature, float>::iterator wit = w.find(key);
        wit->second = clip_by_zero(wit->second, lambda * c);
      } else {
        int c = exampleN;
        feature key_ = feature(key.ftype_, key.str_, key.len_, true);
        last_update[key_] = exampleN;
        w[key_] = clip_by_zero(w[key_], lambda * c);    
      }
    }
  }
  
  void SVM::trainExample(const fv_t &fv, const int y) {
    //    b = + 10.0;

    if (margin(fv, y) < 1.0) {
      muladd(fv, y, eta);
      
      l1_regularize(fv);
    }

    if (exampleN % 500000 == 0) {
      unordered_map<feature, float>::iterator it;
      for (it = w.begin(); it != w.end(); it++) {
        feature key = it->first;
        if (last_update.find(key) != last_update.end()) {
          int c = exampleN - last_update[key];
          last_update[key] = exampleN;
          float newv = clip_by_zero(it->second, lambda * c);
          if (fabsf(newv) < lambda) {
            w.erase(it);
          } else {
            it->second = newv;
          }
        }
      }
    }

    exampleN++;
  }

  static char parse_ftype(char a1, char a0) {
    int d1 = a1 - 0x30;
    int d0 = a0 - 0x30;
    return d1 * 16 + d0;
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
      //      cout << line << " " <<a[0] << endl;
      //      cout << line[0] << a[0].substr(1).c_str() << " " << a[0].size() -1<< endl;
      char ftype = parse_ftype(line[0], line[1]);
      //      cout << hex << ftype << endl;
      feature f(ftype, a[0].substr(2).c_str(), a[0].size() -2, true);
      w[f] = val;
    }
    return 0;
  }
  
  int SVM::save(const char *filename) {
    ofstream ofs(filename);
    if (!ofs) {
      return -1;
    }

    unordered_map<feature, float>::iterator it;
    for (it = w.begin(); it != w.end(); it++) {
      feature key = it->first;
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
  
  bool is_numeric(const string &a, size_t pos) {
    if (a[pos] >= '0' && a[pos] <= '9') {
      return true;
    }
    return false;
  }

  bool is_hiragana(const string &a, size_t pos) {
    unsigned char c1 = a[pos];
    unsigned char c2 = a[pos+1];
    unsigned char c3 = a[pos+2];

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
  
  bool is_katakana(const string &a, size_t pos) {
    unsigned char c1 = a[pos];
    unsigned char c2 = a[pos+1];
    unsigned char c3 = a[pos+2];

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

  bool is_symbol(const string &a, size_t pos) {
    // if (a == "、")
    //   return true;
    // if (a == "。")
    //   return true;
    // if (a == "！")
    //   return true;
    // if (a == "？")
    //   return true;
    if (a[pos] == ',')
      return true;
    if (a[pos] == '.')
      return true;
    if (a[pos] == '!')
      return true;
    if (a[pos] == '?')
      return true;
    return false;
  }

  vector<char>
  gen_char_types(const string &chars, vector<size_t> &positions) {
    vector<char> ret;
    for (size_t i = 0; i < positions.size(); i++) {
      char c;
      size_t pos = positions[i];
      if (is_numeric(chars, pos)) {
        c = 0;
      } else if (is_hiragana(chars, pos)) {
        c = 1;
      } else if (is_katakana(chars, pos)) {
        c = 2;
      } else if (is_symbol(chars, pos)) {
        c = 3;
      } else {
        c = 4;
      }
      ret.push_back(c);
    }
    return ret;
  }

  void
  generate_bigram_fv(const char *str, vector<size_t> &positions, 
                     size_t center, fv_t *fv) {
    size_t start_pos, end_pos;
    if (center >= 1) {
      start_pos = positions[center-1];
    } else {
      start_pos = positions[center];
    }
    if (center < positions[positions.size()-1]) {
      end_pos = positions[center+1];
    } else {
      end_pos = positions[center];
    }
    feature f(static_cast<char>(5), str + start_pos, end_pos - start_pos);

    fv->push_back(make_pair(f, 1.0));
  }

  // // type = letter type like hiragana, katakana, alphabets.
  void
  generate_type_bigram_fv(const vector<char> &char_types, size_t center, fv_t *fv) {
    char fnum = 5;
    if (center >= 1) {
      fnum += char_types[center-1];
    }
    fnum += char_types[center] * 5;
    //    cout << static_cast<int>(fnum) << endl;
    feature f(fnum, "", 0);
    fv->push_back(make_pair(f, 1.0));
  }

  // // type = letter type like hiragana, katakana, alphabets.
  // void
  // generate_char_type_bigram_fv(const vector<string> &chars, const vector<char> &char_types,
  //                              size_t center, fv_t *fv) {
  //   string f1 = string("C");
  //   string f2 = string("D");
  //   if (center >= 1) {
  //     f1 += char_types[center-1];
  //     f2 += chars[center-1];
  //   }
  //   f1 += chars[center];
  //   f2 += char_types[center];
  //   fv->push_back(make_pair(f1, 1.0));
  //   fv->push_back(make_pair(f2, 1.0));
  // }

  void
  generate_trigram_fv(const char *str, vector<size_t> &positions, 
                      size_t center, fv_t *fv) {
    size_t start_pos, end_pos;
    if (center >= 2) {
      start_pos = positions[center-2];
    } else if (center == 1) {
      start_pos = positions[center-1];
    } else {
      start_pos = positions[center];
    }

    if (center < positions[positions.size()-1]) {
      end_pos = positions[center+1];
    } else {
      end_pos = positions[center];
    }

    feature f(static_cast<char>(6), str + start_pos, end_pos - start_pos);

    fv->push_back(make_pair(f, 1.0));
  }
  
  fv_t
  generate_fv(const char *str, vector<size_t> &positions,
              const vector<char> &char_types, size_t center) {
    fv_t fv;

    size_t cs_size = positions.size()-1;

    int j = 0;
    for (size_t i = max((size_t)0, center - 2); i < min(cs_size, center+2); i++) {
      size_t start_pos, end_pos;
      start_pos = positions[i];
      end_pos = positions[i+1];
      feature f(static_cast<char>(j), str + start_pos, end_pos - start_pos);
      fv.push_back(make_pair(f, 1.0));
      j++;
    }
    generate_bigram_fv(str, positions, center, &fv);
    generate_trigram_fv(str, positions, center, &fv);
    //    generate_char_type_bigram_fv(chars, char_types, center, &fv);
    generate_type_bigram_fv(char_types, center, &fv);

    return fv;
  }

  int micter::split(const string &line, vector<string> *result) {
    vector<string> chars;// = string_to_chars(line);
    vector<size_t> positions = string_start_poss(line);
    vector<char> char_types = gen_char_types(line, positions);
    // FIXME: use smart pointer for Exception Safety.
    char *str = strdup(line.c_str());

    vector<size_t> cut_poss;

    size_t cs_size = positions.size()-1;
    for (size_t i = 0; i < cs_size; i++) {
      fv_t fv = generate_fv(str, positions, char_types, i);
      if (svm.dotproduct(fv) >= 0.0) {
        cut_poss.push_back(i);
      }
    }
    
    free(str);
    vector<string> tmp = string_split_at(line, cut_poss, positions);
    *result = tmp;
    return 0;
  }

  int micter::train_sentence(const vector<string> &words) {
    string sentence = string_join(words);
    vector<size_t> positions = string_start_poss(sentence);
    vector<char> char_types = gen_char_types(sentence, positions);
    vector<size_t> cuts = calc_cut_pos(words);
    // FIXME: use smart pointer for Exception Safety.
    char *str = strdup(sentence.c_str());
    
    size_t cs_size = positions.size()-1;
    for (size_t i = 0; i < cs_size; i++) {
      int y;
      fv_t fv;
      fv = generate_fv(str, positions, char_types, i);
      
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
    free(str);
    return 0;
  }

  int micter::test_sentence(const vector<string> &words,
                            int *tp, int *tn, int *fp, int *fn) {
    string sentence = string_join(words);
    vector<size_t> positions = string_start_poss(sentence);
    vector<char> char_types = gen_char_types(sentence, positions);
    vector<size_t> cuts = calc_cut_pos(words);
    char *str = strdup(sentence.c_str());

    //    size_t cs_size = string_utf8_length(sentence);
    size_t cs_size = positions.size()-1;
    //    cout << chars.size() << "  " << positions.size() << endl;
    for (size_t i = 0; i < cs_size; i++) {
      int y;
      fv_t fv;
      fv = generate_fv(str, positions, char_types, i);
      
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
    free(str);
    return 0;

  }

  int micter::load(const char *filename) {
    return svm.load(filename);
  }

  int micter::save(const char *filename) {
    return svm.save(filename);
  }
}
