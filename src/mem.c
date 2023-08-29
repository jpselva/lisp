#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "lisp.h"

////////////////////////////////////////
// STACK                              //
////////////////////////////////////////
#define STKSZ 16383
static Obj* stack[STKSZ];
static Obj** stack_ptr = stack;

/* The stack has two purposes:
 *
 * a) At any instant in time, the stack contains the memory addresses of all
 * objects that the interpreter has direct access to. GC will not free these
 * objects nor any objects accessible through them.
 *
 * b) The stack is a layer of abstraction between interpreter and memory. The
 * interpreter accesses memory by keeping pointers to the stack and
 * dereferencing them to get the actual addresses. GC updates stack addresses
 * when it moves stuff around.
 */

Obj** push(Obj* value) {
    if (stack_ptr >=  stack + STKSZ) {
        error("stack overflow");
    }

    *stack_ptr = value;
    return stack_ptr++;
}

static void pop(size_t nvars) {
    while((nvars > 0) && (stack_ptr > stack)) {
        stack_ptr--;
        nvars--;
    }
    if (nvars != 0) { 
        error("stack underflow"); 
    }
}

Obj* popandreturn(size_t nvars, Obj* value) {
    pop(nvars);
    return value;
}

#define DEF1(var) Obj** var = push(NIL)
#define DEF2(var1, var2) Obj** var1 = push(NIL); Obj** var2 = push(NIL)
#define DEF3(var1, var2, var3) \
    Obj** var1 = push(NIL); Obj** var2 = push(NIL); Obj** var3 = push(NIL)
#define RET(n, exp) popandreturn(n, exp)

/* Since GC can happen in any call to the allocation functions, the following
 * code might lead to undefined behavior:
 *
 *     (*obj)->car = alloc_string("this might fail");
 *
 * What if `(*obj)->car` is evaluated before the right hand side, but `alloc_string`
 * triggers GC? `(*obj)` now points somewhere else but the target of the assignment
 * will still point to the old memory. Hence these two macros:
 */
#define SET_CAR(obj, exp) { Obj* _tmp = (exp); (obj)->car = _tmp; }
#define SET_CDR(obj, exp) { Obj* _tmp = (exp); (obj)->cdr = _tmp; }

////////////////////////////////////////
// MEMORY (heap)                      //
////////////////////////////////////////
#define MEMSZ 65536
#define IS_COMPOUND_OBJ(obj)                              \
    (((obj)->type == CONS) || ((obj)->type == LAMBDA) ||  \
    ((obj)->type == MACRO) || ((obj)->type == TAIL_CALL))
#define IS_STRING_OBJ(obj) \
    (((obj)->type == STRING) || ((obj)->type == SYMBOL))

static Obj mem1[MEMSZ], mem2[MEMSZ];
static Obj* mem = mem1; 
static Obj* free_ptr = mem1;

static Obj* gc_move(Obj* old_addr) {
    // update an address in old memory to one in new memory (only called in GC)
    switch (old_addr->type) {
        case BOOLEAN:
        case EMPTY_LIST:
            // booleans and '() have constant addresses outside the heap
            return old_addr;
        case MOVED_BY_GC:
            return old_addr->forwarding_addr;
        default:
            if (IS_STRING_OBJ(free_ptr)) {
                free(free_ptr->string);
            }

            *free_ptr = *old_addr;
            old_addr->type = MOVED_BY_GC;
            old_addr->forwarding_addr = free_ptr;
            return free_ptr++;
    }
}

static void gc() {
    // Stop-and-copy garbage collection
    Obj* new_mem = (mem == mem1) ? mem2 : mem1;
    free_ptr = new_mem;
    
    for (int i = 0; i < (stack_ptr - stack); i++) {
        stack[i] = gc_move(stack[i]);
    }

    for(Obj* scan_ptr = new_mem; scan_ptr < free_ptr; scan_ptr++) {
        if (IS_COMPOUND_OBJ(scan_ptr)) {
            scan_ptr->car = gc_move(scan_ptr->car);
            scan_ptr->cdr = gc_move(scan_ptr->cdr);
        }
    }

    if (free_ptr == new_mem + MEMSZ) {
        error("memory is full");
    }
    mem = new_mem;
}

static void freemem(Obj* mem) {
    // Free dangling strings from memory
    for(Obj* sweep_ptr = mem; sweep_ptr < mem + MEMSZ; sweep_ptr++) {
        if (IS_STRING_OBJ(sweep_ptr)) {
            free(sweep_ptr->string);
            sweep_ptr->string = NULL;

            // make sure no one will try to free the NULL string later
            sweep_ptr->type = NUMBER;
        }
    }
}

void cleanup() {
    freemem(mem1); freemem(mem2);
    if (stack_ptr != stack) {
        error("finished execution with unpopped variables in the stack");
    }
}

////////////////////////////////////////
// ALLOC_*                            //
////////////////////////////////////////

static Obj* alloc() {
#ifndef ALWAYS_GC
    if (free_ptr == mem + MEMSZ) {
        gc();
    }
#else
    gc(); // useful for finding gc bugs
#endif

    return free_ptr++;
}

Obj* alloc_cons(Obj* const* car, Obj* const* cdr) {
    Obj* new_obj = alloc(); 

    new_obj->type = CONS;
    new_obj->car = *car;
    new_obj->cdr = *cdr;

    return new_obj;
}

Obj* alloc_number(int num) {
   Obj* new_obj = alloc(); 

   new_obj->type = NUMBER;
   new_obj->number = num;

   return new_obj;
}

Obj* alloc_primitive(Primitive prim) {
   Obj* new_obj = alloc(); 

   new_obj->type = PRIMITIVE;
   new_obj->primitive = prim;

   return new_obj;
}

Obj* alloc_string(const char* str) {
   Obj* new_obj = alloc(); 
   char* copied_str = malloc(sizeof(char) * strlen(str));
   strcpy(copied_str, str);

   new_obj->type = STRING;
   new_obj->string = copied_str;

   return new_obj;
}

Obj* alloc_symbol(const char* sym) {
    Obj* scan_ptr;
    for (scan_ptr = mem; scan_ptr < free_ptr; scan_ptr++) {
        if (scan_ptr->type == SYMBOL && !strcmp(scan_ptr->string, sym)) {
            return scan_ptr;
        }
    }

   Obj* new_obj = alloc(); 
   char* copied_sym = malloc(sizeof(char) * strlen(sym));
   strcpy(copied_sym, sym);

   new_obj->type = SYMBOL;
   new_obj->string = copied_sym;

   return new_obj;
}

Obj* alloc_special_form(SpecialForm sf) {
    Obj* new_obj = alloc(); 
    
    new_obj->type = SPECIAL_FORM;
    new_obj->special_form = sf;

    return new_obj;
}
