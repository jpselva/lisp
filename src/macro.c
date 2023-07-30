#include "lisp.h"
#define macro_params(macro) ((macro)->car)
#define macro_body(macro) ((macro)->cdr->car)
#define macro_env(macro) ((macro)->cdr->cdr)

Obj* make_macro(Obj* const* params, Obj* const* body, Obj* const* env) {
    DEF1(tmp);
    *tmp = alloc_cons(body, env);
    *tmp = alloc_cons(params, tmp);
    (*tmp)->type = MACRO;
    return RET(1, *tmp);
}

Obj* expand_macro(Obj* const* macro, Obj* const* exps) {
    DEF4(params, body, env, expanded); 

    *params = macro_params(*macro);
    *body = macro_body(*macro);
    *env = macro_env(*macro);

    *env = extend_environment(params, exps, env);

    *expanded = eval_whole_sequence(body, env); 

    return RET(4, *expanded);
}

