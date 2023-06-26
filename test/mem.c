#include <string.h>
#include "test.h"

extern Obj* free_ptr;
extern Obj mem[];

int main() {
   testsuite("mem.c");

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

   return 0;
}
