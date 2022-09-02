#include "mt32.h"
#include <stdio.h>
#include <time.h>

#define NUM_TESTS 10

int main(void)
{
    mt32_seed(time(NULL));

    uint32_t i;
    for(i = 0; i < NUM_TESTS; ++i) {
        printf("%u\n", mt32_rand());
    }

    return 0;
}
