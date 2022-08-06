#include "argparse.h"

static argflag_t* cmp_with_flags(
    argflag_t* flags, uint32_t flags_len, char* arg, uint32_t arg_len)
{
    argflag_t* iter = NULL;
    for (iter = flags; iter != flags + flags_len; ++iter) {
        if (strncmp(arg, iter->name, arg_len) == 0) {
            return iter;
        }
    }
    return NULL;
}

#define error_if(p, ...)                                                       \
    do {                                                                       \
        if (p) {                                                               \
            fprintf(stderr, "Error: ");                                        \
            fprintf(stderr, __VA_ARGS__);                                      \
            fprintf(stderr, "\n");                                             \
            exit(1);                                                           \
        }                                                                      \
    } while (0);

void parse_normal(argflag_t* flags, uint32_t flag_len, char** arg, char* start,
    char* end, smap_t* captured)
{
    smap_insert(captured, *arg, (void*)1);
    argflag_t* flag = cmp_with_flags(flags, flag_len, start, end - start);

    error_if(flag == NULL,
        "flag: '%s' not found if it was not meant to be a flag try "
        "escaping it with '--'",
        *arg);

    char* s;
    if (!flag->has_arg) {
        *(bool*)flag->target = 1;
        return;
    }

    if (*end == '=') {
        s = end + 1;
    } else if (arg[1] != NULL && strcmp(arg[1], "--") == 0) {
        s = arg[2];
        smap_insert(captured, s, (void*)1);
    } else {
        s = arg[1];
        smap_insert(captured, s, (void*)1);
    }
    error_if(s == NULL, "flag '%s' expects an argument", flag->name);
    *(char**)flag->target = s;
}

void parse_multiple(
    argflag_t* flags, uint32_t flags_len, char** arg, smap_t* captured)
{
    smap_insert(captured, *arg, (void*)1);
    char* s = *arg;
    s++;
    argflag_t* flag;
    for (; *s; s++) {
        flag = cmp_with_flags(flags, flags_len, s, 1);
        error_if(flag == NULL,
            "flag: '%c' not found if '%s' was not meant to be a flag try "
            "escaping it with '--'",
            *s, *arg);
        error_if(flag->has_arg,
            "concattinating flags is only with flags which do not expect "
            "arguments and are no longer than 1 char");
        *(bool*)flag->target = 1;
    }
}

void argparse(argflag_t* flags, uint32_t flags_len, int argc, char** argv,
    int* fargc, char** fargv)
{
    smap_t* captured = smap_create(argc);

    // filter argv for potential flags "everything with a -"
    // already esacapes "--" "-nonflag" sequenz
    char** potflags[argc];
    uint32_t potflags_len = 0;
    {
        char** iter;
        for (iter = argv; iter != argv + argc; iter++) {
            if (strcmp(*iter, "--") == 0) {
                smap_insert(captured, *iter, (void*)1);
                iter++;
                continue;
            }
            if (**iter == '-') {
                potflags[potflags_len++] = iter;
            }
        }
    }
    {
        char*** iter;
        for (iter = potflags; iter != potflags + potflags_len; iter++) {
            char* start = **iter;
            while (*start == '-') {
                start++;
            }
            char* end = start;
            while (*end && *end != '=') {
                end++;
            }

            if (start - **iter == 2 || end - start == 1) {
                parse_normal(flags, flags_len, *iter, start, end, captured);
            } else {
                parse_multiple(flags, flags_len, *iter, captured);
            }
        }
    }

    if (fargv != NULL) {
        char** iter;
        *fargv = 0;
        for (iter = argv; iter != argv + argc; iter++) {
            if (!smap_contains(captured, *iter)) {
                fargv[(*fargc)++] = *iter;
            }
        }
        fargv[*fargc] = NULL;
    }

    smap_free(captured);
}
