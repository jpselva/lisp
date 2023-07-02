#include "lisp.h"

int main() {
    Env env = extend_environment(NIL, NIL, NIL);
    setup_env(env);
    Obj* exp, *value;
    
    printf("> ");
    while (peek(stdin) != EOF) {
        exp = read(stdin); 
        value = eval(exp, env);
        write(value);
        printf("\n> ");
    }

    return 0;
}
