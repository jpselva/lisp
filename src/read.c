#include <stdio.h>
#include <ctype.h>
#include "lisp.h"
#define MAXTEXT 256
#define isparens(c) ((c == '(') || (c == ')'))

char peek(FILE* stream) {
    char c;
    while (isspace(c = getc(stream)));
    ungetc(c, stream);
    return c;
}

Obj* read(FILE*);

static Obj* readlist(FILE* stream) {
    DEF3(head, exp, tmp);

    *head = alloc_cons(&NIL, &NIL);
    *exp = *head;
    char c;

    while ((c = peek(stream)) != ')') {
       if (c == EOF)
           error("expected closing ')' but got EOF");

       *tmp = read(stream);
       SET_CDR(*exp, alloc_cons(tmp, &NIL));
       *exp = (*exp)->cdr;
    }

    getc(stream); // read closing ')'

    return RET(3, (*head)->cdr);
}

static Obj* readstring(FILE* stream) {
    char string[MAXTEXT];
    char* s = string;

    while ((*s++ = getc(stream)) != '"') {
        if (s - string == MAXTEXT)
            error("max string length reached");
    }

    *--s = '\0';
    return alloc_string(string);
}


static Obj* readnumber(FILE* stream) {
    int number = 0;
    char c;

    while (isdigit(c = getc(stream))) {
        number *= 10;
        number += c - '0';
    }

    ungetc(c, stream);
    return alloc_number(number);
}

static Obj* readsymbol(FILE* stream) {
    char symbol[MAXTEXT];
    char* s = symbol;

    while (isgraph(*s++ = getc(stream)) && !isparens(*(s-1))) {
        if (s - symbol == MAXTEXT)
            error("max symbol length reached");
    }

    s -= 1;
    ungetc(*s, stream);
    *s = '\0';
    return alloc_symbol(symbol);
}

Obj* read(FILE* stream) {
    char c = peek(stream);

    if (c == '(') {
        getc(stream);
        return readlist(stream);
    } else if (c == '"') {
        getc(stream);
        return readstring(stream);
    } else if (isdigit(c)) {
        return readnumber(stream);
    } else {
        return readsymbol(stream);
    }
}

