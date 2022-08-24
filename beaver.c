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

    { .name = "ezstr", .src = "ezstr/ezstr.c" },
    { .name = "ezstr", .src = "ezstr/test.c" },
};

uint32_t modules_len = sizeof(modules) / sizeof(*modules);

char* smap[] = { "smap", NULL };
char* argparse[] = { "argparse", NULL };
char* ezstr[] = { "ezstr", NULL };

void cr_smap(void)
{
    compile(smap, FLAGS);
    call_or_panic("./out");
}

void cr_ezstr(void)
{
    compile(ezstr, FLAGS);
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
    cr_ezstr();
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
    } else if (strcmp(argv[1], "ezstr/") == 0) {
        cr_ezstr();
    }
    return 0;
}
