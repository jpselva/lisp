#include "lisp.h"
#define lambda_params(lambda) (lambda)->car
#define lambda_body(lambda) (lambda)->cdr->car
#define lambda_env(lambda) (lambda)->cdr->cdr

Obj* make_lambda(Obj* const* params, Obj* const* body, Obj* const* env) {
    DEF1(tmp);
    *tmp = alloc_cons(body, env);
    *tmp = alloc_cons(params, tmp);
    (*tmp)->type = LAMBDA;
    return RET(1, *tmp);
}

Obj* apply(Obj* const* proc, Obj* const* args) {
    if ((*proc)->type == LAMBDA) {
        DEF3(params, env, body);

        *params = lambda_params(*proc);
        *env = lambda_env(*proc);
        *body = lambda_body(*proc);

        *env = extend_environment(params, args, env);
        Obj* result = eval_sequence(body, env);

        return RET(3, result);
    } else if ((*proc)->type == PRIMITIVE) {
        return (*proc)->primitive(args);
    } else {
        error("cannot apply object of that type");
    }
}

