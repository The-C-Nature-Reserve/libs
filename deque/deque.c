#include "deque.h"

typedef struct entry_t entry_t;
struct entry_t {
    void* val;

    entry_t* prev;
    entry_t* next;
};

struct deque_t {
    uint64_t size;
    entry_t* head;
    entry_t* tail;
};
