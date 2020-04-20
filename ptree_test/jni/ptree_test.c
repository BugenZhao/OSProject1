#include "../../include/ptree.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_PRO_CNT 1 << 10

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

// My test for ptree
void bugens_test(void) {
    // Emulate the case, where the buffer size is not enough
    struct prinfo infos[MAX_PRO_CNT];
    int nr = 10;
    int ret = 0;
    // Write 0x88
    memset(infos, 0x88, sizeof(struct prinfo) * MAX_PRO_CNT);

    ret = syscall(__NR_ptree, infos, &nr);
    if (nr != 10) {
        // It is believed that there must exist 10 processes
        fprintf(stderr, "TEST ERROR: nr==%d\n", nr);
    } else if (ret < nr) {
        // Traversed must be greater than copied
        fprintf(stderr, "TEST ERROR: ret<nr\n");
    } else if (*(uint64_t *)(infos + 9) == 0x8888888888888888u) {
        // infos[9] must be written
        fprintf(stderr, "TEST ERROR: *(uint64_t *)(infos + 9)==%ull\n",
                *(uint64_t *)(infos + 9));
    } else if (*(uint64_t *)(infos + 10) != 0x8888888888888888u) {
        // infos[10] must NOT be touched
        fprintf(stderr, "TEST ERROR: *(uint64_t *)(infos + 10)==%ull\n",
                *(uint64_t *)(infos + 10));
    } else {
        fprintf(stderr, "TEST PASSED\n");
    }
}

int main(int argc, char **argv) {
    // Do some test first
    bugens_test();

    // Allocate buffers on user stack
    struct prinfo infos[MAX_PRO_CNT];
    int nr = MAX_PRO_CNT;
    int ret = 0;

    ret = syscall(__NR_ptree, infos, &nr);
    printf(
        "nr (copied)    = %d\n"
        "ret(traversed) = %d\n\n",
        nr, ret);

    // Traverse and print
    traverse(infos, nr);

    exit(0);
}
