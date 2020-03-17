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
    if (cpid == 0) {
        printf(STUDENT_ID " Child is %d\n", getpid());
        execl("./ptree_test", "ptree_test", NULL);
        exit(0);
    }
    wait(NULL);

    exit(0);
}
