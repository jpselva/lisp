#include "lisp.h"

Obj* list_of_values(Obj* exps, Env env);

Obj* eval(Obj* exp, Env env) {
    Obj* operator, *operands;

    switch (exp->type) {
        case NUMBER:
        case STRING:
            return exp;
            break;
        case SYMBOL:
            return lookup(exp, env);
            break;
        case CONS:
            operator = eval(exp->car, env);
            operands = list_of_values(exp->cdr, env);
            return apply(operator, operands);
            break;
        default:
            error("exp of that type can't be evaluated");
    }
}

Obj* eval_sequence(Obj* exps, Env env) {
    Obj* value = NULL;

    while (exps != NULL) {
        value = eval(exps->car, env);
        exps = exps->cdr;
    }

    return value;
}

Obj* list_of_values(Obj* exps, Env env) {
    Obj head = { .cdr = NULL };
    Obj* entry = &head;

    while (exps != NULL) {
        entry->cdr = alloc_cons(eval(exps->car, env), NULL);
        entry = entry->cdr;
        exps = exps->cdr;
    }

    return head.cdr; 
}
