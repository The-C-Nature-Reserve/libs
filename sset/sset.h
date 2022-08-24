#ifndef SSET_H
#define SSET_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

typedef struct sset_t sset_t;
typedef uint32_t sset_iter_t;
#define SSET_NULL ((uint32_t)0xFFFFFFFF)

void sset_free(sset_t* m);
sset_t* sset_create(uint32_t size);
int sset_insert(sset_t* m, char* key);
uint32_t sset_size(sset_t* m);
bool sset_contains(sset_t* m, char* key);

void sset_move_key(sset_t* m, char* key, char** dest_key);
void sset_copy_key(sset_t* m, char* key, char** dest_key);

sset_iter_t sset_iter_next(sset_t* m, sset_iter_t i);
char* sset_iter_key(sset_t* m, sset_iter_t i);

#endif
