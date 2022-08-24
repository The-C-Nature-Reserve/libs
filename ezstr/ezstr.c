#include "ezstr.h"

#define INIT_SIZE ((uint64_t)64)

typedef struct head_t head_t;
struct head_t {
    uint64_t size;
    uint64_t len;
};

static head_t* get_head(char* s)
{
    return (head_t*)((uint8_t*)s - sizeof(head_t));
}

char* ezstr_create()
{
    uint8_t* b = malloc(sizeof(head_t) * INIT_SIZE);
    head_t* h = (head_t*)b;
    h->size = INIT_SIZE;
    h->len = 0;
    char* s = (char*)(b + sizeof(head_t));
    *s = 0;
    return s;
}

void ezstr_free(char* s)
{
    if (s == NULL) {
        return;
    }
    head_t* h = get_head(s);
    free(h);
}

char* ezstr_cat(char* dest_s, char* src_s)
{
    head_t* dest = get_head(dest_s);
    head_t* src = get_head(src_s);

    if (src->len + dest->len + 1 >= dest->size) {
        uint64_t new_size = dest->size << 1;
        head_t* new_dest = realloc(dest, new_size + sizeof(head_t));
        if (new_dest == NULL) {
            return NULL;
        }
        dest = new_dest;
        dest->size = new_size;
        dest_s = (char*)((uint8_t*)dest + sizeof(head_t));
    }
    memcpy(dest_s + dest->len, src_s, src->len);
    dest->len += src->len;
    dest_s[dest->len] = 0;
    return dest_s;
}

char* ezstr_cat_cstr(char* dest_s, char* src)
{
    head_t* dest = get_head(dest_s);
    uint64_t src_len = strlen(src);

    if (src_len + dest->len + 1 >= dest->size) {
        uint64_t new_size = dest->size << 1;
        head_t* new_dest = realloc(dest, new_size + sizeof(head_t));
        if (new_dest == NULL) {
            return NULL;
        }
        dest = new_dest;
        dest->size = new_size;
        dest_s = (char*)((uint8_t*)dest + sizeof(head_t));
    }
    memcpy(dest_s + dest->len, src, src_len);
    dest->len += src_len;
    dest_s[dest->len] = 0;
    return dest_s;

}

char* ezstr_clear(char* s)
{
    head_t* h = get_head(s);
    h->len = 0;
    *s = 0;
    return s;
}

char* ezstr_cpy(char* dest_s, char* src_s) 
{
    head_t* dest = get_head(dest_s);
    head_t* src = get_head(src_s);

    if (src->len + 1 >= dest->size) {
        uint64_t new_size = src->size + 1;
        head_t* new_dest = realloc(dest, new_size + sizeof(head_t));
        if (new_dest == NULL) {
            return NULL;
        }
        dest = new_dest;
        dest->size = new_size;
        dest_s = (char*)((uint8_t*)dest + sizeof(head_t));
    }
    memcpy(dest_s, src_s, src->len);
    dest->len = src->len;
    dest_s[dest->len] = 0;
    return dest_s;
}

char* ezstr_cpy_cstr(char* dest_s, char* src)
{
    head_t* dest = get_head(dest_s);
    uint64_t src_len = strlen(src);

    if (src_len + 1 >= dest->size) {
        uint64_t new_size = src_len + 1;
        head_t* new_dest = realloc(dest, new_size + sizeof(head_t));
        if (new_dest == NULL) {
            return NULL;
        }
        dest = new_dest;
        dest->size = new_size;
        dest_s = (char*)((uint8_t*)dest + sizeof(head_t));
    }
    memcpy(dest_s, src, src_len);
    dest->len = src_len;
    dest_s[dest->len] = 0;
    return dest_s;

}
