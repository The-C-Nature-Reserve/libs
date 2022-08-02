#ifndef SMAP_H
#define SMAP_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

typedef struct smap_t smap_t;

void smap_free(smap_t* m);
smap_t* smap_create(uint32_t size);
void* smap_insert(smap_t* m, char* key, void* value);
void* smap_get(smap_t* m, char* key);
uint32_t smap_size(smap_t* m);
void smap_move(smap_t* m, char* key, char** dest_key, void** dest_value);

#endif
