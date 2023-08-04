#include "lisp.h"

static Obj constants[] = {
 { .type = BOOLEAN },
 { .type = BOOLEAN },
 { .type = EMPTY_LIST }, 
};

Obj* true_c = constants + 0;
Obj* false_c = constants + 1;
Obj* nil_c = constants + 2;

