#include "lisp.h"
#include <stdio.h>
#include <stdlib.h>

void error(const char* msg) {
    printf(msg);
    exit(1);
}
