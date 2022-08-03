#include "smap.h"

typedef struct entry_t entry_t;
struct entry_t {
    char* key;
    void* value;

    uint32_t next;
    uint32_t prev;
    uint32_t chain;
    uint32_t hash;
};

struct smap_t {
    entry_t* buf;
    uint32_t buf_alloced;
    uint32_t map_alloced;

    uint32_t* map;
    uint32_t size;

    uint32_t unused;
    uint32_t used;
};

// just to be sure
#define SMAP_NULL ((uint32_t)0xFFFFFFFF)

// Hashing Algorithms
// ----------------------------------------------------------------------------

static inline uint32_t fnv_hash(char* s)
{
    uintptr_t h = 2166136261;
    for (; *s; s++) {
        h *= 16777619;
        h ^= *s;
    }
    return h;
}

#define HASH(key) fnv_hash(key)

// static utility functions
// ----------------------------------------------------------------------------

// TODO this can be a macro
static inline void pos(smap_t* m, uint32_t h, char* key, uint32_t* map_pos,
    uint32_t* buf_pos, uint32_t* chain)
{
    *map_pos = h % m->map_alloced;
    *buf_pos = m->map[*map_pos];
    *chain = SMAP_NULL;

    while (*buf_pos != SMAP_NULL && strcmp(m->buf[*buf_pos].key, key) != 0) {
        *chain = *buf_pos;
        *buf_pos = m->buf[*buf_pos].chain;
    }
}

static inline uint32_t unused_pop(smap_t* m)
{
    uint32_t r = m->unused;
    m->unused = m->buf[r].next;
    return r;
}

static inline void unused_push(smap_t* m, uint32_t e)
{
    m->buf[e].next = m->unused;
    m->unused = e;
}

static inline void used_push(smap_t* m, uint32_t e)
{
    m->buf[e].next = m->used;
    m->buf[e].prev = SMAP_NULL;

    // if null this is segfault
    if (m->used != SMAP_NULL) {
        m->buf[m->used].prev = e;
    }

    m->used = e;
}

static inline void used_pop(smap_t* m, uint32_t pos)
{
    uint32_t prev = m->buf[pos].prev;
    uint32_t next = m->buf[pos].next;
    if (next != SMAP_NULL) {
        m->buf[next].prev = prev;
    }
    if (prev != SMAP_NULL) {
        m->buf[prev].next = next;
    }
}

static inline int check_realloc(smap_t* m)
{
    if (m->unused != SMAP_NULL) {
        return 0;
    }

    // update blocks
    {
        uint32_t new_buf_alloced = m->buf_alloced << 1;
        uint32_t new_map_alloced = m->map_alloced << 1;
        entry_t* new_buf = realloc(m->buf, new_buf_alloced * sizeof(*new_buf));
        uint32_t* new_map = realloc(m->map, new_map_alloced * sizeof(*new_map));

        if (new_buf == NULL || new_map == NULL) {
            free(new_buf);
            free(new_map);
            return -1;
        }

        memset(new_map, 0xFF, new_map_alloced * sizeof(*new_map));

        m->buf_alloced = new_buf_alloced;
        m->map_alloced = new_map_alloced;
        m->map = new_map;
        m->buf = new_buf;
    }

    // push new entries to unused stack
    {
        uint32_t iter;
        for (iter = m->size; iter < m->buf_alloced; iter++) {
            unused_push(m, iter);
        }
    }

    // map used stack to map
    {
        uint32_t map_pos;
        uint32_t buf_pos;
        uint32_t chain;
        uint32_t iter = m->used;
        entry_t* e;
        while (iter != SMAP_NULL) {
            e = &m->buf[iter];
            e->chain = SMAP_NULL;
            pos(m, e->hash, e->key, &map_pos, &buf_pos, &chain);

            if (chain != SMAP_NULL) {
                m->buf[chain].chain = iter;
            } else {
                m->map[map_pos] = iter;
            }

            iter = m->buf[iter].next;
        }
    }
    return 1;
}

// init and free
// ----------------------------------------------------------------------------
smap_t* smap_create(uint32_t size)
{
    smap_t* m = malloc(sizeof(*m));
    if (m == NULL) {
        return NULL;
    }

    *m = (smap_t) {
        .buf_alloced = size,
        .map_alloced = size << 2,
        .unused = SMAP_NULL,
        .used = SMAP_NULL,
        .size = 0,
    };

    m->map = malloc(m->map_alloced * sizeof(*m->map));
    m->buf = malloc(m->buf_alloced * sizeof(*m->buf));

    if (m->map == NULL || m->buf == NULL) {
        free(m->map);
        free(m->buf);
        free(m);
        return NULL;
    }
    memset(m->map, 0xFF, m->map_alloced * sizeof(*m->map));

    // push new entries to unused stack
    {
        uint32_t iter;
        for (iter = 0; iter < m->buf_alloced; iter++) {
            unused_push(m, iter);
        }
    }
    return m;
}

void smap_free(smap_t* m)
{
    if (m == NULL) {
        return;
    }
    free(m->map);
    free(m->buf);
    free(m);
}

// public utility functions
// ----------------------------------------------------------------------------

uint32_t smap_size(smap_t* m) { return m->size; }

bool smap_contains(smap_t* m, char* key)
{
    uint32_t map_pos;
    uint32_t buf_pos;
    uint32_t chain;
    pos(m, HASH(key), key, &map_pos, &buf_pos, &chain);
    return buf_pos != SMAP_NULL;
}

// TODO: iter

// insert set etc.
// ----------------------------------------------------------------------------

void* smap_insert(smap_t* m, char* key, void* value)
{
    uint32_t map_pos;
    uint32_t buf_pos;
    uint32_t chain;
    uint32_t hash = HASH(key);
    pos(m, hash, key, &map_pos, &buf_pos, &chain);

    if (buf_pos != SMAP_NULL) {
        m->buf[buf_pos].value = value;
        return value;
    }

    // realloc edge cases
    {
        int r = check_realloc(m);
        if (r < 0) {
            return NULL;
        }
        if (r > 0) {
            pos(m, hash, key, &map_pos, &buf_pos, &chain);
        }
    }

    uint32_t t = unused_pop(m);
    used_push(m, t);
    if (chain != SMAP_NULL) {
        m->buf[chain].chain = t;
    } else {
        m->map[map_pos] = t;
    }
    m->buf[t].key = key;
    m->buf[t].value = value;
    m->buf[t].chain = SMAP_NULL;
    m->buf[t].hash = hash;
    m->size++;
    return value;
}

void* smap_get(smap_t* m, char* key)
{
    uint32_t map_pos;
    uint32_t buf_pos;
    uint32_t chain;
    pos(m, HASH(key), key, &map_pos, &buf_pos, &chain);

    if (buf_pos == SMAP_NULL) {
        return NULL;
    }
    return m->buf[buf_pos].value;
}

void smap_move_pair(smap_t* m, char* key, char** dest_key, void** dest_value)
{
    uint32_t map_pos;
    uint32_t buf_pos;
    uint32_t chain;
    pos(m, HASH(key), key, &map_pos, &buf_pos, &chain);

    if (buf_pos == SMAP_NULL) {
        if (dest_key != NULL) {
            *dest_key = NULL;
        }
        if (dest_value != NULL) {
            *dest_value = NULL;
        }
        return;
    }

    if (dest_key != NULL) {
        *dest_key = m->buf[buf_pos].key;
    }
    if (dest_value != NULL) {
        *dest_value = m->buf[buf_pos].value;
    }

    if (chain != SMAP_NULL) {
        m->buf[chain].chain = SMAP_NULL;
    } else {
        m->map[map_pos] = SMAP_NULL;
    }

    used_pop(m, buf_pos);
    unused_push(m, buf_pos);
    m->size--;
}

// copy move semantics
// ----------------------------------------------------------------------------

void smap_copy_pair(smap_t* m, char* key, char** dest_key, void** dest_value)
{
    uint32_t map_pos;
    uint32_t buf_pos;
    uint32_t chain;
    pos(m, HASH(key), key, &map_pos, &buf_pos, &chain);

    if (buf_pos == SMAP_NULL) {
        if (dest_key != NULL) {
            *dest_key = NULL;
        }
        if (dest_value != NULL) {
            *dest_value = NULL;
        }
        return;
    }

    if (dest_key != NULL) {
        *dest_key = m->buf[buf_pos].key;
    }
    if (dest_value != NULL) {
        *dest_value = m->buf[buf_pos].value;
    }
}

// iter
// ----------------------------------------------------------------------------

smap_iter_t smap_iter_next(smap_t* m, smap_iter_t i)
{
    if (i == SMAP_NULL) {
        return m->used;
    }

    return m->buf[i].next;
}

char* smap_iter_key(smap_t* m, smap_iter_t i) { return m->buf[i].key; }
void* smap_iter_value(smap_t* m, smap_iter_t i) { return m->buf[i].value; }
