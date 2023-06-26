#include <stdlib.h>
#include <string.h>
#include "lisp.h"
#define MEMSZ 1024

Obj mem[MEMSZ];
Obj* free_ptr = mem;

Obj* alloc() {
    if (free_ptr == mem + MEMSZ)
        error("memory is full");

    return free_ptr++;
}

Obj* alloc_cons(Obj* car, Obj* cdr) {
   Obj* new_obj = alloc(); 

   new_obj->type = CONS;
   new_obj->car = car;
   new_obj->cdr = cdr;

   return new_obj;
}

Obj* alloc_number(int num) {
   Obj* new_obj = alloc(); 

   new_obj->type = NUMBER;
   new_obj->number = num;

   return new_obj;
}

Obj* alloc_primitive(PrimitiveFunc prim) {
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

