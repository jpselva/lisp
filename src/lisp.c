#include "lisp.h"

int main() {
    DEF3(env, exp, value);
    *env = extend_environment(&NIL, &NIL, &NIL);
    setup_env(env);
    
#ifndef QUIET
        printf("> ");
#endif
    while (peek(stdin) != EOF) {
        *exp = read(stdin); 
        *value = eval(exp, env);
        write(value);
#ifndef QUIET
        printf("\n> ");
#endif
    }

    RET(3, NIL);
    return 0;
}
