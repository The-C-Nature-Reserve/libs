#ifndef CON_H
#define CON_H

#include "lib/cmp.h"
#include "lib/hash.h"
#include "lib/map.h"

#include <stdarg.h>
#include <assert.h>
#include <ctype.h>

typedef void*(*con_parser_t)(char**);

#define con_parser_map_create(p, ...) con_parser_map_create_(p, __VA_ARGS__, NULL)
map_t* con_parser_map_create_(char* p, ...);

void con_parser_map_free(map_t* tm);
map_t** con_parse(map_t* parser_map, uint64_t* len, char* s);

void* con_u64_parser(char** sp);

#endif
