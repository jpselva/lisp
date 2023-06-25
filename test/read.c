#include <string.h>
#include "test.h"

int main() {
    testsuite("read.c");
    char* mock_input = "(func arg1 (concat \"hello\" \"world\")\n\t(srqt 2))";
    FILE* stream = fmemopen(mock_input, strlen(mock_input), "r");

    obj* exp = read(stream);
    
    return 0;
}

