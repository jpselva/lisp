#include "test.h"

#define C(a, b) alloc_cons(a,b)
#define NUM(n) alloc_number(n)
#define SYM(s) alloc_symbol(s)

Obj* eval_seq_exps;
Obj* eval_seq_env;
Obj eval_seq_return_obj = { .type = STRING, .string = "dummy" };
Obj* eval_seq_return = &eval_seq_return_obj;

Obj* eval_sequence(Obj* exps, Env env) {
    // mock
    eval_seq_exps = exps;
    eval_seq_env = env;
    return eval_seq_return;
}

Obj* test_primitive(Obj* args) {
    return alloc_number(args->car->number + args->cdr->car->number);
}

int main() {
    testsuite("apply.c");

    /**** 1st part - primitive ****/
    Obj* primitive = alloc_primitive(test_primitive);
    Obj* args = C(NUM(2), C(NUM(3), NIL));

    Obj* result = apply(primitive, args);

    test("applies primitive", result->type == NUMBER);
    test("applies primitive", result->number == 5);

    /**** 2nd part - lambda ****/
    Env base_env = extend_environment(C(SYM("x"), NIL), C(NUM(1), NIL), NIL);
    Obj* params = C(SYM("arg1"), C(SYM("arg2"), NIL));
    Obj* body = C(SYM("+"), C(SYM("arg1"), C(SYM("arg2"), NIL)));
    Lambda proc = make_lambda(params, body, base_env);

    result = apply(proc, args);

    test("returns correct value for lambdas", result == eval_seq_return);
    test("extracts lambda body", eval_seq_exps == body);
    test("extends env", eval_seq_env->cdr == base_env);
    test("extends env", eval_seq_env->car->car == params);
    test("extends env", eval_seq_env->car->cdr == args);

    return 0;
}
