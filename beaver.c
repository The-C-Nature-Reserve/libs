#include "beaver.h"

#define FLAGS "-g -Wall -Werror -Og"

module_t modules[] = {
    { .name = "smap", .src = "smap/smap.c" },
    { .name = "smap", .src = "smap/test.c" },

    { .name = "argparse", .src = "smap/smap.c" },
    { .name = "argparse", .src = "argparse/argparse.c" },
    { .name = "argparse", .src = "argparse/test.c" },

};

uint32_t modules_len = sizeof(modules) / sizeof(*modules);

char* smap[] = { "smap", NULL };
char* argparse[] = { "argparse", NULL };

int main(int argc, char** argv)
{
    auto_update(argv);

    if (argc == 1) {
    } else if (strcmp(argv[1], "clean") == 0) {
        rm("build/*");
        rm("out");
    } else if (strcmp(argv[1], "smap/") == 0) {
        compile(smap, FLAGS);
        call_or_panic("./out");
    } else if (strcmp(argv[1], "argparse/") == 0) {
        compile(argparse, FLAGS);
        call_or_panic("./out");
    } else {
    }
    return 0;
}
