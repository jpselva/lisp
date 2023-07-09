#include <stdio.h>

typedef enum {
    CONS,
    NUMBER,
    PRIMITIVE,
    STRING,
    SYMBOL,
    LAMBDA,
    SPECIAL_FORM,
    BOOLEAN,
    EMPTY_LIST,
    MOVED_BY_GC,
} Type;

typedef struct Obj* Env;
typedef struct Obj* (*Primitive)(struct Obj*);
typedef struct Obj* (*SpecialForm)(struct Obj*, Env);

typedef struct Obj {
    Type type; 
    union {
        int number;
        char* string;
        Primitive primitive;
        SpecialForm special_form;
        struct {
            struct Obj* car;
            struct Obj* cdr;
        };
        struct Obj* forwarding_addr;
    };
} Obj;

/**** error.c ****/
void error(const char* msg);

/**** constants.c ****/
#define TRUE &trueobj
#define FALSE &falseobj
#define NIL &nilobj
extern Obj trueobj;
extern Obj falseobj;
extern Obj nilobj;

/**** mem.c ****/
#define GC_TRACK1(v1) gc_track(&v1)
#define GC_TRACK2(v1, v2) gc_track(&v1), gc_track(&v2)
#define GC_TRACK3(v1, v2, v3) gc_track(&v1), gc_track(&v2), gc_track(&v3)
#define GC_RELEASE(n) gc_release(n)
#define SET_CAR(obj, value) { Obj* tmp = value; obj->car = tmp; }
#define SET_CDR(obj, value) { Obj* tmp = value; obj->cdr = tmp; }
void gc_track(Obj** var_ptr);
void gc_release(size_t nvars);
Obj* alloc_cons(Obj* car, Obj* cdr);
Obj* alloc_number(int num);
Obj* alloc_primitive(Primitive prim);
Obj* alloc_string(const char* str);
Obj* alloc_symbol(const char* sym);
Obj* alloc_special_form(SpecialForm sf);

/**** read.c ****/
char peek(FILE* stream);
Obj* read(FILE* stream);

/**** env.c ****/
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

/**** defs.c ****/
void setup_env(Env env);

/**** write.c ****/
void write(Obj* obj);
