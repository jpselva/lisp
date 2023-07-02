#include "test.h"
#include <string.h>

#define C(a, b) alloc_cons(a,b)
#define NUM(n) alloc_number(n)
#define SYM(s) alloc_symbol(s)

int main() {
    testsuite("defs.c")

    Env env = extend_environment(NIL, NIL, NIL);
    setup_env(env);
    Obj* pair = alloc_cons(NUM(42), NUM(99));
    define_variable(SYM("p"), pair, env);

    Obj* exp = C(SYM("car"), C(SYM("p"), NIL)); 
    Obj* value = eval(exp, env);
    test("car", (value->type == NUMBER) && (value->number == pair->car->number));

    exp = C(SYM("cdr"), C(SYM("p"), NIL)); 
    value = eval(exp, env);
    test("cdr", (value->type == NUMBER) && (value->number == pair->cdr->number));

    exp = C(SYM("cons"), C(NUM(2), C(NUM(3), NIL))); 
    value = eval(exp, env);
    test("cons", value->type == CONS);
    test("cons", (value->car->type == NUMBER) && (value->car->number == 2));
    test("cons", (value->cdr->type == NUMBER) && (value->cdr->number == 3));

    define_variable(SYM("q"), pair, env);
    exp = C(SYM("eq?"), C(SYM("q"), C(SYM("p"), NIL)));
    value = eval(exp, env);
    test("eq? (should return true)", value == TRUE);

    exp = C(SYM("eq?"), C(SYM("q"), C(NUM(3), NIL)));
    value = eval(exp, env);
    test("eq? (should return false)", value == FALSE);

    define_variable(SYM("x"), NUM(1), env);
    exp = C(SYM("+"), C(NUM(2), C(SYM("x"), NIL)));
    value = eval(exp, env);
    test("+", ((value->type == NUMBER) && (value->number == 3)));

    exp = C(SYM("="), C(NUM(3), C(NUM(3), NIL)));
    value = eval(exp, env);
    test("= (should return true)", value == TRUE);

    exp = C(SYM("="), C(NUM(4), C(NUM(3), NIL)));
    value = eval(exp, env);
    test("= (should return false)", value == FALSE);

    /**** 2nd part - special forms ****/
    exp = C(SYM("if"), C(NUM(2), C(SYM("x"), C(NUM(4), NIL))));
    value = eval(exp, env);
    test("(if 2 x 4)", ((value->type == NUMBER) && (value->number == 1)));

    exp = C(SYM("if"), C(NUM(2), C(SYM("x"), NIL)));
    value = eval(exp, env);
    test("(if 2 x)", ((value->type == NUMBER) && (value->number == 1)));

    exp = C(SYM("if"), C(SYM("#f"), C(SYM("x"), C(NUM(4), NIL))));
    value  = eval(exp, env);
    test("(if #f x 4)", ((value->type == NUMBER && (value->number == 4))));

    exp = C(SYM("if"), C(SYM("#f"), C(SYM("x"), NIL)));
    value  = eval(exp, env);
    test("(if #f x)", value == TRUE);

    exp = C(SYM("define"), C(SYM("z"), C(NUM(42), NIL)));
    eval(exp, env);
    value = lookup(SYM("z"), env);
    test("(define z 42)", ((value->type == NUMBER) && (value->number == 42)));

    exp = C(SYM("set!"), C(SYM("z"), C(NUM(99), NIL)));
    eval(exp, env);
    value = lookup(SYM("z"), env);
    test("(set! z 99)", ((value->type == NUMBER) && (value->number == 99)));

    Obj* params = C(SYM("a"), C(SYM("b"), NIL));
    Obj* body = C(C(SYM("+"), C(SYM("a"), C(SYM("b"), NIL))), NIL);
    exp = C(SYM("lambda"), C(params, body));
    value = eval(exp, env);
    test("(lambda (a b) (+ a b))", value->type == LAMBDA);
    test("(lambda (a b) (+ a b))", lambda_params(value) == params);
    test("(lambda (a b) (+ a b))", lambda_body(value) == body);
    test("(lambda (a b) (+ a b))", lambda_env(value) == env);

    Obj* exp1 = C(SYM("set!"), C(SYM("z"), C(NUM(19), NIL)));
    Obj* exp2 = C(SYM("+"), C(NUM(2), C(NUM(3), NIL)));
    exp = C(SYM("begin"), C(exp1, C(exp2, NIL)));
    value = eval(exp, env);
    test("(begin (set! z 20) (+ 2 3))", ((value->type == NUMBER) && (value->number == 5)));
    value = lookup(SYM("z"), env);
    test("(begin (set! z 20) (+ 2 3))", ((value->type == NUMBER) && (value->number == 19)));
}

