#include "sset.h"

typedef struct entry_t entry_t;
struct entry_t {
    char* key;

    uint32_t next;
    uint32_t prev;
    uint32_t chain;
    uint32_t hash;
};

struct sset_t {
    entry_t* buf;
    uint32_t buf_alloced;
    uint32_t set_alloced;

    uint32_t* set;
    uint32_t size;

    uint32_t unused;
    uint32_t used;
};

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
static inline void pos(sset_t* m, uint32_t h, char* key, uint32_t* set_pos,
    uint32_t* buf_pos, uint32_t* chain)
{
    *set_pos = h % m->set_alloced;
    *buf_pos = m->set[*set_pos];
    *chain = SSET_NULL;

    while (*buf_pos != SSET_NULL && strcmp(m->buf[*buf_pos].key, key) != 0) {
        *chain = *buf_pos;
        *buf_pos = m->buf[*buf_pos].chain;
    }
}

static inline uint32_t unused_pop(sset_t* m)
{
    uint32_t r = m->unused;
    m->unused = m->buf[r].next;
    return r;
}

static inline void unused_push(sset_t* m, uint32_t e)
{
    m->buf[e].next = m->unused;
    m->unused = e;
}

static inline void used_push(sset_t* m, uint32_t e)
{
    m->buf[e].next = m->used;
    m->buf[e].prev = SSET_NULL;

    // if null this is segfault
    if (m->used != SSET_NULL) {
        m->buf[m->used].prev = e;
    }

    m->used = e;
}

static inline void used_pop(sset_t* m, uint32_t pos)
{
    uint32_t prev = m->buf[pos].prev;
    uint32_t next = m->buf[pos].next;
    if (next != SSET_NULL) {
        m->buf[next].prev = prev;
    }
    if (prev != SSET_NULL) {
        m->buf[prev].next = next;
    }
}

static inline int check_realloc(sset_t* m)
{
    if (m->unused != SSET_NULL) {
        return 0;
    }

    // update blocks
    {
        uint32_t new_buf_alloced = m->buf_alloced << 1;
        uint32_t new_set_alloced = m->set_alloced << 1;
        entry_t* new_buf = realloc(m->buf, new_buf_alloced * sizeof(*new_buf));
        uint32_t* new_set = realloc(m->set, new_set_alloced * sizeof(*new_set));

        if (new_buf == NULL || new_set == NULL) {
            free(new_buf);
            free(new_set);
            return -1;
        }

        memset(new_set, 0xFF, new_set_alloced * sizeof(*new_set));

        m->buf_alloced = new_buf_alloced;
        m->set_alloced = new_set_alloced;
        m->set = new_set;
        m->buf = new_buf;
    }

    // push new entries to unused stack
    {
        uint32_t iter;
        for (iter = m->size; iter < m->buf_alloced; iter++) {
            unused_push(m, iter);
        }
    }

    // set used stack to set
    {
        uint32_t set_pos;
        uint32_t buf_pos;
        uint32_t chain;
        uint32_t iter = m->used;
        entry_t* e;
        while (iter != SSET_NULL) {
            e = &m->buf[iter];
            e->chain = SSET_NULL;
            pos(m, e->hash, e->key, &set_pos, &buf_pos, &chain);

            if (chain != SSET_NULL) {
                m->buf[chain].chain = iter;
            } else {
                m->set[set_pos] = iter;
            }

            iter = m->buf[iter].next;
        }
    }
    return 1;
}

// init and free
// ----------------------------------------------------------------------------
sset_t* sset_create(uint32_t size)
{
    sset_t* m = malloc(sizeof(*m));
    if (m == NULL) {
        return NULL;
    }

    *m = (sset_t) {
        .buf_alloced = size,
        .set_alloced = size << 2,
        .unused = SSET_NULL,
        .used = SSET_NULL,
        .size = 0,
    };

    m->set = malloc(m->set_alloced * sizeof(*m->set));
    m->buf = malloc(m->buf_alloced * sizeof(*m->buf));

    if (m->set == NULL || m->buf == NULL) {
        free(m->set);
        free(m->buf);
        free(m);
        return NULL;
    }
    memset(m->set, 0xFF, m->set_alloced * sizeof(*m->set));

    // push new entries to unused stack
    {
        uint32_t iter;
        for (iter = 0; iter < m->buf_alloced; iter++) {
            unused_push(m, iter);
        }
    }
    return m;
}

void sset_free(sset_t* m)
{
    if (m == NULL) {
        return;
    }
    free(m->set);
    free(m->buf);
    free(m);
}

// public utility functions
// ----------------------------------------------------------------------------

uint32_t sset_size(sset_t* m) { return m->size; }

bool sset_contains(sset_t* m, char* key)
{
    uint32_t set_pos;
    uint32_t buf_pos;
    uint32_t chain;
    pos(m, HASH(key), key, &set_pos, &buf_pos, &chain);
    return buf_pos != SSET_NULL;
}

// insert set etc.
// ----------------------------------------------------------------------------

int sset_insert(sset_t* m, char* key)
{
    uint32_t set_pos;
    uint32_t buf_pos;
    uint32_t chain;
    uint32_t hash = HASH(key);
    pos(m, hash, key, &set_pos, &buf_pos, &chain);

    if (buf_pos != SSET_NULL) {
        return 1;
    }

    // realloc edge cases
    {
        int r = check_realloc(m);
        if (r < 0) {
            return -1;
        }
        if (r > 0) {
            pos(m, hash, key, &set_pos, &buf_pos, &chain);
        }
    }

    uint32_t t = unused_pop(m);
    used_push(m, t);
    if (chain != SSET_NULL) {
        m->buf[chain].chain = t;
    } else {
        m->set[set_pos] = t;
    }
    m->buf[t].key = key;
    m->buf[t].chain = SSET_NULL;
    m->buf[t].hash = hash;
    m->size++;
    return 0;
}

void sset_move_key(sset_t* m, char* key, char** dest_key)
{
    uint32_t set_pos;
    uint32_t buf_pos;
    uint32_t chain;
    pos(m, HASH(key), key, &set_pos, &buf_pos, &chain);

    if (buf_pos == SSET_NULL) {
        if (dest_key != NULL) {
            *dest_key = NULL;
        }
        return;
    }

    if (dest_key != NULL) {
        *dest_key = m->buf[buf_pos].key;
    }

    if (chain != SSET_NULL) {
        m->buf[chain].chain = SSET_NULL;
    } else {
        m->set[set_pos] = SSET_NULL;
    }

    used_pop(m, buf_pos);
    unused_push(m, buf_pos);
    m->size--;
}

// copy move semantics
// ----------------------------------------------------------------------------

void sset_copy_key(sset_t* m, char* key, char** dest_key)
{
    uint32_t set_pos;
    uint32_t buf_pos;
    uint32_t chain;
    pos(m, HASH(key), key, &set_pos, &buf_pos, &chain);

    if (buf_pos == SSET_NULL) {
        if (dest_key != NULL) {
            *dest_key = NULL;
        }
        return;
    }

    if (dest_key != NULL) {
        *dest_key = m->buf[buf_pos].key;
    }
}

// iter
// ----------------------------------------------------------------------------

sset_iter_t sset_iter_next(sset_t* m, sset_iter_t i)
{
    if (i == SSET_NULL) {
        return m->used;
    }

    return m->buf[i].next;
}

char* sset_iter_key(sset_t* m, sset_iter_t i) { return m->buf[i].key; }
