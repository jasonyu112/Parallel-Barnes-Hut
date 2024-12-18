#pragma once
#ifndef _ARGPARSE_H
#define _ARGPARSE_H

#include "ya_getopt.h"
#include <stdlib.h>
#include <iostream>

struct options_t {
    char* in_file;
    char* out_file;
    int steps;
    double theta;
    double dt;
    bool v;
};

void get_opts(int argc, char** argv, struct options_t* opts);
#endif
