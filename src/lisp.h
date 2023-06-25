#include <stdio.h>

typedef enum {
    CONS,
    NUMBER,
    PRIMITIVE,
    STRING,
    SYMBOL,
    LAMBDA
} obj_type;

typedef struct obj* (*primitive_func)(struct obj*);

typedef struct obj {
    obj_type type; 
    union {
        double number;
        char* string;
        primitive_func primitive;
        struct {
            struct obj* car;
            struct obj* cdr;
        };
    };
} obj;

/**** error.c ****/
void error(const char* msg);

/**** mem.c ****/
obj* alloc_cons(obj* car, obj* cdr);
obj* alloc_number(double num);
obj* alloc_primitive(primitive_func prim);
obj* alloc_string(const char* str);
obj* alloc_symbol(const char* sym);

/**** read.c ****/
obj* read(FILE* stream);
