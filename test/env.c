#include "test.h"

#define C(a, b) alloc_cons(a, b)
#define SYM(sym) alloc_symbol(sym)
#define NUM(num) alloc_number(num)

int main() {
    testsuite("env.c");

    /**** 1st part - one frame ****/
    obj* vars1 = C(SYM("x"), C(SYM("y"), C(SYM("z"), NULL)));
    obj* values1 = C(NUM(1), C(NUM(2), C(NUM(3), NULL)));

    obj* env = extend_environment(vars1, values1, NULL);

    obj* lookup_val = lookup(SYM("y"), env);
    test("variable lookup single frame", lookup_val->type == NUMBER);
    test("variable lookup single frame", lookup_val->number == 2);

    set_variable(SYM("z"), NUM(10), env);
    test("variable set single frame", lookup(SYM("z"), env)->number == 10);

    define_variable(SYM("a"), NUM(42), env);
    obj* frame = env->car;
    test("variable def single frame", frame->car->car == SYM("a"));
    test("variable def single frame", frame->cdr->car->type == NUMBER);
    test("variable def single frame", frame->cdr->car->number == 42);

    /**** 2nd part - two frames ****/
    obj* vars2 = C(SYM("y"), NULL);
    obj* vals2 = C(NUM(300), NULL);
    env = extend_environment(vars2, vals2, env);

    lookup_val = lookup(SYM("x"), env);
    test("lookup transverses frames if needed", lookup_val->type == NUMBER);
    test("lookup transverses frames if needed", lookup_val->number == 1);

    set_variable(SYM("x"), NUM(34), env);
    lookup_val = lookup(SYM("x"), env->cdr);
    test("set transverses frames if needed", lookup_val->number == 34);

    lookup_val = lookup(SYM("y"), env);
    test("vars in earlier frames shadow those in later frames", lookup_val->type == NUMBER);
    test("vars in earlier frames shadow those in later frames", lookup_val->number == 300);

    set_variable(SYM("y"), NUM(200), env);
    obj* shadowed_y = lookup(SYM("y"), env->cdr); 
    obj* actual_y = lookup(SYM("y"), env);
    test("set should't alter shadowed variables", shadowed_y->number == 2);
    test("set altered variable's earliest occurrence in a frame", actual_y->number == 200);

    define_variable(SYM("x"), NUM(77), env); 
    frame = env->car;
    test("definition always inserts in 1st frame", frame->car->car == SYM("x"));
    test("definition always inserts in 1st frame", frame->cdr->car->type == NUMBER);
    test("definition always inserts in 1st frame", frame->cdr->car->number == 77);
}
