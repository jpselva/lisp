#include <string.h>
#include "lisp.h"

#define FRAME_VARS(frame) ((frame)->car)
#define FRAME_VALUES(frame) ((frame)->cdr)
#define FIRST_FRAME(env) ((env)->car)
#define REST_FRAMES(env) ((env)->cdr)

static Obj* make_frame(Obj* const* vars, Obj* const* values) {
    return alloc_cons(vars, values);
}

static void set_frame(Obj* const* frame, Obj* const* vars, Obj* const* values) {
    SET_CAR(*frame, *vars);
    SET_CDR(*frame, *values);
}

Obj* extend_environment(Obj* const* vars, Obj* const* values, Obj* const* env)  {
    DEF2(frame, new_env);

    *frame = make_frame(vars, values);
    *new_env = alloc_cons(frame, env);

    return RET(2, *new_env);
}

void define_variable(Obj* const* var, Obj* const* value, Obj* const* env) {
    DEF3(frame, vars, values);

    *frame = FIRST_FRAME(*env);
    *vars = FRAME_VARS(*frame); 
    *values = FRAME_VALUES(*frame);

    for (Obj* var_scan = *vars; var_scan != NIL; var_scan = var_scan->cdr) {
        if (var_scan->car == *var)
            error("redefinition of variable");
    }

    *vars = alloc_cons(var, vars);
    *values = alloc_cons(value, values);
    set_frame(frame, vars, values);

    RET(3, NIL);
}

void set_variable(Obj* const* var, Obj* const* value, Obj* const* env) {
    Obj *env_scan, *var_scan, *value_scan; 

    for (env_scan = *env; env_scan != NIL; env_scan = REST_FRAMES(env_scan)) {
        var_scan = FRAME_VARS(FIRST_FRAME(env_scan));
        value_scan = FRAME_VALUES(FIRST_FRAME(env_scan));

        while ((var_scan != NIL) && (value_scan != NIL)) {
            if (var_scan->car == *var) {
                value_scan->car = *value;
                return;
            }
            var_scan = var_scan->cdr;
            value_scan = value_scan->cdr;
        }
    }
    error("attempt to set undefined variable");
}

Obj* lookup(Obj* const* var, Obj* const* env) {
    Obj *env_scan, *var_scan, *value_scan; 

    for (env_scan = *env; env_scan != NIL; env_scan = REST_FRAMES(env_scan)) {
        var_scan = FRAME_VARS(FIRST_FRAME(env_scan));
        value_scan = FRAME_VALUES(FIRST_FRAME(env_scan));

        while ((var_scan != NIL) && (value_scan != NIL)) {
            if (var_scan->car == *var) {
                return value_scan->car;
            }
            var_scan = var_scan->cdr;
            value_scan = value_scan->cdr;
        }
    }
    error("undefined variable");
}

