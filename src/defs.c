#include "lisp.h"

/**** Primitive functions ****/

int length(Obj* args) {
    int len;
    for (len = 0; args != NIL; args = args->cdr) {
        if (args->type != CONS) {
            return -1; // not a list
        }
        len++;
    }
    return len;
}

Obj* car(Obj* args) {
    if (length(args) != 1) {
        error("car takes exactly one argument");
    }
    if (args->car->type != CONS) {
        error("can't take car of non-cons object");
    }
    return args->car->car;
}

Obj* cdr(Obj* args) {
    if (length(args) != 1) {
        error("cdr takes only one argument");
    }
    if (args->car->type != CONS) {
        error("can't take cdr of non-cons object");
    }
    return args->car->cdr;
}

Obj* cons(Obj* args) {
    if (length(args) != 2) {
        error("cons takes exactly two arguments");
    }
    return alloc_cons(args->car, args->cdr->car);
}

Obj* is_eq(Obj* args) {
    if (length(args) != 2) {
        error("eq? takes exactly two arguments");
    }
    return (args->car == args->cdr->car) ? TRUE : FALSE;
}

Obj* plus(Obj* args) {
    int result = 0;

    while (args != NIL) {
        if (args->car->type != NUMBER) {
            error("can't sum something that's not a number");
        }
        result += args->car->number;
        args = args->cdr;
    }

    return alloc_number(result);
}

Obj* is_equal(Obj* args) {
   if (length(args) != 2) {
       error("= takes exactly two arguments");
   }
   if ((args->car->type != NUMBER) || (args->cdr->car->type != NUMBER)) {
       error("both arguments to = must be numbers");
   }

   return (args->car->number == args->cdr->car->number) ? TRUE : FALSE;
}

/**** Special forms ****/

Obj* eval_if(Obj* exps, Env env) {
    int exps_length = length(exps);
    if ((exps_length != 2) && (exps_length != 3)) {
        error("if expects 2 or 3 expressions");
    }
    if (eval(exps->car, env) != FALSE) {
        return eval(exps->cdr->car, env);
    } else if (exps_length == 3) {
        return eval(exps->cdr->cdr->car, env);
    } else {
        return TRUE;
    }
}

Obj* eval_define(Obj* exps, Env env) {
    if (length(exps) != 2 || exps->car->type != SYMBOL) {
        error("invalid define syntax");
    }
    Obj* variable_value = eval(exps->cdr->car, env);
    define_variable(exps->car, variable_value, env);
    return variable_value;
}

Obj* eval_set(Obj* exps, Env env) {
    if (length(exps) != 2 || exps->car->type != SYMBOL) {
        error("invalid set! syntax");
    }
    Obj* variable_value = eval(exps->cdr->car, env);
    set_variable(exps->car, variable_value, env);
    return variable_value;
}

Obj* eval_lambda(Obj* exps, Env env) {
    if (length(exps) < 2 || exps->cdr->car->type != CONS) {
        error("invalid lambda syntax");
    }
    return make_lambda(exps->car, exps->cdr, env);
}

Obj* eval_begin(Obj* exps, Env env) {
    return eval_sequence(exps, env);
}

void setup_env(Env env) {
    define_variable(alloc_symbol("#t"), TRUE, env);
    define_variable(alloc_symbol("#f"), FALSE, env);
    define_variable(alloc_symbol("car"), alloc_primitive(car), env);
    define_variable(alloc_symbol("cdr"), alloc_primitive(cdr), env);
    define_variable(alloc_symbol("cons"), alloc_primitive(cons), env);
    define_variable(alloc_symbol("eq?"), alloc_primitive(is_eq), env);
    define_variable(alloc_symbol("+"), alloc_primitive(plus), env);
    define_variable(alloc_symbol("="), alloc_primitive(is_equal), env);
    define_variable(alloc_symbol("if"), alloc_special_form(eval_if), env);
    define_variable(alloc_symbol("define"), alloc_special_form(eval_define), env);
    define_variable(alloc_symbol("set!"), alloc_special_form(eval_set), env);
    define_variable(alloc_symbol("lambda"), alloc_special_form(eval_lambda), env);
    define_variable(alloc_symbol("begin"), alloc_special_form(eval_begin), env);
}

