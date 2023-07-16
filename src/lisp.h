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

typedef struct Obj* (*Primitive)(struct Obj** args);
typedef struct Obj* (*SpecialForm)(struct Obj** exps, struct Obj** env);

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
Obj* alloc_cons(Obj** car, Obj** cdr);
Obj* alloc_number(int num);
Obj* alloc_primitive(Primitive prim);
Obj* alloc_string(const char* str);
Obj* alloc_symbol(const char* sym);
Obj* alloc_special_form(SpecialForm sf);

/**** read.c ****/
char peek(FILE* stream);
Obj* read(FILE* stream);

/**** env.c ****/
Obj* extend_environment(Obj** vars, Obj** values, Obj** env);
void define_variable(Obj** var, Obj** value, Obj** env);
void set_variable(Obj** var, Obj** value, Obj** env);
Obj* lookup(Obj** var, Obj** env);

/**** eval.c ****/
Obj* eval(Obj** exp, Obj** env);
Obj* eval_sequence(Obj** exps, Obj** env);

/**** apply.c ****/
Obj* make_lambda(Obj** params, Obj** body, Obj** env);
Obj* apply(Obj** proc, Obj** args);

/**** defs.c ****/
void setup_env(Obj** env);

/**** write.c ****/
void write(Obj** obj);
