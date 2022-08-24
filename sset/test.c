#include "sset.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "../test.h"


void test_iter(void)
{
    START_TEST();
    sset_t* m = sset_create(1);
    char* ss[] = { "Hello", "World", "I", "am", "Ben" };

    sset_insert(m, ss[0]);
    sset_insert(m, ss[1]);
    sset_insert(m, ss[2]);
    sset_insert(m, ss[3]);
    sset_insert(m, ss[4]);

    sset_iter_t i = SSET_NULL;
    char* key;
    uint32_t count = 0;
    while ((i = sset_iter_next(m, i)) != SSET_NULL) {
        count++;
        key = sset_iter_key(m, i);
        assert(key);
    }

    assert(count == 5);

    sset_free(m);
    END_TEST();
}

void test_contains(void)
{
    START_TEST();
    sset_t* m = sset_create(1);

    char s[] = "Hello";
    assert(!sset_contains(m, s));
    sset_insert(m, s);
    assert(sset_contains(m, s));

    char s1[] = "World";
    assert(!sset_contains(m, s1));
    sset_insert(m, s1);
    assert(sset_contains(m, s1));

    char s2[] = "I am Ben";
    assert(!sset_contains(m, s2));
    sset_insert(m, s2);
    assert(sset_contains(m, s2));

    sset_free(m);
    END_TEST();
}

void test_copy_key(void)
{
    START_TEST();

    char* ss[] = { "Hello", "World", "I", "am", "Ben" };

    sset_t* m = sset_create(1);

    assert(sset_size(m) == 0);

    sset_insert(m, ss[0]);
    sset_insert(m, ss[1]);
    sset_insert(m, ss[2]);
    sset_insert(m, ss[3]);
    sset_insert(m, ss[4]);

    assert(sset_size(m) == 5);

    char* key;

    sset_copy_key(m, "Hello", &key);
    assert(sset_size(m) == 5);
    assert(key == ss[0]);

    sset_copy_key(m, "Hello", &key);
    assert(sset_size(m) == 5);
    assert(key == ss[0]);

    sset_copy_key(m, "World", &key);
    assert(sset_size(m) == 5);
    assert(key == ss[1]);

    sset_copy_key(m, "World", &key);
    assert(sset_size(m) == 5);
    assert(key == ss[1]);

    sset_copy_key(m, "I", NULL);
    assert(sset_size(m) == 5);

    sset_copy_key(m, "I", &key);
    assert(sset_size(m) == 5);
    assert(key == ss[2]);

    sset_copy_key(m, "am", &key);
    assert(sset_size(m) == 5);
    assert(key == ss[3]);

    sset_copy_key(m, "am", &key);
    assert(sset_size(m) == 5);
    assert(key == ss[3]);

    sset_copy_key(m, "Ben", NULL);
    assert(sset_size(m) == 5);

    sset_copy_key(m, "Ben", &key);
    assert(sset_size(m) == 5);
    assert(key == ss[4]);

    sset_free(m);

    END_TEST();
}

void test_move_key(void)
{
    START_TEST();

    char* ss[] = { "Hello", "World", "I", "am", "Ben" };

    sset_t* m = sset_create(1);

    assert(sset_size(m) == 0);

    sset_insert(m, ss[0]);
    sset_insert(m, ss[1]);
    sset_insert(m, ss[2]);
    sset_insert(m, ss[3]);
    sset_insert(m, ss[4]);

    assert(sset_size(m) == 5);

    char* key;

    sset_move_key(m, "Hello", &key);
    assert(sset_size(m) == 4);
    assert(key == ss[0]);

    sset_move_key(m, "Hello", &key);
    assert(key == NULL);

    sset_move_key(m, "World", &key);
    assert(sset_size(m) == 3);
    assert(key == ss[1]);

    sset_move_key(m, "World", &key);
    assert(key == NULL);

    sset_move_key(m, "I", NULL);
    assert(sset_size(m) == 2);

    sset_move_key(m, "I", &key);
    assert(key == NULL);

    sset_move_key(m, "am", &key);
    assert(sset_size(m) == 1);
    assert(key == ss[3]);

    sset_move_key(m, "am", &key);
    assert(key == NULL);

    sset_move_key(m, "Ben", NULL);
    assert(sset_size(m) == 0);

    sset_move_key(m, "Ben", &key);
    assert(key == NULL);

    sset_free(m);

    END_TEST();
}

void test_init(void)
{
    START_TEST();
    sset_t* m = sset_create(100);
    sset_free(m);
    END_TEST();
}

void test_size(void)
{
    START_TEST();
    sset_t* m = sset_create(1);
    assert(sset_size(m) == 0);

    sset_insert(m, "Hello");
    assert(sset_size(m) == 1);

    sset_insert(m, "World");
    assert(sset_size(m) == 2);

    sset_insert(m, "I");
    assert(sset_size(m) == 3);

    sset_insert(m, "am");
    assert(sset_size(m) == 4);

    sset_insert(m, "Ben");
    assert(sset_size(m) == 5);

    sset_free(m);

    END_TEST();
}

int main(void)
{
    test_init();
    test_iter();
    test_size();
    test_move_key();
    test_copy_key();
    test_contains();
    return 0;
}
