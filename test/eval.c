#include "test.h"

#define C(a, b) alloc_cons(a,b)
#define NUM(n) alloc_number(n)
#define SYM(s) alloc_symbol(s)

Obj* test_primitive(Obj* args) {
    return alloc_number(args->car->number + args->cdr->car->number);
}

int main() {
    testsuite("eval.c");

    Obj* primitive = alloc_primitive(test_primitive);

    Obj* params = C(SYM("y"), C(SYM("z"), NULL));
    Obj* subexp = C(SYM("+"), C(SYM("y"), C(SYM("z"), NULL)));
    Obj* body = C(C(SYM("+"), C(SYM("x"), C(subexp, NULL))), NULL);
    Obj* proc = make_lambda(params, body, NULL);

    Obj* vars = C(SYM("+"), C(SYM("x"), C(SYM("foo"), NULL)));
    Obj* values = C(primitive, C(NUM(1), C(proc, NULL)));
    Env env = extend_environment(vars, values, NULL);
    proc->cdr->cdr = env; // set env now that it exists
    
    /**** 1st part ****/
    Obj* exp = C(SYM("+"), C(NUM(1), C(SYM("x"), NULL)));

    Obj* value = eval(exp, env);

    test("(+ 1 x)", value->type == NUMBER);
    test("(+ 1 x)", value->number == 2);

    /**** 2nd part ****/
    exp = C(SYM("+"), C(exp, C(NUM(1), NULL)));

    value = eval(exp, env);

    test("(+ (+ 1 x) 1)", value->type == NUMBER);
    test("(+ (+ 1 x) 1)", value->number == 3);

    /**** 3rd part ****/
    exp = C(SYM("foo"), C(NUM(2), C(NUM(3), NULL)));

    value = eval(exp, env);

    test("(foo 2 3)", value->type == NUMBER);
    test("(foo 2 3)", value->number == 6);
}
