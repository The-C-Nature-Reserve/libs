#include "ezstr.h"
#include <stdio.h>

#include "../test.h"

void test_ezstr_cat(void)
{
    START_TEST();

    char* s = ezstr_create();
    char* o = ezstr_create();

    assert(strcmp(s, o) == 0);
    assert(strlen(s) == 0);

    o = ezstr_cpy_cstr(o, "Hello World");

    assert(strcmp(o, "Hello World") == 0);

    s = ezstr_cpy(s, o);
    assert(strcmp(o, s) == 0);

    s = ezstr_cat(s, o);
    assert(strcmp(s, "Hello WorldHello World") == 0);

    s = ezstr_cat(s, o);
    assert(strcmp(s, "Hello WorldHello WorldHello World") == 0);

    s = ezstr_cat_cstr(s, "Another one bites the dust");
    assert(
        strcmp(s, "Hello WorldHello WorldHello WorldAnother one bites the dust")
        == 0);

    s = ezstr_cpy(s, o);
    assert(strcmp(o, s) == 0);

    ezstr_free(s);
    ezstr_free(o);

    END_TEST();
}

int main(void)
{
    test_ezstr_cat();
    return 0;
}
