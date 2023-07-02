#include "lisp.h"

Obj* list_of_values(Obj* exps, Env env);

Obj* eval(Obj* exp, Env env) {
    Obj* operator, *operands;

    switch (exp->type) {
        case NUMBER:
        case STRING:
        case EMPTY_LIST:
            return exp;
            break;
        case SYMBOL:
            return lookup(exp, env);
            break;
        case CONS:
            operator = eval(exp->car, env);
            if (operator->type == SPECIAL_FORM) {
                return operator->special_form(exp->cdr, env);
            } else {
                operands = list_of_values(exp->cdr, env);
                return apply(operator, operands);
            }
            break;
        default:
            error("exp of that type can't be evaluated");
    }
}

Obj* eval_sequence(Obj* exps, Env env) {
    Obj* value = NIL;

    while (exps != NIL) {
        if (exps->type != CONS) {
            error("eval_sequence expects a list of expressions");
        }
        value = eval(exps->car, env);
        exps = exps->cdr;
    }

    return value;
}

Obj* list_of_values(Obj* exps, Env env) {
    Obj head = { .cdr = NIL };
    Obj* entry = &head;

    while (exps != NIL) {
        if (exps->type != CONS) {
            error("list_of_values expects a list of expressions");
        }
        entry->cdr = alloc_cons(eval(exps->car, env), NIL);
        entry = entry->cdr;
        exps = exps->cdr;
    }

    return head.cdr; 
}
