#include <stdio.h>

typedef enum {
    CONS,
    NUMBER,
    PRIMITIVE,
    STRING,
    SYMBOL,
    LAMBDA,
    MACRO,
    SPECIAL_FORM,
    BOOLEAN,
    EMPTY_LIST,
    MOVED_BY_GC,
    TAIL_CALL,
} Type;

typedef struct Obj* (*Primitive)(struct Obj* const* args);
typedef struct Obj* (*SpecialForm)(struct Obj* const* exps, struct Obj* const* env);

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
#define TRUE true_c
#define FALSE false_c
#define NIL nil_c
extern Obj* true_c;
extern Obj* false_c;
extern Obj* nil_c;

/**** mem.c ****/
#define DEF1(var) Obj** var = push(NIL)
#define DEF2(var1, var2) Obj** var1 = push(NIL); Obj** var2 = push(NIL)
#define DEF3(var1, var2, var3) \
    Obj** var1 = push(NIL); Obj** var2 = push(NIL); Obj** var3 = push(NIL)
#define DEF4(var1, var2, var3, var4) \
    Obj** var1 = push(NIL); Obj** var2 = push(NIL); \
    Obj** var3 = push(NIL); Obj** var4 = push(NIL)
#define RET(n, exp) popandreturn(n, exp)
#define SET_CAR(obj, exp) { Obj* _tmp = (exp); (obj)->car = _tmp; }
#define SET_CDR(obj, exp) { Obj* _tmp = (exp); (obj)->cdr = _tmp; }
Obj** push(Obj* addr);
Obj* popandreturn(size_t nvars, Obj* value);
void cleanup();
Obj* alloc_cons(Obj* const* car, Obj* const* cdr);
Obj* alloc_number(int num);
Obj* alloc_primitive(Primitive prim);
Obj* alloc_string(const char* str);
Obj* alloc_symbol(const char* sym);
Obj* alloc_special_form(SpecialForm sf);

/**** read.c ****/
char peek(FILE* stream);
Obj* read(FILE* stream);

/**** env.c ****/
Obj* extend_environment(Obj* const* vars, Obj* const* values, Obj* const* env);
void define_variable(Obj* const* var, Obj* const* value, Obj* const* env);
void set_variable(Obj* const* var, Obj* const* value, Obj* const* env);
Obj* lookup(Obj* const* var, Obj* const* env);

/**** eval.c ****/
Obj* eval(Obj* const* exp, Obj* const* env);
Obj* eval_sequence(Obj* const* exps, Obj* const* env);
Obj* eval_whole_sequence(Obj* const* body, Obj* const* env);
Obj* make_tail_call(Obj* const* exp, Obj* const* env);

/**** apply.c ****/
Obj* make_lambda(Obj* const* params, Obj* const* body, Obj* const* env);
Obj* apply(Obj* const* proc, Obj* const* args);

/**** defs.c ****/
void setup_env(Obj* const* env);

/**** write.c ****/
void write(Obj* const* obj);

/**** macro.c ****/
Obj* make_macro(Obj* const* params, Obj* const* body, Obj* const* env);
Obj* expand_macro(Obj* const* macro, Obj* const* exps);

