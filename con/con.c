#include "con.h"

map_t* con_parser_map_create_(char* p, ...)
{
    map_t* tm = map_create(cmp_str, 8);

    if (tm == NULL) {
        return NULL;
    }

    // TODO: initialize ?
    va_list ap;
    va_start(ap, p);
    con_parser_t* parser = NULL;
    char* key = NULL;
    uint64_t key_len = 0;
    uint64_t hash = 0;

    do {
        // alloc key to own block
        {
            key_len = strlen(p);
            // TODO: check bad_alloc
            key = malloc(key_len + 1);
            key[key_len] = 0;
            memcpy(key, p, key_len);
        }

        // alloc parser to own block
        {
            // TODO: check bad_alloc
            parser = malloc(sizeof(*parser));
            *parser = va_arg(ap, con_parser_t);
        }

        // insert them into the map
        {
            hash = hash_str_djb2(p);
            map_insert(tm, hash, key, parser);
        }
    } while ((p = va_arg(ap, char*)) != NULL);

    return tm;
}

void con_parser_map_free(map_t* tm)
{
    if (tm == NULL) {
        return;
    }

    map_iterator_t iter = MAP_ITERATOR_INIT;
    map_iterator_t end = MAP_ITERATOR_END;

    while ((iter = map_iterator_next(tm, iter)) != end) {
        free(map_iterator_key(tm, iter));
        free(map_iterator_value(tm, iter));
    }

    map_free(tm);
}

// will be expecting entry on '{'
// and will leave on '}'
// will return parsed object

static map_t* con_parse_object_(map_t* parser_map, char** text)
{
    char* s = *text;
    assert(*s == '{');
    map_t* map = map_create(cmp_str, 8);

    char* start = NULL;
    char* key = NULL;
    void* value = NULL;
    con_parser_t* parser = NULL;
    uint64_t hash = 0;

    while (*s && *s != '}') {

        // find start of key
        {
            s++;
            while (*s && isspace(*s)) {
                s++;
            }
        }

        if(*s == '}') {
            break;
        }

        start = s;

        // find end of key
        {
            while (*s && !isspace(*s) && *s != '=') {
                s++;
            }
        }

        // alloc key and hash
        {
            assert(s != start); // strlen(key) > 0
            key = malloc(s - start + 1);
            strncpy(key, start, s - start);
            hash = hash_str_djb2(key);
        }

        // find parser
        {
            parser = map_get(parser_map, hash, key);
            if (parser == NULL) {
                // TODO nice error
            }
        }

        // advance to '='
        s = strchr(s, '=');
        assert(s);

        // get value and insert
        value = (*parser)(&s);
        map_insert(map, hash, key, value);
    }

    assert(*s == '}');
    *text = s;
    return map;
}

map_t** con_parse(map_t* parser_map, uint64_t* len, char* s)
{
    s = strchr(s, '{');

    uint64_t i = 0;
    *len = 4;
    map_t** maps = malloc(*len * sizeof(*maps));

    do {
        if (i >= *len) {
            *len <<= 1;
            maps = realloc(maps, *len * sizeof(*maps));
        }
        maps[i++] = con_parse_object_(parser_map, &s);
        s = strchr(s, '{');

    } while (s);
    return maps;
}

// con parser
// ----------------------------------------------------------------------------

static bool is_uint(char* s)
{
    while (*s) {
        if (!isdigit(*s)) {
            return 0;
        }
        s++;
    }
    return 1;
}

void* con_u64_parser(char** sp)
{
    char* s = *sp;
    assert(*s == '=');

    // advance to number
    {
        s++;
        while (isspace(*s)) {
            s++;
        }
    }

    char* start = s;

    // find end
    {
        while (*s != '{' && !isspace(*s) && *s != ',') {
            s++;
        }
    }

    // make null terminated string
    char tmp = *s;
    *s = 0;

    if (!is_uint(start)) {
        // TODO: error
    }

    // actually parse value
    uint64_t* value = NULL;
    {
        value = malloc(sizeof(*value));
        *value = atol(start);
    }

    // cleanup
    {
        *s = tmp;
        s = strpbrk(s, ",}");
        assert(s);
        *sp = s;
    }
    return value;
}

void con_move(map_t* con, char* key, char** key_dest, void** value_dest)
{
    uint64_t hash = hash_str_djb2(key);
}
