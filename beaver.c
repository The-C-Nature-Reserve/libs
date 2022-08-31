#include "lib/beaver.h"

#define FLAGS "-g -Wall -Werror -Og"

module_t modules[] = {
    { .name = "smap", .src = "smap/smap.c" },
    { .name = "smap", .src = "smap/test.c" },

    { .name = "sset", .src = "sset/sset.c" },
    { .name = "sset", .src = "sset/test.c" },

    { .name = "argparse", .src = "smap/smap.c" },
    { .name = "argparse", .src = "argparse/argparse.c" },
    { .name = "argparse", .src = "argparse/test.c" },

    { .name = "ezstr", .src = "ezstr/ezstr.c" },
    { .name = "ezstr", .src = "ezstr/test.c" },
};

uint32_t modules_len = sizeof(modules) / sizeof(*modules);

char* smap[] = { "smap", NULL };
char* sset[] = { "sset", NULL };
char* argparse[] = { "argparse", NULL };
char* ezstr[] = { "ezstr", NULL };

void cr_smap(void)
{
    compile(smap, FLAGS);
    call_or_panic("./out");
}

void cr_sset(void)
{
    compile(sset, FLAGS);
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
    prepare_all(FLAGS);
    cr_smap();
    cr_sset();
    cr_argparse();
    cr_ezstr();
}

#define VAL(func, name)                                                        \
    {                                                                          \
        func();                                                                \
        call_or_panic("valgrind --log-file=tmp_log.log ./out");                \
        call_or_panic("echo \"==== " name " ====\" >> log.log");               \
        call_or_panic("cat tmp_log.log >> log.log");                           \
    }

void valgrind(void)
{
    prepare_all(FLAGS);
    call_or_panic("echo \"VALGRIND\" > log.log");

    VAL(cr_smap, "SMAP");
    VAL(cr_sset, "SSET");
    VAL(cr_argparse, "ARGPARSE");
    VAL(cr_ezstr, "EZSTR");

    rm("tmp_log.log");
}

int main(int argc, char** argv)
{
    auto_update(argv);

    if (argc == 1) {
        cr_all();
    } else if (strcmp(argv[1], "clean") == 0) {
        rm("$(find build/ -type f)");
        rm("out");
        rm("log.log");
    } else if (strcmp(argv[1], "smap/") == 0) {
        cr_smap();
    } else if (strcmp(argv[1], "sset/") == 0) {
        cr_sset();
    } else if (strcmp(argv[1], "argparse/") == 0) {
        cr_argparse();
    } else if (strcmp(argv[1], "ezstr/") == 0) {
        cr_ezstr();
    } else if (strcmp(argv[1], "valgrind") == 0) {
        valgrind();
    }
    return 0;
}
