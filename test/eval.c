#include "test.h"

#define C(a, b) alloc_cons(a,b)
#define NUM(n) alloc_number(n)
#define SYM(s) alloc_symbol(s)

Obj* test_primitive(Obj* args) {
    return alloc_number(args->car->number + args->cdr->car->number);
}

Obj* test_special_form(Obj* exps, Env env) {
    if (eval(exps->car, env) != FALSE) {
        return eval(exps->cdr->car, env);
    } else {
        return eval(exps->cdr->cdr->car, env);
    }
}

int main() {
    testsuite("eval.c");

    Obj* primitive = alloc_primitive(test_primitive);
    Obj* special_form = alloc_special_form(test_special_form);

    Obj* params = C(SYM("y"), C(SYM("z"), NIL));
    Obj* subexp = C(SYM("+"), C(SYM("y"), C(SYM("z"), NIL)));
    Obj* body = C(C(SYM("+"), C(SYM("x"), C(subexp, NIL))), NIL);
    Obj* proc = make_lambda(params, body, NIL);

    Obj* vars = C(SYM("+"), C(SYM("x"), C(SYM("foo"), C(SYM("if"), NIL))));
    Obj* values = C(primitive, C(NUM(1), C(proc, C(special_form, NIL))));
    Env env = extend_environment(vars, values, NIL);
    proc->cdr->cdr = env; // set env now that it exists
    
    /**** 0th part ****/
    Obj* exp = NIL;
    Obj* value = eval(exp, env);
    test("nil", value == NIL);

    /**** 1st part ****/
    exp = C(SYM("+"), C(NUM(1), C(SYM("x"), NIL)));

    value = eval(exp, env);

    test("(+ 1 x)", value->type == NUMBER);
    test("(+ 1 x)", value->number == 2);

    /**** 2nd part ****/
    exp = C(SYM("+"), C(exp, C(NUM(1), NIL)));

    value = eval(exp, env);

    test("(+ (+ 1 x) 1)", value->type == NUMBER);
    test("(+ (+ 1 x) 1)", value->number == 3);

    /**** 3rd part ****/
    exp = C(SYM("foo"), C(NUM(2), C(NUM(3), NIL)));

    value = eval(exp, env);

    test("(foo 2 3)", value->type == NUMBER);
    test("(foo 2 3)", value->number == 6);

    /**** 4th part ****/
    exp = C(SYM("if"), C(NUM(2), C(SYM("x"), C(NUM(4), NIL))));
    value = eval(exp, env);

    test("(if 2 x 4)", ((value->type == NUMBER) && (value->number == 1)));
}
