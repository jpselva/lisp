#include <stdlib.h>
#include <string.h>
#include "lisp.h"
#define MEMSZ 1024

obj mem[MEMSZ];
obj* free_ptr = mem;

obj* alloc() {
    if (free_ptr == mem + MEMSZ)
        error("memory is full");

    return free_ptr++;
}

obj* alloc_cons(obj* car, obj* cdr) {
   obj* new_obj = alloc(); 

   new_obj->type = CONS;
   new_obj->car = car;
   new_obj->cdr = cdr;

   return new_obj;
}

obj* alloc_number(int num) {
   obj* new_obj = alloc(); 

   new_obj->type = NUMBER;
   new_obj->number = num;

   return new_obj;
}

obj* alloc_primitive(primitive_func prim) {
   obj* new_obj = alloc(); 

   new_obj->type = PRIMITIVE;
   new_obj->primitive = prim;

   return new_obj;
}

obj* alloc_string(const char* str) {
   obj* new_obj = alloc(); 
   char* copied_str = malloc(sizeof(char) * strlen(str));
   strcpy(copied_str, str);

   new_obj->type = STRING;
   new_obj->string = copied_str;

   return new_obj;
}

obj* alloc_symbol(const char* sym) {
    obj* scan_ptr;
    for (scan_ptr = mem; scan_ptr < free_ptr; scan_ptr++) {
        if (scan_ptr->type == SYMBOL && !strcmp(scan_ptr->string, sym)) {
            return scan_ptr;
        }
    }

   obj* new_obj = alloc(); 
   char* copied_sym = malloc(sizeof(char) * strlen(sym));
   strcpy(copied_sym, sym);

   new_obj->type = SYMBOL;
   new_obj->string = copied_sym;

   return new_obj;
}

