#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "lisp.h"
#define MEMSZ 1024
#define STKSZ 256 // < MEMSZ

Obj* stack[STKSZ];
Obj** stack_ptr = stack;

Obj** push(Obj* value) {
    *stack_ptr = value;
    return stack_ptr++;
}

void pop(size_t nvars) {
    while((nvars > 0) && (stack_ptr > stack)) {
        stack_ptr--;
        nvars--;
    }
    if (nvars != 0) { 
        error("stack underflow"); 
    }
}

#define DEFINE1(var) Obj** var = push(NIL)
#define DEFINE2(var1, var2) Obj** var1 = push(NIL); Obj** var2 = push(NIL)
#define DEFINE3(var1, var2, var3) \
    Obj** var1 = push(NIL); Obj** var2 = push(NIL); Obj** var3 = push(NIL)
#define FREE(n) pop(n)

// Since GC can happen in any call to the allocation functions, the following
// code might lead to undefined behavior:
//
//     (*obj)->car = alloc_string("this might fail");
//
// What if `(*obj)->car` is evaluated before the right hand side, but `alloc_string`
// triggers GC? `(*obj)` now points somewhere else but the target of the assignment
// will still point to the old memory. Hence these two macros:
#define SET_CAR(obj, exp) { Obj* tmp = exp; (*obj)->car = tmp; }
#define SET_CDR(obj, exp) { Obj* tmp = exp; (*obj)->cdr = tmp; }

Obj mem1[MEMSZ], mem2[MEMSZ];
Obj* mem = mem1; 
Obj* free_ptr = mem1;

Obj* gc_move(Obj* old_addr) {
    switch (old_addr->type) {
        case BOOLEAN:
        case EMPTY_LIST:
            return old_addr;
        case MOVED_BY_GC:
            return old_addr->forwarding_addr;
        default:
            *free_ptr = *old_addr;
            old_addr->type = MOVED_BY_GC;
            old_addr->forwarding_addr = free_ptr;
            return free_ptr++;
    }
}

void gc() {
    Obj* new_mem = (mem == mem1) ? mem2 : mem1;
    free_ptr = new_mem;
    
    for (int i = 0; i < (stack_ptr - stack); i++) {
        stack[i] = gc_move(stack[i]);
    }

    for(Obj* scan_ptr = new_mem; scan_ptr < free_ptr; scan_ptr++) {
        if ((scan_ptr->type == CONS) || (scan_ptr->type == LAMBDA)) {
            scan_ptr->car = gc_move(scan_ptr->car);
            scan_ptr->cdr = gc_move(scan_ptr->cdr);
        }
    }

    // Free dangling strings from memory
    for(Obj* sweep_ptr = mem; sweep_ptr < mem + MEMSZ; sweep_ptr++) {
        if ((sweep_ptr->type == STRING) || (sweep_ptr->type == SYMBOL)) {
            free(sweep_ptr->string);
            sweep_ptr->type = NUMBER; // TODO: is this necessary?
            sweep_ptr->string = NULL;
        }
    }

    if (free_ptr == new_mem + MEMSZ) {
        error("memory is full");
    }
    mem = new_mem;
}

Obj* alloc() {
#ifndef ALWAYS_GC
    if (free_ptr == mem + MEMSZ) {
        gc();
    }
#else
    gc(); // useful for finding gc bugs
#endif

    return free_ptr++;
}

Obj* alloc_cons(Obj** car, Obj** cdr) {
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
