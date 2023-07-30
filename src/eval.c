#include "lisp.h"
#define TAIL_CALL_EXP(tc) ((tc)->car)
#define TAIL_CALL_ENV(tc) ((tc)->cdr)

Obj* make_tail_call(Obj* const* exp, Obj* const* env) {
    Obj* tc = alloc_cons(exp, env);
    tc->type = TAIL_CALL;
    return tc;
}

Obj* list_of_values(Obj* const* exps, Obj* const* env);
Obj* eval_compound(Obj* const* exps, Obj* const* env);

Obj* eval(Obj* const* exp, Obj* const* env) {
    DEF3(current_exp, current_env, value);

    *current_exp = *exp;
    *current_env = *env;
    
    for (;;) {
        switch ((*current_exp)->type) {
            case NUMBER:
            case STRING:
            case EMPTY_LIST:
                *value = *current_exp;
                break;
            case SYMBOL:
                *value = lookup(current_exp, current_env);
                break;
            case CONS: 
                *value = eval_compound(current_exp, current_env);
                break;
            default:
                error("exp of that type can't be evaluated");
        }

        if ((*value)->type == TAIL_CALL) {
            *current_exp = TAIL_CALL_EXP(*value);
            *current_env = TAIL_CALL_ENV(*value);
        } else {
            break;
        }
    }

    return RET(3, *value);
}

Obj* eval_compound(Obj* const* exp, Obj* const* env) {
    DEF3(operator, operands, expanded);

    *operator = (*exp)->car;
    *operator = eval(operator, env);
    *operands = (*exp)->cdr;

    if ((*operator)->type == SPECIAL_FORM) {
        return RET(3, (*operator)->special_form(operands, env)); 
    } else if ((*operator)->type == MACRO) {
        *expanded = expand_macro(operator, operands);
        return RET(3, make_tail_call(expanded, env));
    } else {
        *operands = list_of_values(operands, env);
        return RET(3, apply(operator, operands));
    }
}

Obj* list_of_values(Obj* const* exps, Obj* const* env) {
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

Obj* eval_sequence(Obj* const* exps, Obj* const* env) {
    DEF3(value, exp_scan, exp);

    *value = NIL;

    for (*exp_scan = *exps; *exp_scan != NIL; *exp_scan = (*exp_scan)->cdr) {
        if ((*exp_scan)->type != CONS) {
            error("eval_sequence expects a list of expressions");
        } 

        *exp = (*exp_scan)->car;

        if ((*exp_scan)->cdr == NIL) {
            break;
        } else {
            *value = eval(exp, env);
        }
    }

    *value = make_tail_call(exp, env);

    return RET(3, *value);
}

/* like eval_sequence, but without tail call for last exp */
Obj* eval_whole_sequence(Obj* const* body, Obj* const* env) {
    DEF3(exp_scan, exp, value);

    for (*exp_scan = *body; *exp_scan != NIL; *exp_scan = (*exp_scan)->cdr) {
        if ((*exp_scan)->type != CONS) {
            error("eval_whole_sequence expects list of expressions");
        }

        *exp = (*exp_scan)->car;
        *value = eval(exp, env);
    }

    return RET(3, *value);
}

