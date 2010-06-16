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
#include "cmdline.h"
#include "util.hpp"

#include <iostream>
#include <fstream>

using namespace std;

static void set_cmdline(cmdline::parser &p)
{
  p.add<string>("model", 'm', "model file", false, "/usr/local/share/micter/micter.model");
  p.add("benchmark", 'b', "benchmark mode");
  p.add("help", 0, "print this message");
  p.footer("filename ...");
}

static void proc_split(micter::micter &mic, istream &ifs)
{
  while(!ifs.eof()) {
    string line;
    vector<string> words;
    getline(cin, line);
    mic.split(line, &words);
    for (size_t i = 0; i < words.size(); i++) {
      cout << words[i] << " ";
    }
    cout << endl;
  }
}

static void proc_bench(micter::micter &mic, istream &ifs)
{
  vector<string> words;
  int tp = 0, tn = 0;
  int fp = 0, fn = 0;
  
  while (!ifs.eof()) {
    string line;
    if (getline(ifs, line) == NULL) {
      break;
    }

    if (line == "") {
      mic.test_sentence(words, &tp, &tn, &fp, &fn);
      words.clear();
    } else {
      words.push_back(line);
    }
  }
  if (words.size() > 0) {
    mic.test_sentence(words, &tp, &tn, &fp, &fn);
    words.clear();
  }

  cout << "  accuracy:  " << ((double)tp + tn) / (tp + tn + fp + fn) << endl;
  cout << "  precision: " << ((double)tp) / (tp + fp) << endl;
  cout << "  recall:    " << ((double)tp) / (tp + fn) << endl;
}


static void proc(micter::micter &mic, istream &ifs, bool bench_mode)
{
  if (bench_mode) {
    proc_bench(mic, ifs);
  } else {
    proc_split(mic, ifs);
  }
}

static void proc_stdin(micter::micter &mic, bool bench_mode)
{
  proc(mic, cin, bench_mode);
}


static void proc_file(micter::micter &mic, const string &filename, bool bench_mode)
{
  ifstream ifs(filename);
  proc(mic, ifs, bench_mode);
}

int main(int argc, char *argv[])
{
  cmdline::parser p;
  vector<string> rest;
  micter::micter mic;
  bool  bench_mode = false;

  set_cmdline(p);

  p.parse(argc, argv);

  if (p.exist("help")){
    cout << p.usage();
    return 0;
  }

  int ret = mic.load(p.get<string>("model").c_str());
  if (ret < 0) {
    cerr << "failed to load model file " << p.get<string>("model") << endl;
    exit(-1);
  }

  if (p.exist("benchmark")) {
    bench_mode = true;
  }

  rest = p.rest();

  if (rest.size() == 0) {
    proc_stdin(mic, bench_mode);
  }
  else {
    vector<string>::iterator it;
    for (it = rest.begin(); it != rest.end(); it++) {
      proc_file(mic, *it, bench_mode);
    }
  }

  return 0;
}
