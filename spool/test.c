#include "pool.h"
#include <stdio.h>

void hello(void* a)
{
    printf("Hello World %lu\n", (uintptr_t)a);
}

int main(void)
{
    pool_t* p = pool_create(4);

    pool_async(p, hello, 0);
    pool_async(p, hello, 0);
    pool_async(p, hello, 0);
    pool_async(p, hello, 0);
    pool_async(p, hello, 0);
    pool_async(p, hello, 0);
    pool_async(p, hello, 0);
    pool_async(p, hello, 0);
    pool_async(p, hello, 0);

    pool_free(p);
}
