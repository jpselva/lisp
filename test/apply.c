#include "test.h"

#define C(a, b) alloc_cons(a,b)
#define NUM(n) alloc_number(n)
#define SYM(s) alloc_symbol(s)

Obj** eval_seq_exps;
Obj** eval_seq_env;
Obj eval_seq_return_obj = { .type = STRING, .string = "dummy" };
Obj* eval_seq_return = &eval_seq_return_obj;

Obj* eval_sequence(Obj** exps, Obj** env) {
    // mock
    eval_seq_exps = exps;
    eval_seq_env = env;
    return eval_seq_return;
}

Obj* test_primitive(Obj** args) {
    return alloc_number((*args)->car->number + (*args)->cdr->car->number);
}

void test_apply_primitive() {
    DEF3(primitive, args, value);

    *primitive = alloc_primitive(test_primitive);
    *value = NUM(3);
    *args = alloc_cons(value, &NIL);
    *value = NUM(2);
    *args = alloc_cons(value, args);

    Obj* result = apply(primitive, args);

    test("applies primitive", result->type == NUMBER);
    test("applies primitive", result->number == 5);

    RET(3, NIL);
}

void test_apply_lambda() {
    DEF4(env, params, body, var); DEF3(value, proc, args);

    // construct base environment
    *env= extend_environment(&NIL, &NIL, &NIL);
    *var = SYM("x"); *value = NUM(1);
    define_variable(var, value, env);

    // construct parameter list
    *var = SYM("arg2");
    *params = alloc_cons(var, &NIL);
    *var = SYM("arg1");
    *params = alloc_cons(var, params);

    // construct body
    *var = SYM("arg2");
    *body = alloc_cons(var, &NIL);
    *var = SYM("arg1");
    *body = alloc_cons(var, body);
    *var = SYM("+");
    *body = alloc_cons(var, body);

    // allocate lambda
    *proc = make_lambda(params, body, env);

    // allocate arguments
    *var = NUM(1);
    *args = alloc_cons(var, &NIL);
    *var = NUM(2);
    *args  = alloc_cons(var, args);

    Obj* result = apply(proc, args);

    test("returns correct value for lambdas", result == eval_seq_return);
    test("extracts lambda body", *eval_seq_exps == *body);
    test("extends env", (*eval_seq_env)->cdr == *env);
    test("extends env", (*eval_seq_env)->car->car == *params);
    test("extends env", (*eval_seq_env)->car->cdr == *args);
}

int main() {
    testsuite("apply.c");

    test_apply_primitive();
    test_apply_lambda();

    return 0;
}
