#include "lisp.h"

// REMINDER: once gc is implemented, these two shouldn't be gc'ed
// trick: check if type > SPECIAL_FORM
Obj trueobj = { .type = BOOLEAN };
Obj falseobj = { .type = BOOLEAN };
Obj nilobj = { .type = EMPTY_LIST };

