#include "../smap.h"

#include <ctype.h>
#include <stdio.h>
#include <assert.h>

char* read_file(char* path)
{
    FILE* f = fopen(path, "r");
    fseek(f, 0, SEEK_END);
    uint32_t len = ftell(f);
    rewind(f);

    char* t = malloc(len + 1);
    fread(t, 1, len, f);
    t[len] = 0;

    fclose(f);
    return t;
}

char* get_word(char* text)
{
    static char* s = NULL;
    if (text != NULL) {
        s = text;
        return NULL;
    }

    if(*s == '\0') {
        return NULL;
    }

    while (isspace(*s)) {
        s++;
    }
    char* r = s;
    while (!isspace(*s)) {
        s++;
    }
    if (*s) {
        *s = 0;
        s++;
    }
    return r;
}

int main(void)
{
    char* text = read_file("lorem");
    get_word(text);

    char* s;

    smap_t* m = smap_create(1);

    uint32_t* v;

    while ((s = get_word(NULL)) != NULL) {
        v = smap_get(m, s);
        if (v == NULL) {
            v = calloc(1, sizeof(*v));
            v = smap_insert(m, s, v);
        }
        *v += 1;
    }

    smap_iter_t i = SMAP_NULL;
    while ((i = smap_iter_next(m, i)) != SMAP_NULL) {
        v = smap_iter_value(m, i);
        printf("%s: %u\n", smap_iter_key(m, i), *v);
        free(v);
    }

    free(text);
    smap_free(m);
}
