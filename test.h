#ifndef TEST_H
#define TEST_H

#include <assert.h>

#define BLUE "\033[34m"
#define RESET "\033[39m"
#define GREEN "\033[32m"

#define START_TEST() printf(BLUE "[TESTING:]" RESET " %s:\t", __func__);
#define END_TEST() printf(GREEN "DONE\n" RESET);

#endif
