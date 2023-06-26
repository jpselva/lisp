#include <stdio.h>

typedef enum {
    CONS,
    NUMBER,
    PRIMITIVE,
    STRING,
    SYMBOL,
    LAMBDA
} Type;

typedef struct Obj* (*PrimitiveFunc)(struct Obj*);

typedef struct Obj {
    Type type; 
    union {
        int number;
        char* string;
        PrimitiveFunc primitive;
        struct {
            struct Obj* car;
            struct Obj* cdr;
        };
    };
} Obj;

/**** error.c ****/
void error(const char* msg);

/**** mem.c ****/
Obj* alloc_cons(Obj* car, Obj* cdr);
Obj* alloc_number(int num);
Obj* alloc_primitive(PrimitiveFunc prim);
Obj* alloc_string(const char* str);
Obj* alloc_symbol(const char* sym);

/**** read.c ****/
Obj* read(FILE* stream);

/**** env.c ****/
typedef Obj* Env;
Obj* extend_environment(Obj* vars, Obj* values, Env env);
void define_variable(Obj* var, Obj* value, Env env);
void set_variable(Obj* var, Obj* value, Env env);
Obj* lookup(Obj* var, Env env);

/**** eval.c ****/
Obj* eval(Obj* exp, Env env);
Obj* eval_sequence(Obj* exps, Env env);

/**** apply.c ****/
#define lambda_params(lambda) lambda->car
#define lambda_body(lambda) lambda->cdr->car
#define lambda_env(lambda) lambda->cdr->cdr
typedef Obj* Lambda;
Lambda make_lambda(Obj* params, Obj* body, Env env);
Obj* apply(Obj* proc, Obj* args);

