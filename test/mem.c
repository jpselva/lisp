#include <string.h>
#include "test.h"
#define MEMSZ 1024

extern Obj* free_ptr;
extern Obj* mem;
void gc();

void test_strs_and_symbols() {
    test("free_ptr == mem initially", free_ptr == mem);

    char* my_str = "hello world";
    Obj* strobj = alloc_string(my_str);

    test("increments free_ptr", free_ptr - mem == 1);
    test("alloc_string stores correct string", !strcmp(strobj->string, my_str));
    test("alloc_string assigns correct type", strobj->type == STRING);
    test("alloc_string copies string", my_str != strobj->string);

    char* my_sym = "foo";
    Obj* symobj = alloc_symbol(my_sym);

    test("allocates when reading symbol for 1st time", free_ptr - mem == 2);
    test("alloc_symbol stores correct string", !strcmp(symobj->string, my_sym));
    test("alloc_symbol assigns correct type", symobj->type == SYMBOL);
    test("alloc_symbol copies string", my_sym != symobj->string);

    Obj* other_symobj = alloc_symbol(my_sym);

    test("does not allocate when reading symbol the 2nd time", free_ptr - mem == 2);
    test("alloc_symbol returns interned symbol", other_symobj == symobj);

    gc();
}

void test_gc_triggers() {
    for (int i = 0; i < MEMSZ + MEMSZ/2; i++) alloc_string("garbage");

    test("gc happens when memory is full", free_ptr == mem + MEMSZ/2);
    gc();
}

void test_simple_gc() {
    alloc_string("this is garbage");

    char* do_not_gc_str = "don't free this memory!";
    Obj* do_not_gc = alloc_string(do_not_gc_str);

    GC_TRACK1(do_not_gc);
    gc();

    test("frees unused memory in gc", free_ptr == mem + 1);
    test("doesn't free stacked addresses", do_not_gc == mem);
    test("doesn't free stacked addresses", mem[0].type == STRING);
    test("doesn't free stacked addresses", !strcmp(mem[0].string, do_not_gc_str))

    GC_RELEASE(1);
    gc();
    test("frees memory if it's not in the stack", free_ptr == mem);

    gc(); // just testing if it doesn't crash when stack and mem are empty
}

void test_gc_pairs1() {
    // garbage collect memory with a pair

    for (int i = 0; i < 100; i++) alloc_string("garbage");

    Obj* pair;
    pair = alloc_cons(NIL, NIL); GC_TRACK1(pair);
    pair->car = alloc_string("this is the car");
    pair->cdr = alloc_string("this is the cdr");
    gc();

    test("frees unused memory in gc", free_ptr == mem + 3);
    test("updates stack variables when they are moved", pair == mem);
    test("copies variables correctly", pair->type == CONS);
    test("copies and updates car correctly", 
            (pair->car->type == STRING) && !strcmp(pair->car->string, "this is the car"));
    test("copies and updates cdr correctly", 
            (pair->cdr->type == STRING) && !strcmp(pair->cdr->string, "this is the cdr"));

    GC_RELEASE(1);
    gc();
}

void test_gc_pairs2() {
    // trigger gc right when an assignment to a pair's car happens

    // leave only a single memory cell free
    for (int i = 0; i < MEMSZ - 1; i++) alloc_string("garbage");

    Obj* pair = alloc_cons(NIL, NIL); GC_TRACK1(pair);
    SET_CAR(pair, alloc_string("this is the car")); // gc happens here
    SET_CDR(pair, alloc_string("this is the cdr"));

    test("frees unused memory in gc", free_ptr == mem + 3); 
    test("updates stack variables when they are moved", pair == mem);
    test("copies variables correctly", pair->type == CONS);
    test("copies and updates car correctly", 
            (pair->car->type == STRING) && !strcmp(pair->car->string, "this is the car"));
    test("copies and updates cdr correctly", 
            (pair->cdr->type == STRING) && !strcmp(pair->cdr->string, "this is the cdr"));

    GC_RELEASE(1);
    gc();
}

void test_gc_pairs3() {
    // trigger gc during alloc_cons

    // leave only 2 cells free
    for (int i = 0; i < MEMSZ - 2; i++) alloc_string("garbage");

    Obj* car = alloc_string("this is the car"); GC_TRACK1(car);
    Obj* cdr = alloc_string("this is the cdr"); GC_TRACK1(cdr);
    Obj* pair = alloc_cons(car, cdr); // gc happens here
    GC_TRACK1(pair);

    test("frees unused memory in gc", free_ptr == mem + 3); 
    test("updates stack variables when they are moved", car == mem);
    test("updates stack variables when they are moved", cdr == mem + 1);
    test("updates stack variables when they are moved", pair == mem + 2);
    test("copies variables correctly", pair->type == CONS);
    test("copies and updates car correctly", 
            (pair->car->type == STRING) && !strcmp(pair->car->string, "this is the car"));
    test("copies and updates cdr correctly", 
            (pair->cdr->type == STRING) && !strcmp(pair->cdr->string, "this is the cdr"));

    GC_RELEASE(3);
    gc();
}

void test_gc_nil_and_booleans() {
    Obj* nil = NIL;
    Obj* fals = FALSE;
    GC_TRACK2(nil, fals);
    gc();
    test("doesn't collect nils or booleans", free_ptr = mem);
    GC_RELEASE(2);
}

void test_gc_cyclic() {
    for (int i = 0; i < 200; i++) alloc_string("garbage");

    Obj* pair = alloc_cons(NIL, NIL); GC_TRACK1(pair);
    SET_CAR(pair, alloc_cons(pair, NIL));
    gc();

    test("frees unused memory in gc", free_ptr = mem + 2);
    test("updates stack variables when they are moved", pair == mem);
    test("copies variables correctly", pair->type == CONS);
    test("copies and updates car correctly", 
            (pair->car->type == CONS) && (pair->car->car == pair));
}

int main() {
    testsuite("mem.c");

    test_strs_and_symbols();
    test_gc_triggers();
    test_simple_gc();
    test_gc_pairs1();
    test_gc_pairs2();
    test_gc_pairs3();
    test_gc_nil_and_booleans();
    test_gc_cyclic();

    return 0;
}
