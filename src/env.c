#include <string.h>
#include "lisp.h"

#define frame_vars(frame) frame->car
#define frame_values(frame) frame->cdr
#define set_frame(frame, vars, values) \
    (frame->car = vars, frame->cdr = values)

#define first_frame(env) env->car
#define rest_frames(env) env->cdr

Obj* make_frame(Obj* vars, Obj* values) {
    return alloc_cons(vars, values);
}

Obj* extend_environment(Obj* vars, Obj* values, Env env)  {
    Obj* frame = make_frame(vars, values);
    Env new_env = alloc_cons(frame, env);
    return new_env;
}

void define_variable(Obj* var, Obj* value, Env env) {
    Obj* frame = first_frame(env);
    Obj* var_scan = frame_vars(frame);

    while (var_scan != NIL) {
        if (var_scan->car == var)
            error("redefinition of variable");
        var_scan = var_scan->cdr;
    }

    Obj* new_vars = alloc_cons(var, frame_vars(frame)); 
    Obj* new_values = alloc_cons(value, frame_values(frame));
    set_frame(frame, new_vars, new_values);
}

void set_variable(Obj* var, Obj* value, Env env) {
    Obj* frame, *var_scan, *value_scan; 

    for (; env != NIL; env = rest_frames(env)) {
        frame = first_frame(env);
        var_scan = frame_vars(frame);
        value_scan = frame_values(frame);

        while (var_scan != NIL) {
            if (var_scan->car == var) {
                value_scan->car = value;
                return;
            }
            var_scan = var_scan->cdr;
            value_scan = value_scan->cdr;
        }
    }
    error("attempt to set undefined variable");
}

Obj* lookup(Obj* var, Env env) {
    Obj* frame, *var_scan, *value_scan; 

    for (; env != NIL; env = rest_frames(env)) {
        frame = first_frame(env);
        var_scan = frame_vars(frame);
        value_scan = frame_values(frame);

        while (var_scan != NIL) {
            if (var_scan->car == var)
                return value_scan->car;
            var_scan = var_scan->cdr;
            value_scan = value_scan->cdr;
        }
    }
    error("variable not found");
}

