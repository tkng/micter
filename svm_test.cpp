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

#include <sstream>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
// fast version of parse_line, which doesn't use so many istringstreams.
// static int parse_line(const string &line, micter::fv_t *fv, int *y)
// {
//   const char *cline = strdup(line.c_str());
//   char *endp;

//   *y = strtol(cline, &endp, 10);

//   if (endp) {
//     cerr << "label (integer) expected. ";
//     return -1;
//   }
// }

static int parse_line(const string &line, micter::fv_t *fv, int *y)
{
  // empty line cannot be parsed.
  if (line == "") {
    return -1;
  }

  istringstream is(line);
  int y_;
  if (!(is >> y_)) {
    cerr << "parse error: no label" << endl;
    return -1;
  }
  *y = y_;

  string tmp;

  while (is >> tmp) {
    vector<string> ret;
    string id;
    float val;
    string_split(tmp, ':', 2, &ret);
    id = ret[0];
    istringstream(ret[1]) >> val;
    fv->push_back(std::make_pair(id, val));
  }
  return 0;
}


static void train(micter::SVM &svm, const char *filename)
{
  ifstream ifs(filename);
  
  while(ifs && !ifs.eof()) {
    string line;
    micter::fv_t fv;
    int y;
    getline(ifs, line);
    int ret = parse_line(line, &fv, &y);
    if (ret == 0) {
      svm.trainExample(fv, y);
    }
  }
}

static void test(micter::SVM &svm, const char *filename)
{
  ifstream ifs(filename);
  int ok1 = 0, ok2 = 0;
  int ng1 = 0, ng2 = 0;

  while(!ifs.eof()) {
    string line;
    micter::fv_t fv;
    int y;
    getline(ifs, line);
    int ret = parse_line(line, &fv, &y);
    if (ret == 0) {
      if (y > 0) {
        if (svm.margin(fv, y) > 0.0) {
          ok1++;
        } else {
          ng1++;
        }
      } else {
        if (svm.margin(fv, y) > 0.0) {
          ok2++;
        } else {
          ng2++;
        }
      }
    }
  }
  cout << filename << endl;
  cout << "  accuracy:  " << ((double)ok1 + ok2) / (ok1+ok2 + ng1 + ng2) << endl;
  cout << "  precision: " << ((double)ok1) / (ok1 + ng1) << endl;
  cout << "  recall:    " << ((double)ok1) / (ok1 + ng2) << endl;
}



int main(int argc, char *argv[])
{
  micter::SVM svm(1.0, 0.00001);

  const char *train_file = "a2a";
  const char *test_file = "a2a.t";


  for (int i = 0; i < 3; i++) {
    train(svm, train_file);
  }

  test(svm, train_file);
  test(svm, test_file);

  return 0;
}
