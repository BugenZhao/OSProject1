#define _GNU_SOURCE
#include "../../include/ptree.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    struct prinfo infos[MAX_PRO_CNT];
    int nr;
    int i;

    syscall(__NR_ptree, infos, &nr);
    printf("nr=%d\n", nr);

    for (i = 0; i < nr; i++) {
        printf("%d, %s\n", infos[i].pid, infos[i].comm);
    }

    exit(0);
}
