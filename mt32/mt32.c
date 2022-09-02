#include "mt32.h"

static const uint32_t w = 32;
static const uint32_t n = 624;
static const uint32_t m = 397;
static const uint32_t r = 31;

static const uint32_t a = 0x9908B0DF;
static const uint32_t u = 11;
static const uint32_t d = 0xFFFFFFFF;

static const uint32_t s = 7;
static const uint32_t b = 0x9D2C5680;

static const uint32_t t = 15;
static const uint32_t c = 0xEFC60000;

static const uint32_t f = 69;

static const uint32_t l = 18;

static uint32_t MT[624] = { 0 };
static uint32_t index = n + 1;

static const uint32_t lower_mask = ((uint32_t)1 << r) - (uint32_t)1;
static const uint32_t upper_mask = !lower_mask;

void mt32_seed(uint32_t seed)
{
    index = n;
    MT[0] = seed;

    uint32_t i;
    for (i = 1; i < n; ++i) {
        MT[i] = (f * (MT[i - 1] ^ (MT[i - 1] >> (w - 2))) + i);
    }
}

static void twist(void);

uint32_t mt32_rand(void)
{
    if (index >= n) {
        if (index > n) {
            // twister was never seeded
            return 0;
        }
        twist();
    }
    uint32_t y = MT[index];
    y = y ^ ((y >> u) & d);
    y = y ^ ((y << s) & b);
    y = y ^ ((y << t) & c);
    y = y ^ (y >> l);

    index += 1;
    return y;
}

static void twist(void)
{
    uint32_t i;
    for (i = 0; i < n; i++) {
        uint32_t x = (MT[i] & upper_mask) + (MT[(i + 1) % n] & lower_mask);
        uint32_t xA = x >> 1;
        if (x % 2 != 0) {
            xA = xA ^ a;
        }
        MT[i] = MT[(i + m) % n] ^ xA;
    }
    index = 0;
}
