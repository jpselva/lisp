#include "lisp.h"
#define lambda_params(lambda) lambda->car
#define lambda_body(lambda) lambda->cdr->car
#define lambda_env(lambda) lambda->cdr->cdr

typedef Obj* Lambda;

Lambda make_lambda(Obj* params, Obj* body, Env env) {
    Lambda proc = alloc_cons(params, alloc_cons(body, env));
    proc->type = LAMBDA;
    return proc;
}

Obj* apply(Obj* proc, Obj* args) {
    if (proc->type == LAMBDA) {
        Env env = extend_environment(lambda_params(proc), args, lambda_env(proc));
        Obj* result = eval_sequence(lambda_body(proc), env);
        return  result;
    } else if (proc->type == PRIMITIVE) {
        return proc->primitive(args);
    } else
        error("cannot apply object of that type");
}

