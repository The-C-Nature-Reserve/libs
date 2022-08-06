#ifndef ARGPARSE_H
#define ARGPARSE_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "../smap/smap.h"

typedef struct argflag_t argflag_t;
struct argflag_t {
    char name[32];
    void* target;
    bool has_arg;
};

void argparse(argflag_t* flags, uint32_t flags_len, int argc, char** argv,
    int* fargc, char** fargv);

#endif

