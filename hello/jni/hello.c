#include "hello.h"

int main() {
    printf("Hello, Android!?\n");
    int i = syscall(356, 123, "Hello, Android!!!");
    printf("i = %d\n", i);
    return 0;
}
