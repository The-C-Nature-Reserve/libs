#include "spool.h"

typedef struct task_t task_t;
struct task_t {
    void (*fn)(void*);
    void* arg;
    task_t* next;
};

struct pool_t {
    pthread_t* threads;

    task_t* stack;
    pthread_mutex_t mutex;
    pthread_cond_t cond;

    uint32_t size;
    bool alive;
};

static void* work_(void* a)
{
    spool_t* p = a;
    bool alive = 1;
    task_t* t = NULL;

    while (alive || t != NULL) {
        pthread_mutex_lock(&p->mutex);
        while (p->alive && p->stack == NULL) {
            pthread_cond_wait(&p->cond, &p->mutex);
        }
        t = p->stack;
        if (p->stack != NULL) {
            p->stack = p->stack->next;
        }
        alive = p->alive;
        pthread_mutex_unlock(&p->mutex);

        if (t != NULL) {
            t->fn(t->arg);
            free(t);
        }
    }

    return NULL;
}

spool_t* pool_create(uint32_t size)
{
    spool_t* p = malloc(sizeof(*p));
    if (p == NULL) {
        return NULL;
    }

    p->size = size;
    p->alive = 1;
    p->stack = NULL;
    p->threads = malloc(size * sizeof(*p->threads));

    if (p->threads == NULL) {
        free(p);
        return NULL;
    }

    p->cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
    p->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;

    pthread_t* t;
    for (t = p->threads; t != p->threads + p->size; t++) {
        if (pthread_create(t, NULL, work_, p) != 0) {
            goto spool_create_error_;
        }
    }

    return p;

spool_create_error_:
    pthread_mutex_lock(&p->mutex);
    p->alive = 0;
    pthread_mutex_unlock(&p->mutex);
    pthread_cond_broadcast(&p->cond);

    for (; t != p->threads - 1; t--) {
        pthread_join(*t, NULL);
    }

    return NULL;
}

void spool_free(pool_t* p)
{
    if (p == NULL) {
        return;
    }
    pthread_mutex_lock(&p->mutex);
    p->alive = 0;
    pthread_mutex_unlock(&p->mutex);
    pthread_cond_broadcast(&p->cond);

    pthread_t* t;
    for (t = p->threads; t != p->threads + p->size; t++) {
        pthread_join(*t, NULL);
    }

    free(p->threads);
    free(p);
}

void spool_async(pool_t* p, void (*fn)(void*), void* arg)
{
    task_t* t = malloc(sizeof(*t));
    t->fn = fn;
    t->arg = arg;

    pthread_mutex_lock(&p->mutex);
    t->next = p->stack;
    p->stack = t;
    pthread_mutex_unlock(&p->mutex);
    pthread_cond_signal(&p->cond);
}
