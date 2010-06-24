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
  p.add<string>("model", 'm', "model file (if exists, will be overwritten)", true);
  p.add<size_t>("iterate", 'I', "iteration times", false, 3);
  p.add("help", 0, "print this message");
  p.footer("filename ...");
}

static void proc(micter::micter &mic, istream &ifs)
{
  vector<string> words;
  
  while (!ifs.eof()) {
    string line;
    if (getline(ifs, line) == NULL) {
      break;
    }

    if (line == "") {
      mic.train_sentence(words);
      words.clear();
    } else {
      words.push_back(line);
    }
  }
  if (words.size() > 0) {
    mic.train_sentence(words);
    words.clear();
  }
}

static void proc_stdin(micter::micter &mic)
{
  proc(mic, cin);
}

static void proc_file(micter::micter &mic, const string &filename)
{
  ifstream ifs(filename.c_str());
  proc(mic, ifs);
}


int main(int argc, char *argv[])
{
  cmdline::parser p;
  vector<string> rest;
  micter::micter mic;

  set_cmdline(p);

  p.parse(argc, argv);

  if (p.exist("help")){
    cout << p.usage();
    return 0;
  }

  if (!p.exist("model")) {
    cout << "output model filename must be argued." << endl;
    cout << p.usage();
    return -1;
  }

  size_t times = p.get<size_t>("iterate");

  rest = p.rest();

  if (rest.size() == 0) {
    proc_stdin(mic);
  }
  else {
    for (size_t i = 0; i < times; i++) {
      vector<string>::iterator it;
      for (it = rest.begin(); it != rest.end(); it++) {
        proc_file(mic, *it);
      }
    }
  }

  mic.save(p.get<string>("model").c_str());

  return 0;
}
