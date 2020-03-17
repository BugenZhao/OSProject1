#define _GNU_SOURCE
#include "../../include/ptree.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void traverse(struct prinfo *infos, int nr) {
    struct prinfo **stack = malloc(sizeof(struct prinfo *) * nr);
    int top = -1;
    size_t tab_cnt = 0;
    size_t i, j;
    struct prinfo *info;

    for (i = 0; i < nr; i++) {
        info = infos + i;
        if (top == -1) {
            stack[++top] = info;
        } else if (stack[top]->first_child_pid == infos[i].pid) {
            stack[++top] = info;
        } else if (stack[top]->next_sibling_pid == infos[i].pid) {
            stack[top] = info;
        } else {
            while (stack[top]->pid != infos[i].parent_pid) --top;
            stack[++top] = info;
        }
        for (j = 0; j < top; j++) printf("\t");
        printf("%-16s,%d,%ld,%d,%d,%d,%ld\n", info->comm,
               info->pid, info->state, info->parent_pid, info->first_child_pid,
               info->next_sibling_pid, info->uid);
    }

    free(stack);
}

int main() {
    struct prinfo infos[MAX_PRO_CNT];
    int nr;
    size_t i;

    syscall(__NR_ptree, infos, &nr);
    printf("nr=%d\n", nr);

    traverse(infos, nr);

    exit(0);
}
