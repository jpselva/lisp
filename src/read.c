#include <stdio.h>
#include <ctype.h>
#include "lisp.h"
#define MAXTEXT 256

char peek(FILE* stream) {
    char c;
    while (isspace(c = getc(stream)));
    ungetc(c, stream);
    return c;
}

obj* read(FILE*);

obj* readlist(FILE* stream) {
    obj head = { .cdr = NULL };
    obj* exp = &head;
    char c;

   while ((c = peek(stream)) != ')') {
       if (c == EOF)
           error("expected closing ')' but got EOF");
       exp->cdr = alloc_cons(read(stream), NULL);
       exp = exp->cdr;
   }

   getc(stream); // read closing ')'
   return head.cdr;
}

obj* readstring(FILE* stream) {
    char string[MAXTEXT];
    char* s = string;

    while ((*s = getc(stream)) != '"') {
        if (s - string == MAXTEXT)
            error("max string length reached");
    }

    *s = '\0';
    return alloc_string(string);
}

obj* readsymbol(FILE* stream) {
    char symbol[MAXTEXT];
    char* s = symbol;

    while (isalnum(*s = getc(stream))) {
        if (s - symbol == MAXTEXT)
            error("max symbol length reached");
    }

    ungetc(*s, stream);
    *s = '\0';
    return alloc_symbol(symbol);
}

obj* read(FILE* stream) {
    char c = peek(stream);

    if (c == '(') {
        getc(stream);
        return readlist(stream);
    } else if (c == '"') {
        getc(stream);
        return readstring(stream);
    } else if (isalpha(c)) {
        return readsymbol(stream);
    } else
        error("unexpected character");
}

