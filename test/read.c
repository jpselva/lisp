#include <string.h>
#include "test.h"

int main() {
    testsuite("read.c");

    /**** test 1 ****/
    char* mock_input = "(func arg1 (concat \"hello\" \"world\")\n\t(+ 7 25))";
    FILE* stream = fmemopen(mock_input, strlen(mock_input), "r");

    Obj* exp = read(stream);
    test("whole list", exp->type == CONS);

    Obj* exp1 = exp->car;
    test("symbol operator", (exp1->type == SYMBOL));
    test("symbol operator", !strcmp(exp1->string, "func"));
    
    Obj* exp2 = exp->cdr->car;
    test("symbol operand", (exp2->type == SYMBOL));
    test("symbol operand", !strcmp(exp2->string, "arg1"));

    Obj* exp3 = exp->cdr->cdr->car;
    test("sublist operand", exp3->type == CONS);
    test("sublist operand", exp3->car->type == SYMBOL);
    test("sublist operand", !strcmp(exp3->car->string, "concat"));
    test("sublist operand", exp3->cdr->car->type == STRING);
    test("sublist operand", !strcmp(exp3->cdr->car->string, "hello"));
    test("sublist operand", exp3->cdr->cdr->car->type == STRING);
    test("sublist operand", !strcmp(exp3->cdr->cdr->car->string, "world"));
    test("sublist operand", exp3->cdr->cdr->cdr == NIL);

    Obj* exp4 = exp->cdr->cdr->cdr->car;
    test("math operators", exp4->car->type == SYMBOL);
    test("math operators", !strcmp(exp4->car->string, "+"));
    test("numbers 1", exp4->cdr->car->type == NUMBER);
    test("numbers 1", exp4->cdr->car->number == 7);
    test("numbers 2", exp4->cdr->cdr->car->type == NUMBER);
    test("numbers 2", exp4->cdr->cdr->car->number == 25);

    Obj* end = exp->cdr->cdr->cdr->cdr;
    test("ends list with NIL", end == NIL);
    
    fclose(stream);

    /**** test 2 ****/
    char* mock_input2 = "()";
    FILE* stream2 = fmemopen(mock_input2, strlen(mock_input2), "r");

    exp = read(stream2);
    test("empty list", exp == NIL);

    fclose(stream2);

    /**** test 3 ****/
    char* mock_input3 = "(())";
    FILE* stream3 = fmemopen(mock_input3, strlen(mock_input3), "r");

    exp = read(stream3);
    test("empty list inside list", exp->car == NIL);

    fclose(stream3);

    return 0;
}

