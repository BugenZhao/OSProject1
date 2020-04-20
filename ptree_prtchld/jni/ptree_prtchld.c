#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define STUDENT_ID "518030910211"

int main() {
    pid_t cpid;
    char *cargv[2];

    printf(STUDENT_ID " Parent is %d\n", getpid());

    cpid = fork();
    if (cpid < 0) {
        fprintf(stderr, "fork error\n");
        exit(-1);
    }
    if (cpid == 0) {  // Child
        printf(STUDENT_ID " Child  is %d\n", getpid());
        execl("./ptree_test", "ptree_test", NULL);  // Execute `ptree_test`
        fprintf(stderr, "execl error\n");           // Should never reach here
        exit(-1);
    }
    wait(NULL);  // Parent: wait for the child to terminate

    exit(0);
}
