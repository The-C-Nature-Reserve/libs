#ifndef SMAP_H
#define SMAP_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

typedef struct smap_t smap_t;
typedef uint32_t smap_iter_t;
#define SMAP_NULL ((uint32_t)0xFFFFFFFF)

void smap_free(smap_t* m);
smap_t* smap_create(uint32_t size);
void* smap_insert(smap_t* m, char* key, void* value);
void* smap_get(smap_t* m, char* key);
uint32_t smap_size(smap_t* m);
bool smap_contains(smap_t* m, char* key);

void smap_move_pair(smap_t* m, char* key, char** dest_key, void** dest_value);
void smap_copy_pair(smap_t* m, char* key, char** dest_key, void** dest_value);

smap_iter_t smap_iter_next(smap_t* m, smap_iter_t i);
char* smap_iter_key(smap_t* m, smap_iter_t i);
void* smap_iter_value(smap_t* m, smap_iter_t i);

#endif
