#ifndef EZSTR_H
#define EZSTR_H

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

char* ezstr_create();
void ezstr_free(char* s);
char* ezstr_cat_cstr(char* dest_s, char* src);
char* ezstr_cat(char* dest_s, char* src_s);
char* ezstr_cpy_cstr(char* dest_s, char* src);
char* ezstr_cpy(char* dest_s, char* src_s);

#endif
