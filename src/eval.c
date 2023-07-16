#include "lisp.h"

Obj* list_of_values(Obj** exps, Obj** env);

Obj* eval(Obj** exp, Obj** env) {
    switch ((*exp)->type) {
        case NUMBER:
        case STRING:
        case EMPTY_LIST:
            return *exp;
            break;
        case SYMBOL:
            return lookup(exp, env);
            break;
        case CONS: {
            DEF2(operator, operands);

            *operator = (*exp)->car;
            *operator = eval(operator, env);
            *operands = (*exp)->cdr;

            if ((*operator)->type == SPECIAL_FORM) {
                return RET(2, (*operator)->special_form(operands, env));
            } else {
                *operands = list_of_values(operands, env);
                return RET(2, apply(operator, operands));
            }
        }
        default:
            error("exp of that type can't be evaluated");
    }
}

Obj* eval_sequence(Obj** exps, Obj** env) {
    DEF3(value, exp_scan, exp);

    *value = NIL;

    for (*exp_scan = *exps; *exp_scan != NIL; *exp_scan = (*exp_scan)->cdr) {
        if ((*exp_scan)->type != CONS) {
            error("eval_sequence expects a list of expressions");
        }
        *exp = (*exp_scan)->car;
        *value = eval(exp, env);
    }

    return RET(3, *value);
}

Obj* list_of_values(Obj** exps, Obj** env) {
    DEF4(head, entry, exp_scan, tmp);
    *head = alloc_cons(&NIL, &NIL);
    *entry = *head;

    for (*exp_scan = *exps; *exp_scan != NIL; *exp_scan = (*exp_scan)->cdr) {
        if ((*exp_scan)->type != CONS) {
            error("list_of_values expects a list of expressions");
        }
        *tmp = (*exp_scan)->car;
        *tmp = eval(tmp, env);
        SET_CDR(*entry, alloc_cons(tmp, &NIL));
        *entry = (*entry)->cdr;
    }

    return RET(4, (*head)->cdr);
}
