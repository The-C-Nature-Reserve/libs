#ifndef SPOOL_H
#define SPOOL_H

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct spool_t spool_t;

void spool_async(spool_t* p, void (*fn)(void*), void* arg);
pool_t* spool_create(uint32_t size);
void spool_free(spool_t* p);

#endif

