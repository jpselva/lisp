#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "lisp.h"
#define MEMSZ 1024
#define STKSZ 256 // < MEMSZ

Obj mem1[MEMSZ], mem2[MEMSZ];
Obj* mem = mem1; 
Obj* free_ptr = mem1;

Obj** stack[STKSZ];
Obj*** stack_ptr = stack;

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
        *stack[i] = gc_move(*stack[i]);
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
            sweep_ptr->type = NUMBER; // or anything that's not STRING or SYMBOL (TODO: improve)
            sweep_ptr->string = NULL;
        }
    }

    if (free_ptr == new_mem + MEMSZ) {
        error("memory is full");
    }
    mem = new_mem;
}

void gc_track(Obj** var_ptr) {
    if (stack_ptr == stack + STKSZ) {
        error("stack overflow");
    }
    *stack_ptr++ = var_ptr;
}

void gc_release(size_t nvars) {
    while((nvars > 0) && (stack_ptr > stack)) {
        stack_ptr--;
        nvars--;
    }
    if (nvars != 0) { 
        error("stack underflow"); 
    }
}

#define GC_TRACK1(v1) gc_track(&v1)
#define GC_TRACK2(v1, v2) gc_track(&v1), gc_track(&v2)
#define GC_TRACK3(v1, v2, v3) gc_track(&v1), gc_track(&v2), gc_track(&v3)
#define GC_RELEASE(n) gc_release(n) // just to match the GC_TRACK* macros

// Since GC can happen in any call to the allocation functions, the following
// code might lead to undefined behavior:
//
//     obj->car = alloc_string("this might fail");
//
// What if `obj->car` is evaluated before the right hand side, but `alloc_string`
// triggers GC? `obj` now points somewhere else but the target of the assignment
// will still point to the old memory. Hence these two macros:
#define SET_CAR(obj, value) { Obj* tmp = value; obj->car = tmp; }
#define SET_CDR(obj, value) { Obj* tmp = value; obj->cdr = tmp; }

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

Obj* alloc_cons(Obj* car, Obj* cdr) {
    GC_TRACK2(car, cdr);
    Obj* new_obj = alloc(); 

    new_obj->type = CONS;
    new_obj->car = car;
    new_obj->cdr = cdr;

    GC_RELEASE(2);
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
