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

#include <vector>
#include <string>
#include <tr1/unordered_map>

#include "util.hpp"

namespace micter
{
  typedef std::vector<std::pair<feature, double> > fv_t;

  // Support Vector Machine with L1 or L2 regularization.
  // Training algorithm is FOBOS.
  class SVM {
  private:
    std::tr1::unordered_map<feature, float, feature_hash, feature_equal_to> w;
    std::tr1::unordered_map<feature, int, feature_hash, feature_equal_to> last_update;

    float clip_by_zero(float a, float b) const;
    void muladd(const fv_t &fv, const int y, float scale);
    void l1_regularize(const fv_t& fv);

  public:
    int exampleN;
    float eta;
    float lambda;

    SVM(const float eta, float lambda);
    SVM(const char *filename);
    ~SVM() {};

    float margin(const fv_t &fv, const int y);
    float dotproduct(const fv_t &fv);

    // load model, this method will overwrite parameters such as eta.
    int load(const char *filename);
    // save model
    int save(const char *filename);
    
    void trainExample(const fv_t& fv, const int y);
  };

  class micter {
    private:
    SVM svm;
    public:

    micter();

    int split(const std::string &line, std::vector<std::string> *result);

    int train_sentence(const std::vector<std::string> &words);
    int test_sentence(const std::vector<std::string> &words,
                      int *tp, int *tn, int *fp, int *fn);

    int load(const char *filename);
    int save(const char *filename);

  };
}


