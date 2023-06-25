#include <string.h>
#include "lisp.h"

#define frame_vars(frame) frame->car
#define frame_values(frame) frame->cdr
#define set_frame(frame, vars, values) \
    (frame->car = vars, frame->cdr = values)

#define first_frame(env) env->car
#define rest_frames(env) env->cdr

obj* make_frame(obj* vars, obj* values) {
    return alloc_cons(vars, values);
}

obj* extend_environment(obj* vars, obj* values, obj* env)  {
    obj* frame = make_frame(vars, values);
    obj* new_env = alloc_cons(frame, env);
    return new_env;
}

void define_variable(obj* var, obj* value, obj* env) {
    obj* frame = first_frame(env);
    obj* var_scan = frame_vars(frame);

    while (var_scan != NULL) {
        if (var_scan->car == var)
            error("redefinition of variable");
        var_scan = var_scan->cdr;
    }

    obj* new_vars = alloc_cons(var, frame_vars(frame)); 
    obj* new_values = alloc_cons(value, frame_values(frame));
    set_frame(frame, new_vars, new_values);
}

void set_variable(obj* var, obj* value, obj* env) {
    obj* frame = first_frame(env);
    obj* var_scan = frame_vars(frame);
    obj* value_scan = frame_values(frame);

    for (; env != NULL; env = rest_frames(env)) {
        frame = first_frame(env);
        var_scan = frame_vars(frame);
        value_scan = frame_values(frame);

        while (var_scan != NULL) {
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


obj* lookup(obj* var, obj* env) {
    obj* frame, *var_scan, *value_scan; 

    for (; env != NULL; env = rest_frames(env)) {
        frame = first_frame(env);
        var_scan = frame_vars(frame);
        value_scan = frame_values(frame);

        while (var_scan != NULL) {
            if (var_scan->car == var)
                return value_scan->car;
            var_scan = var_scan->cdr;
            value_scan = value_scan->cdr;
        }
    }
    error("variable not found");
}

