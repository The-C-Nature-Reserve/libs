#include "argparse.h"
#include <assert.h>
#include <stdio.h>

#define BLUE "\033[34m"
#define RESET "\033[39m"
#define GREEN "\033[32m"

#define START_TEST() printf(BLUE "[TESTING:]" RESET " %s:\t", __func__);
#define END_TEST() printf(GREEN "DONE\n" RESET);

void test_single_lstr(void)
{
    START_TEST();

    char* first = NULL;
    char* second = NULL;

    argflag_t flags[] = {
        { .name = "first", .target = &first, .has_arg = 1 },
        { .name = "second", .target = &second, .has_arg = 1 },
    };
    uint32_t flags_len = sizeof(flags) / sizeof(*flags);

    {
        first = NULL;
        second = NULL;
        char* argv[] = { "./prog", "--first", "Hello", "--second", "World" };
        int argc = sizeof(argv) / sizeof(*argv);

        argparse(flags, flags_len, argc, argv, NULL, NULL);

        assert(strcmp(first, "Hello") == 0);
        assert(strcmp(second, "World") == 0);
    }

    {
        first = NULL;
        second = NULL;
        char* argv[] = { "./prog", "--first=Hello", "--second=World" };
        int argc = sizeof(argv) / sizeof(*argv);

        char* fargv[argc];
        int fargc = 0;

        argparse(flags, flags_len, argc, argv, &fargc, fargv);

        assert(strcmp(first, "Hello") == 0);
        assert(strcmp(second, "World") == 0);

        assert(fargc == 1);
        assert(fargv[0] == argv[0]);
        assert(fargv[1] == NULL);
    }

    {
        first = NULL;
        second = NULL;
        char* argv[] = { "./prog", "Hello", "--first", "--", "-World", "World",
            "--second=World" };
        int argc = sizeof(argv) / sizeof(*argv);

        char* fargv[argc];
        int fargc = 0;
        argparse(flags, flags_len, argc, argv, &fargc, fargv);

        assert(strcmp(first, "-World") == 0);
        assert(strcmp(second, "World") == 0);

        assert(fargc == 3);
        assert(fargv[0] == argv[0]);
        assert(fargv[1] == argv[1]);
        assert(fargv[2] == argv[5]);
        assert(fargv[3] == NULL);
    }

    END_TEST();
}

void test_single_str(void)
{
    START_TEST();

    char* a = NULL;
    char* b = NULL;
    argflag_t flags[] = {
        { .name = "a", .target = &a, .has_arg = 1 },
        { .name = "b", .target = &b, .has_arg = 1 },
    };
    uint32_t flags_len = sizeof(flags) / sizeof(*flags);

    {
        a = NULL;
        b = NULL;
        char* argv[] = { "./prog", "-a", "aarg", "Hello World", "-b", "barg" };
        int argc = sizeof(argv) / sizeof(*argv);

        argparse(flags, flags_len, argc, argv, NULL, NULL);

        assert(strcmp(a, "aarg") == 0);
        assert(strcmp(b, "barg") == 0);
    }

    {
        a = NULL;
        b = NULL;
        char* argv[] = { "./prog", "-a=aarg", "Hello World", "-b=barg" };
        int argc = sizeof(argv) / sizeof(*argv);

        argparse(flags, flags_len, argc, argv, NULL, NULL);

        assert(strcmp(a, "aarg") == 0);
        assert(strcmp(b, "barg") == 0);
    }

    {
        a = NULL;
        b = NULL;
        char* argv[]
            = { "./prog", "-a", "--", "-aarg", "Hello World", "-b=barg" };
        int argc = sizeof(argv) / sizeof(*argv);

        argparse(flags, flags_len, argc, argv, NULL, NULL);

        assert(strcmp(a, "-aarg") == 0);
        assert(strcmp(b, "barg") == 0);
    }

    END_TEST();
}

void test_single_lbool(void)
{
    START_TEST();
    bool first = 0;
    bool second = 0;

    argflag_t flags[] = {
        { .name = "first", .target = &first, .has_arg = 0 },
        { .name = "second", .target = &second, .has_arg = 0 },
    };
    uint32_t flags_len = sizeof(flags) / sizeof(*flags);

    {
        first = 0;
        second = 0;
        char* argv[] = { "./prog", "--first", "World!", "--second" };
        int argc = sizeof(argv) / sizeof(*argv);

        argparse(flags, flags_len, argc, argv, NULL, NULL);

        assert(first);
        assert(second);
    }
    {
        first = 0;
        second = 0;
        char* argv[] = { "./prog", "World!", "--second" };
        int argc = sizeof(argv) / sizeof(*argv);

        argparse(flags, flags_len, argc, argv, NULL, NULL);

        assert(!first);
        assert(second);
    }

    END_TEST();
}

void test_single_bool(void)
{
    START_TEST();
    bool c = 0;
    bool f = 0;
    argflag_t flags[] = {
        { .name = "f", .target = &f, .has_arg = 0 },
        { .name = "c", .target = &c, .has_arg = 0 },
    };
    uint32_t flags_len = sizeof(flags) / sizeof(*flags);

    {
        char* argv[] = { "./prog", "-f", "World!", "-c" };
        int argc = sizeof(argv) / sizeof(*argv);

        argparse(flags, flags_len, argc, argv, NULL, NULL);

        assert(c);
        assert(f);
    }

    {
        c = 0;
        f = 0;
        char* argv[] = { "./prog", "World!", "-c" };
        int argc = sizeof(argv) / sizeof(*argv);

        argparse(flags, flags_len, argc, argv, NULL, NULL);

        assert(c);
        assert(!f);
    }

    {
        c = 0;
        f = 0;
        char* argv[] = { "./prog", "World!", "--", "-c" };
        int argc = sizeof(argv) / sizeof(*argv);

        char* fargv[argc];
        int fargc = 0;

        argparse(flags, flags_len, argc, argv, &fargc, fargv);

        assert(!c);
        assert(!f);

        assert(fargc == 3);
        assert(fargv[0] == argv[0]);
        assert(fargv[1] == argv[1]);
        assert(fargv[2] == argv[3]);
    }
    END_TEST();
}

void test_multiple_bool(void)
{
    START_TEST();
    bool c = 0;
    bool f = 0;
    argflag_t flags[] = {
        { .name = "f", .target = &f, .has_arg = 0 },
        { .name = "c", .target = &c, .has_arg = 0 },
    };
    uint32_t flags_len = sizeof(flags) / sizeof(*flags);

    {
        char* argv[] = { "./prog", "-cf", "World!" };
        int argc = sizeof(argv) / sizeof(*argv);

        argparse(flags, flags_len, argc, argv, NULL, NULL);

        assert(c);
        assert(f);
    }

    {
        c = 0;
        f = 0;
        char* argv[] = { "./prog", "World!", "-c" };
        int argc = sizeof(argv) / sizeof(*argv);

        argparse(flags, flags_len, argc, argv, NULL, NULL);

        assert(c);
        assert(!f);
    }

    {
        c = 0;
        f = 0;
        char* argv[] = { "./prog", "World!", "--", "-c" };
        int argc = sizeof(argv) / sizeof(*argv);

        char* fargv[argc];
        int fargc = 0;

        argparse(flags, flags_len, argc, argv, &fargc, fargv);

        assert(!c);
        assert(!f);

        assert(fargc == 3);
        assert(fargv[0] == argv[0]);
        assert(fargv[1] == argv[1]);
        assert(fargv[2] == argv[3]);
    }
    END_TEST();
}

int main(void)
{
    test_multiple_bool();
    test_single_lbool();
    test_single_lstr();
    test_single_str();
    test_single_bool();
    return 0;
}
