#include "lisp.h"

void write(Obj** obj) {
    switch ((*obj)->type) {
        case CONS: {
            DEF1(tmp);

            *tmp = (*obj)->car;
            putchar('(');
            write(tmp);

            printf(" . ");

            *tmp = (*obj)->cdr;
            write(tmp);
            putchar(')');

            RET(1, NIL);
            break;
       } case NUMBER:
            printf("%d", (*obj)->number);
            break;
        case PRIMITIVE:
            printf("primitive %p", (*obj)->primitive);
            break;
        case STRING:
            printf("\"%s\"", (*obj)->string);
            break;
        case SYMBOL:
            printf("%s", (*obj)->string);
            break;
        case LAMBDA:
            printf("compound procedure");
            break;
        case SPECIAL_FORM:
            printf("special form");
            break;
        case BOOLEAN:
            (*obj == TRUE) ? printf("#t") : printf("#f");
            break;
        case EMPTY_LIST:
            printf("()");
            break;
        default:
            error("cannot write object");
    }
}
