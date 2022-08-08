#define BEAVER_AUTO_ASYNC
#define ASAW_LOCATION "lib/asaw.c"

#include "lib/beaver.h"

#define FLAGS "-g -Wall -Werror -Og"

module_t modules[] = {
    { .name = "smap", .src = "smap/smap.c" },
    { .name = "smap", .src = "smap/test.c" },

    { .name = "argparse", .src = "smap/smap.c" },
    { .name = "argparse", .src = "argparse/argparse.c" },
    { .name = "argparse", .src = "argparse/test.c" },

    { .name = "pool", .src = "pool/test.c" },
    { .name = "pool", .src = "pool/pool.c", .extra_flags = "-lpthread" },

};

uint32_t modules_len = sizeof(modules) / sizeof(*modules);

char* smap[] = { "smap", NULL };
char* argparse[] = { "argparse", NULL };
char* pool[] = { "pool", NULL };

void cr_smap(void)
{
    compile(smap, FLAGS);
    call_or_panic("./out");
}

void cr_pool(void)
{
    compile(pool, FLAGS);
    call_or_panic("./out");
}

void cr_argparse(void)
{
    compile(argparse, FLAGS);
    call_or_panic("./out");
}

void cr_all(void)
{
    cr_smap();
    cr_argparse();
    cr_pool();
}

int main(int argc, char** argv)
{
    auto_update(argv);

    if (argc == 1) {
        cr_all();
    } else if (strcmp(argv[1], "clean") == 0) {
        rm("$(find build/ -type f)");
        rm("out");
    } else if (strcmp(argv[1], "smap/") == 0) {
        cr_smap();
    } else if (strcmp(argv[1], "argparse/") == 0) {
        cr_argparse();
    } else if (strcmp(argv[1], "pool/") == 0) {
        cr_pool();
    } else {
    }
    return 0;
}
