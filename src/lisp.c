#include "lisp.h"

int main() {
    DEF3(env, exp, value);
    *env = extend_environment(&NIL, &NIL, &NIL);
    setup_env(env);
    
    printf("> ");
    while (peek(stdin) != EOF) {
        *exp = read(stdin); 
        *value = eval(exp, env);
        write(value);
        printf("\n> ");
    }

    RET(3, NIL);
    return 0;
}
