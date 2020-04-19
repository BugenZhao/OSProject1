#ifndef BUGEN_LINUX_PTREE_H
#define BUGEN_LINUX_PTREE_H

#ifdef __KERNEL__
// Are we in kernel?
#include <linux/sched.h>
#include <linux/types.h>
#else
#include <stdlib.h>
#define TASK_COMM_LEN 16
#endif

#define MAX_PRO_CNT 1 << 10

struct prinfo {
    pid_t parent_pid;         /* process id of parent, 0 if none */
    pid_t pid;                /* process id */
    pid_t first_child_pid;    /* pid of youngest child, 0 if none */
    pid_t next_sibling_pid;   /* pid of older sibling, 0 if none */
    long state;               /* current state of process */
    long uid;                 /* user id of process owner */
    char comm[TASK_COMM_LEN]; /* name of program executed */
};

#define __NR_ptree 356

#endif  // BUGEN_LINUX_PTREE_H
