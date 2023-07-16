#include "test.h"

#define C(a, b) alloc_cons(a, b)
#define SYM(sym) alloc_symbol(sym)
#define NUM(num) alloc_number(num)

int main() {
    testsuite("env.c");
    DEF4(var, value, env, old_env);

    /**** 1st part - one frame ****/
    *env = extend_environment(&NIL, &NIL, &NIL);
    char* symbols[] = {"z", "y", "x"};
    int numbers[] = {3, 2, 1};
    for (int i = 0; i < 3; i++) {
        *var = alloc_symbol(symbols[i]);
        *value = alloc_number(numbers[i]);
        define_variable(var, value, env);
    }

    *var = SYM("y"); 
    Obj* lookup_val = lookup(var, env);
    test("variable lookup single frame", lookup_val->type == NUMBER);
    test("variable lookup single frame", lookup_val->number == 2);

    *var = SYM("z"); *value = NUM(10);
    set_variable(var, value, env);
    test("variable set single frame", lookup(var, env)->number == 10);

    *var = SYM("a"); *value = NUM(42);
    define_variable(var, value, env);
    Obj* frame = (*env)->car;
    test("variable def single frame", frame->car->car == SYM("a"));
    test("variable def single frame", frame->cdr->car->type == NUMBER);
    test("variable def single frame", frame->cdr->car->number == 42);

    /**** 2nd part - two frames ****/
    *env = extend_environment(&NIL, &NIL, env);
    *var = SYM("y"); *value = NUM(300);
    define_variable(var, value, env);

    *var = SYM("x");
    lookup_val = lookup(var, env);
    test("lookup transverses frames if needed", lookup_val->type == NUMBER);
    test("lookup transverses frames if needed", lookup_val->number == 1);

    *var = SYM("x"); *value = NUM(34);
    set_variable(var, value, env);
    lookup_val = lookup(var, env);
    test("set transverses frames if needed", lookup_val->number == 34);

    *var = SYM("y");
    lookup_val = lookup(var, env);
    test("vars in earlier frames shadow those in later frames", lookup_val->type == NUMBER);
    test("vars in earlier frames shadow those in later frames", lookup_val->number == 300);

    *old_env = (*env)->cdr;
    *var = SYM("y"); *value = NUM(200);
    set_variable(var, value, env);
    Obj* shadowed_y = lookup(var, old_env);
    Obj* actual_y = lookup(var, env);
    test("set should't alter shadowed variables", shadowed_y->number == 2);
    test("set altered variable's earliest occurrence in a frame", actual_y->number == 200);

    *var = SYM("x"); *value = NUM(77);
    define_variable(var, value, env); 
    frame = (*env)->car;
    test("definition always inserts in 1st frame", frame->car->car == SYM("x"));
    test("definition always inserts in 1st frame", frame->cdr->car->type == NUMBER);
    test("definition always inserts in 1st frame", frame->cdr->car->number == 77);
}
