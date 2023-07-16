#include "lisp.h"

int length(Obj** args) {
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

Obj* car_p(Obj** args) {
    if (length(args) != 1) {
        error("car takes exactly one argument");
    }
    if ((*args)->car->type != CONS) {
        error("can't take car of non-cons object");
    }
    return (*args)->car->car;
}

Obj* cdr_p(Obj** args) {
    if (length(args) != 1) {
        error("cdr takes only one argument");
    }
    if ((*args)->car->type != CONS) {
        error("can't take cdr of non-cons object");
    }
    return (*args)->car->cdr;
}

Obj* cons_p(Obj** args) {
    DEF2(car, cdr);

    if (length(args) != 2) {
        error("cons takes exactly two arguments");
    }
    *car = (*args)->car;
    *cdr = (*args)->cdr->car;

    return RET(2, alloc_cons(car, cdr));
}

Obj* eq_p(Obj** args) {
    if (length(args) != 2) {
        error("eq? takes exactly two arguments");
    }
    return ((*args)->car == (*args)->cdr->car) ? TRUE : FALSE;
}

Obj* plus_p(Obj** args) {
    int result = 0;

    for (Obj* num = *args; num != NIL; num = num->cdr) {
        if (num->car->type != NUMBER) {
            error("+ takes only numbers");
        }
        result += num->car->number;
    }

    return alloc_number(result);
}

Obj* minus_p(Obj** args) {
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

Obj* equal_p(Obj** args) {
   if (length(args) != 2) {
       error("= takes exactly two arguments");
   }
   if (((*args)->car->type != NUMBER) || ((*args)->cdr->car->type != NUMBER)) {
       error("both arguments to = must be numbers");
   }

   return ((*args)->car->number == (*args)->cdr->car->number) ? TRUE : FALSE;
}

Obj* gt_p(Obj** args) {
   if (length(args) != 2) {
       error("> takes exactly two arguments");
   }
   if (((*args)->car->type != NUMBER) || ((*args)->cdr->car->type != NUMBER)) {
       error("both arguments to = must be numbers");
   }

   return ((*args)->car->number > (*args)->cdr->car->number) ? TRUE : FALSE;
}

Obj* lt_p(Obj** args) {
   if (length(args) != 2) {
       error("< takes exactly two arguments");
   }
   if (((*args)->car->type != NUMBER) || ((*args)->cdr->car->type != NUMBER)) {
       error("both arguments to = must be numbers");
   }

   return ((*args)->car->number < (*args)->cdr->car->number) ? TRUE : FALSE;
}

Obj* write_p(Obj** args) {
    DEF1(obj);

    if (length(args) != 1) {
        error("write takes 1 argument");
    }

    *obj = (*args)->car;
    write(obj);
    return RET(1, NIL);
}

/**** Special forms ****/

Obj* quote_sf(Obj** exps, Obj** env) {
    if (length(exps) != 1) {
        error("quote expects exactly one expression");
    }
    return (*exps)->car;
}

Obj* or_sf(Obj** exps, Obj** env) {
    DEF2(exp_scan, exp);

    Obj* result = FALSE;

    for(*exp_scan = *exps; *exp_scan != NIL; *exp_scan = (*exp_scan)->cdr) {
        *exp = (*exp_scan)->car;
        if ((result = eval(exp, env)) != FALSE) {
            break;
        }
    }

    return RET(2, result);
}

Obj* if_sf(Obj** exps, Obj** env) {
    DEF3(predicate, consequent, alternative);

    int exps_length = length(exps);

    if ((exps_length != 2) && (exps_length != 3)) {
        error("if expects 2 or 3 expressions");
    }

    *predicate = (*exps)->car;
    *consequent = (*exps)->cdr->car;
    *alternative = (*exps)->cdr->cdr->car;

    if (eval(predicate, env) != FALSE) {
        return RET(3, eval(consequent, env));
    } else if (exps_length == 3) {
        return RET(3, eval(alternative, env));
    } else {
        return RET(3, TRUE);
    }
}

Obj* define_sf(Obj** exps, Obj** env) {
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

Obj* set_sf(Obj** exps, Obj** env) {
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

Obj* lambda_sf(Obj** exps, Obj** env) {
    DEF2(params, body);

    if (length(exps) < 2) {
        error("invalid lambda syntax");
    }

    *params = (*exps)->car;
    *body = (*exps)->cdr;

    return RET(2, make_lambda(params, body, env));
}

Obj* begin_sf(Obj** exps, Obj** env) {
    return eval_sequence(exps, env);
}

#define DEFVAR(var, value)                    \
    {                                         \
        DEF2(_symbol, _obj);                  \
        *_symbol = alloc_symbol((var));       \
        *_obj = (value);                      \
        define_variable(_symbol, _obj, env);  \
        RET(2, NIL);                          \
    }

void setup_env(Obj** env) {
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

    DEFVAR("quote", alloc_special_form(quote_sf));
    DEFVAR("or", alloc_special_form(or_sf));
    DEFVAR("if", alloc_special_form(if_sf));
    DEFVAR("define", alloc_special_form(define_sf));
    DEFVAR("set!", alloc_special_form(set_sf));
    DEFVAR("lambda", alloc_special_form(lambda_sf));
    DEFVAR("begin", alloc_special_form(begin_sf));
}

