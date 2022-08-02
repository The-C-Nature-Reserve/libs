#include "smap.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define START_TEST() printf("TESTING: %s:\t", __func__);
#define END_TEST() printf("DONE\n");

void test_move(void)
{
    START_TEST();

    char* ss[] = { "Hello", "World", "I", "am", "Ben" };

    smap_t* m = smap_create(1);

    assert(smap_size(m) == 0);

    smap_insert(m, ss[0], (void*)1);
    smap_insert(m, ss[1], (void*)2);
    smap_insert(m, ss[2], (void*)3);
    smap_insert(m, ss[3], (void*)4);
    smap_insert(m, ss[4], (void*)5);

    assert(smap_size(m) == 5);

    char* key;
    char* value;

    smap_move(m, "Hello", &key, (void**)&value);
    assert(smap_size(m) == 4);
    assert(key == ss[0]);
    assert(value == (void*)1);

    smap_move(m, "Hello", &key, (void**)&value);
    assert(key == NULL);
    assert(value == NULL);

    smap_move(m, "World", &key, (void**)&value);
    assert(smap_size(m) == 3);
    assert(key == ss[1]);
    assert(value == (void*)2);

    smap_move(m, "World", &key, (void**)&value);
    assert(key == NULL);
    assert(value == NULL);

    smap_move(m, "I", &key, (void**)&value);
    assert(smap_size(m) == 2);
    assert(key == ss[2]);
    assert(value == (void*)3);

    smap_move(m, "I", &key, (void**)&value);
    assert(key == NULL);
    assert(value == NULL);

    smap_move(m, "am", &key, (void**)&value);
    assert(smap_size(m) == 1);
    assert(key == ss[3]);
    assert(value == (void*)4);

    smap_move(m, "am", &key, (void**)&value);
    assert(key == NULL);
    assert(value == NULL);

    smap_move(m, "Ben", &key, (void**)&value);
    assert(smap_size(m) == 0);
    assert(key == ss[4]);
    assert(value == (void*)5);

    smap_move(m, "Ben", &key, (void**)&value);
    assert(key == NULL);
    assert(value == NULL);

    smap_free(m);

    END_TEST();
}

void test_init(void)
{
    START_TEST();
    smap_t* m = smap_create(100);
    smap_free(m);
    END_TEST();
}

void test_size(void)
{
    START_TEST();
    smap_t* m = smap_create(1);
    assert(smap_size(m) == 0);

    smap_insert(m, "Hello", "value");
    assert(smap_size(m) == 1);

    smap_insert(m, "World", "value");
    assert(smap_size(m) == 2);

    smap_insert(m, "I", "value");
    assert(smap_size(m) == 3);

    smap_insert(m, "am", "value");
    assert(smap_size(m) == 4);

    smap_insert(m, "Ben", "value");
    assert(smap_size(m) == 5);

    smap_free(m);

    END_TEST();
}

void test_insert_get(void)
{
    START_TEST();
    char text[]
        = "egestas fringilla phasellus faucibus scelerisque eleifend donec "
          "pretium vulputate sapien nec sagittis aliquam malesuada bibendum "
          "arcu vitae elementum curabitur vitae nunc sed velit dignissim "
          "sodales ut eu sem integer vitae justo eget magna fermentum iaculis "
          "eu non diam phasellus vestibulum lorem sed risus ultricies "
          "tristique nulla aliquet enim tortor at auctor urna nunc id cursus "
          "metus aliquam eleifend mi in nulla posuere sollicitudin aliquam "
          "ultrices sagittis orci a scelerisque purus semper eget duis at "
          "tellus at urna condimentum mattis pellentesque id nibh tortor id "
          "aliquet lectus proin nibh nisl condimentum id venenatis a "
          "condimentum vitae sapien pellentesque habitant morbi tristique";

    smap_t* m = smap_create(1);

    char* s = text;
    char* start;
    char tmp;

    uint32_t values[100] = { 0 };
    uint32_t vid = 0;
    uint32_t* v;

    while (*s) {

        while (!isalpha(*s)) {
            s++;
        }

        start = s;
        while (isalpha(*s)) {
            s++;
        }

        tmp = *s;
        *s = '\0';

        v = smap_get(m, start);
        if (v == NULL) {
            v = smap_insert(m, start, &values[vid++]);
        }
        *v += 1;

        if (tmp != '\0') {
            s++;
        }
    }

    // tests
    assert(*(uint32_t*)smap_get(m, "morbi") == 1);
    assert(*(uint32_t*)smap_get(m, "habitant") == 1);
    assert(*(uint32_t*)smap_get(m, "lectus") == 1);
    assert(*(uint32_t*)smap_get(m, "mattis") == 1);
    assert(*(uint32_t*)smap_get(m, "condimentum") == 3);
    assert(*(uint32_t*)smap_get(m, "tellus") == 1);
    assert(*(uint32_t*)smap_get(m, "duis") == 1);
    assert(*(uint32_t*)smap_get(m, "justo") == 1);
    assert(*(uint32_t*)smap_get(m, "arcu") == 1);
    assert(*(uint32_t*)smap_get(m, "integer") == 1);
    assert(*(uint32_t*)smap_get(m, "ut") == 1);
    assert(*(uint32_t*)smap_get(m, "nisl") == 1);
    assert(*(uint32_t*)smap_get(m, "velit") == 1);
    assert(*(uint32_t*)smap_get(m, "tortor") == 2);
    assert(*(uint32_t*)smap_get(m, "sed") == 2);
    assert(*(uint32_t*)smap_get(m, "curabitur") == 1);
    assert(*(uint32_t*)smap_get(m, "sapien") == 2);
    assert(*(uint32_t*)smap_get(m, "vitae") == 4);
    assert(*(uint32_t*)smap_get(m, "egestas") == 1);
    assert(*(uint32_t*)smap_get(m, "sollicitudin") == 1);
    assert(*(uint32_t*)smap_get(m, "venenatis") == 1);
    assert(*(uint32_t*)smap_get(m, "elementum") == 1);
    assert(*(uint32_t*)smap_get(m, "ultrices") == 1);
    assert(*(uint32_t*)smap_get(m, "proin") == 1);
    assert(*(uint32_t*)smap_get(m, "nec") == 1);
    assert(*(uint32_t*)smap_get(m, "sagittis") == 2);
    assert(*(uint32_t*)smap_get(m, "phasellus") == 2);
    assert(*(uint32_t*)smap_get(m, "donec") == 1);
    assert(*(uint32_t*)smap_get(m, "fringilla") == 1);
    assert(*(uint32_t*)smap_get(m, "vulputate") == 1);
    assert(*(uint32_t*)smap_get(m, "auctor") == 1);
    assert(*(uint32_t*)smap_get(m, "pellentesque") == 2);
    assert(*(uint32_t*)smap_get(m, "sem") == 1);
    assert(*(uint32_t*)smap_get(m, "lorem") == 1);
    assert(*(uint32_t*)smap_get(m, "malesuada") == 1);
    assert(*(uint32_t*)smap_get(m, "faucibus") == 1);
    assert(*(uint32_t*)smap_get(m, "aliquam") == 3);
    assert(*(uint32_t*)smap_get(m, "eget") == 2);
    assert(*(uint32_t*)smap_get(m, "scelerisque") == 2);
    assert(*(uint32_t*)smap_get(m, "eleifend") == 2);
    assert(*(uint32_t*)smap_get(m, "nibh") == 2);
    assert(*(uint32_t*)smap_get(m, "pretium") == 1);
    assert(*(uint32_t*)smap_get(m, "risus") == 1);
    assert(*(uint32_t*)smap_get(m, "dignissim") == 1);
    assert(*(uint32_t*)smap_get(m, "magna") == 1);
    assert(*(uint32_t*)smap_get(m, "nunc") == 2);
    assert(*(uint32_t*)smap_get(m, "iaculis") == 1);
    assert(*(uint32_t*)smap_get(m, "non") == 1);
    assert(*(uint32_t*)smap_get(m, "at") == 3);
    assert(*(uint32_t*)smap_get(m, "ultricies") == 1);
    assert(*(uint32_t*)smap_get(m, "nulla") == 2);
    assert(*(uint32_t*)smap_get(m, "tristique") == 2);
    assert(*(uint32_t*)smap_get(m, "fermentum") == 1);
    assert(*(uint32_t*)smap_get(m, "vestibulum") == 1);
    assert(*(uint32_t*)smap_get(m, "aliquet") == 2);
    assert(*(uint32_t*)smap_get(m, "enim") == 1);
    assert(*(uint32_t*)smap_get(m, "sodales") == 1);
    assert(*(uint32_t*)smap_get(m, "urna") == 2);
    assert(*(uint32_t*)smap_get(m, "id") == 4);
    assert(*(uint32_t*)smap_get(m, "cursus") == 1);
    assert(*(uint32_t*)smap_get(m, "metus") == 1);
    assert(*(uint32_t*)smap_get(m, "bibendum") == 1);
    assert(*(uint32_t*)smap_get(m, "diam") == 1);
    assert(*(uint32_t*)smap_get(m, "orci") == 1);
    assert(*(uint32_t*)smap_get(m, "mi") == 1);
    assert(*(uint32_t*)smap_get(m, "in") == 1);
    assert(*(uint32_t*)smap_get(m, "posuere") == 1);
    assert(*(uint32_t*)smap_get(m, "purus") == 1);
    assert(*(uint32_t*)smap_get(m, "eu") == 2);
    assert(*(uint32_t*)smap_get(m, "a") == 2);
    assert(*(uint32_t*)smap_get(m, "semper") == 1);

    smap_free(m);
    END_TEST();
}

int main(void)
{
    test_init();
    test_insert_get();
    test_size();
    test_move();
    return 0;
}
