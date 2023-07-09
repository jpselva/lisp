#include <stdio.h>
#include <assert.h>
#include "../src/lisp.h"

#define testsuite(descr) printf("TEST: " descr "\n");

#define test(descr, cond)                         \
    ((cond) ?                                     \
        printf("  PASS: %s\n", descr) :          \
        printf(">>FAIL: %s (%s)\n", descr, #cond));
