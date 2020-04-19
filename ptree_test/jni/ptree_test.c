#define _GNU_SOURCE
#include "../../include/ptree.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Traverse `prinfo`s and print them
// @infos: array of `prinfo`s
// @nr: number of `prinfo`s
void traverse(struct prinfo *infos, int nr) {
    // Use a stack to represent the relationships
    struct prinfo **stack = malloc(sizeof(struct prinfo *) * nr);

    int top = -1;  // Top of stack, or count of tabs
    size_t i, j;
    struct prinfo *info;

    for (i = 0; i < nr; i++) {
        info = infos + i;  // Get the i-th one
        if (top == -1) {
            // Empty stack
            stack[++top] = info;
        } else if (stack[top]->first_child_pid == infos[i].pid) {
            // A child of the stack top -> push
            stack[++top] = info;
        } else if (stack[top]->next_sibling_pid == infos[i].pid) {
            // A sibling of the stack top -> replace
            stack[top] = info;
        } else {
            // Neither child nor sibling ->
            //  pop until we find its parent, then push
            while (stack[top]->pid != infos[i].parent_pid) --top;
            stack[++top] = info;
        }
        // Print the info
        for (j = 0; j < top; j++) printf("\t");
        printf("%-16s,%d,%ld,%d,%d,%d,%ld\n", info->comm, info->pid,
               info->state, info->parent_pid, info->first_child_pid,
               info->next_sibling_pid, info->uid);
    }

    free(stack);
}

int main() {
    // Allocate buffers on user stack
    struct prinfo infos[MAX_PRO_CNT];
    int nr;

    syscall(__NR_ptree, infos, &nr);
    printf("nr=%d\n", nr);

    // Traverse and print
    traverse(infos, nr);

    exit(0);
}
