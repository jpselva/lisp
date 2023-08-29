#include "lisp.h"
#define islist(obj) ((obj->type == CONS) || obj == NIL)

static int length(Obj* const* args) {
    Obj* scan = *args;
    int len;

    for (len = 0; scan != NIL; scan = scan->cdr) {
        if (scan->type != CONS) {
            return -1; // not a list
        }
        len++;
    }

    return len;
}

/**** Primitive functions ****/

static Obj* car_p(Obj* const* args) {
    if (length(args) != 1) {
        error("car takes exactly one argument");
    }
    if ((*args)->car->type != CONS) {
        error("can't take car of non-cons object");
    }
    return (*args)->car->car;
}

static Obj* cdr_p(Obj* const* args) {
    if (length(args) != 1) {
        error("cdr takes only one argument");
    }
    if ((*args)->car->type != CONS) {
        error("can't take cdr of non-cons object");
    }
    return (*args)->car->cdr;
}

static Obj* cons_p(Obj* const* args) {
    DEF2(car, cdr);

    if (length(args) != 2) {
        error("cons takes exactly two arguments");
    }
    *car = (*args)->car;
    *cdr = (*args)->cdr->car;

    return RET(2, alloc_cons(car, cdr));
} 

static Obj* eq_p(Obj* const* args) {
    if (length(args) != 2) {
        error("eq? takes exactly two arguments");
    }
    return ((*args)->car == (*args)->cdr->car) ? TRUE : FALSE;
}

static  Obj* plus_p(Obj* const* args) {
    int result = 0;

    for (Obj* num = *args; num != NIL; num = num->cdr) {
        if (num->car->type != NUMBER) {
            error("+ takes only numbers");
        }
        result += num->car->number;
    }

    return alloc_number(result);
}

static Obj* minus_p(Obj* const* args) {
    if (length(args) < 1) {
        error("- expects at least one argument");
    } else if ((*args)->car->type != NUMBER) {
        error("- takes only numbers");
    }

    int result = (*args)->car->number;

    for (Obj* num = (*args)->cdr; num != NIL; num = num->cdr) {
        if (num->car->type != NUMBER) {
            error("- takes only numbers");
        }
        result -= num->car->number;
    }

    return alloc_number(result);
}

static Obj* equal_p(Obj* const* args) {
   if (length(args) != 2) {
       error("= takes exactly two arguments");
   }
   if (((*args)->car->type != NUMBER) || ((*args)->cdr->car->type != NUMBER)) {
       error("both arguments to = must be numbers");
   }

   return ((*args)->car->number == (*args)->cdr->car->number) ? TRUE : FALSE;
}

static Obj* gt_p(Obj* const* args) {
   if (length(args) != 2) {
       error("> takes exactly two arguments");
   }
   if (((*args)->car->type != NUMBER) || ((*args)->cdr->car->type != NUMBER)) {
       error("both arguments to = must be numbers");
   }

   return ((*args)->car->number > (*args)->cdr->car->number) ? TRUE : FALSE;
}

static Obj* lt_p(Obj* const* args) {
   if (length(args) != 2) {
       error("< takes exactly two arguments");
   }
   if (((*args)->car->type != NUMBER) || ((*args)->cdr->car->type != NUMBER)) {
       error("both arguments to = must be numbers");
   }

   return ((*args)->car->number < (*args)->cdr->car->number) ? TRUE : FALSE;
}

static Obj* write_p(Obj* const* args) {
    DEF1(obj);

    if (length(args) != 1) {
        error("write takes 1 argument");
    }

    *obj = (*args)->car;
    write(obj);
    return RET(1, NIL);
}

static Obj* list_p(Obj* const* args) {
    return *args;
}

/**** Special forms ****/

static Obj* quote_sf(Obj* const* exps, Obj* const* env) {
    if (length(exps) != 1) {
        error("quote expects exactly one expression");
    }
    return (*exps)->car;
}

static Obj* or_sf(Obj* const* exps, Obj* const* env) {
    DEF2(exp_scan, exp);

    Obj* result = FALSE;

    for(*exp_scan = *exps; *exp_scan != NIL; *exp_scan = (*exp_scan)->cdr) {
        if ((*exp_scan)->type != CONS) {
            error("incorrect 'or' syntax");
        }

        *exp = (*exp_scan)->car;

        if ((*exp_scan)->cdr == NIL) {
            result = make_tail_call(exp, env);
            break;
        }

        if ((result = eval(exp, env)) != FALSE) {
            break;
        }
    }

    return RET(2, result);
}

static Obj* and_sf(Obj* const* exps, Obj* const* env) {
    DEF2(exp_scan, exp);

    Obj* result = TRUE;

    for(*exp_scan = *exps; *exp_scan != NIL; *exp_scan = (*exp_scan)->cdr) {
        if ((*exp_scan)->type != CONS) {
            error("incorrect 'and' syntax");
        }

        *exp = (*exp_scan)->car;

        if ((*exp_scan)->cdr == NIL) {
            result = make_tail_call(exp, env);
            break;
        }

        if ((result = eval(exp, env)) == FALSE) {
            break;
        }
    }

    return RET(2, result);
}

static Obj* if_sf(Obj* const* exps, Obj* const* env) {
    DEF3(predicate, consequent, alternative);

    int exps_length = length(exps);

    if ((exps_length != 2) && (exps_length != 3)) {
        error("if expects 2 or 3 expressions");
    }

    *predicate = (*exps)->car;
    *consequent = (*exps)->cdr->car;
    *alternative = (exps_length == 3) ? (*exps)->cdr->cdr->car : NIL;

    if (eval(predicate, env) != FALSE) {
        return RET(3, make_tail_call(consequent, env));
    } else if (exps_length == 3) {
        return RET(3, make_tail_call(alternative, env));
    } else {
        return RET(3, TRUE);
    }
}

static Obj* define_sf(Obj* const* exps, Obj* const* env) {
    DEF2(var, value);

    if (length(exps) != 2 || (*exps)->car->type != SYMBOL) {
        error("invalid define syntax");
    }

    *var = (*exps)->car;
    *value = (*exps)->cdr->car;
    *value = eval(value, env);
    define_variable(var, value, env);

    return RET(2, *value);
}

static Obj* set_sf(Obj* const* exps, Obj* const* env) {
    DEF2(var, value);

    if (length(exps) != 2 || (*exps)->car->type != SYMBOL) {
        error("invalid set! syntax");
    }

    *var = (*exps)->car;
    *value = (*exps)->cdr->car;
    *value = eval(value, env);
    set_variable(var, value, env);

    return RET(2, *value);
}

static Obj* lambda_sf(Obj* const* exps, Obj* const* env) {
    DEF2(params, body);

    if (length(exps) < 2 || !islist((*exps)->car)) {
        error("invalid lambda syntax");
    }

    *params = (*exps)->car;
    *body = (*exps)->cdr;

    return RET(2, make_lambda(params, body, env));
}

static Obj* begin_sf(Obj* const* exps, Obj* const* env) {
    return eval_sequence(exps, env);
}

static Obj* macro_sf(Obj* const* exps, Obj* const* env) {
    DEF2(params, body);

    if (length(exps) < 2 || !islist((*exps)->car)) {
        error("invalid macro syntax");
    }

    *params = (*exps)->car;
    *body = (*exps)->cdr;

    return RET(2, make_macro(params, body, env));
}

#define DEFVAR(var, value)                    \
    {                                         \
        DEF2(_symbol, _obj);                  \
        *_symbol = alloc_symbol((var));       \
        *_obj = (value);                      \
        define_variable(_symbol, _obj, env);  \
        RET(2, NIL);                          \
    }

void setup_env(Obj* const* env) {
    DEFVAR("#t", TRUE);
    DEFVAR("#f", FALSE);

    DEFVAR("car", alloc_primitive(car_p));
    DEFVAR("cdr", alloc_primitive(cdr_p));
    DEFVAR("cons", alloc_primitive(cons_p));
    DEFVAR("eq?", alloc_primitive(eq_p));
    DEFVAR("+", alloc_primitive(plus_p));
    DEFVAR("-", alloc_primitive(minus_p));
    DEFVAR("=", alloc_primitive(equal_p));
    DEFVAR(">", alloc_primitive(gt_p));
    DEFVAR("<", alloc_primitive(lt_p));
    DEFVAR("write", alloc_primitive(write_p));
    DEFVAR("list", alloc_primitive(list_p));

    DEFVAR("quote", alloc_special_form(quote_sf));
    DEFVAR("or", alloc_special_form(or_sf));
    DEFVAR("and", alloc_special_form(and_sf));
    DEFVAR("if", alloc_special_form(if_sf));
    DEFVAR("define", alloc_special_form(define_sf));
    DEFVAR("set!", alloc_special_form(set_sf));
    DEFVAR("lambda", alloc_special_form(lambda_sf));
    DEFVAR("begin", alloc_special_form(begin_sf));
    DEFVAR("macro", alloc_special_form(macro_sf));
}

