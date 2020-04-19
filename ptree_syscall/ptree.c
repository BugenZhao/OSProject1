#include "../include/ptree.h"

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/unistd.h>

MODULE_LICENSE("Dual BSD/GPL");

static int (*oldcall)(void);

// Convert a kernel's `task_struct` instance to a `prinfo` instance
// @task: a `task_struct` instance
// @info: a `prinfo` instance
static int task2prinfo(const struct task_struct *task, struct prinfo *info) {
    if (task == NULL) return -1;

    memset(info, 0, sizeof(struct prinfo));
    if (task->parent) info->parent_pid = task->parent->pid;  // Task's parent
    info->pid = task->pid;                                   // Task's pid
    info->state = task->state;                      // Task's running state
    info->uid = task->cred->uid;                    // Task's user id
    memcpy(info->comm, task->comm, TASK_COMM_LEN);  // Task's command

    if (!list_empty(&task->children))  // Has children
        info->first_child_pid =
            list_entry(task->children.next, struct task_struct, sibling)->pid;
    if (!list_empty(&task->sibling))  // Has sibling
        info->next_sibling_pid =
            list_entry(task->sibling.next, struct task_struct, sibling)->pid;

    return 0;
}

// Convert `task_struct`s into `prinfo`s, in a depth-first order
// @task: array of input `task_struct`s
// @infos: array of output `prinfo`s
// @idx: number of `prinfo`s, initially 0
static void dfs(struct task_struct *task, struct prinfo *infos, int *idx) {
    struct list_head *ptr;

    if (task == NULL) return;

    // Convert `task`
    if (task2prinfo(task, infos + (*idx)) != 0)
        printk(KERN_ERR "task2prinfo error\n");
    (*idx)++;

    // For every child, call `dfs()`
    list_for_each(ptr, &task->children) {
        dfs(list_entry(ptr, struct task_struct, sibling), infos, idx);
    }
}

// Ptree system call
// @buf: user buffer to store the array of `prinfo`s
// @nr: user buffer to store the number of `prinfo`s
static int ptree_syscall(struct prinfo *buf, int *nr) {
    struct prinfo *kbuf;
    int knr = 0;

    kbuf = kmalloc(sizeof(struct prinfo) * MAX_PRO_CNT, 0);  // Alloc kernel buf

    read_lock(&task_lock);        // Get the task_lock
    dfs(&init_task, kbuf, &knr);  // Do dfs
    read_unlock(&task_lock);      // Release

    // Copy the results to user buffers
    if (copy_to_user(buf, kbuf, sizeof(struct prinfo) * knr) != 0)
        printk(KERN_ERR "copy prinfos error\n");
    if (copy_to_user(nr, &knr, sizeof(int)) != 0)
        printk(KERN_ERR "copy nr error\n");

    kfree(kbuf);  // Free kernel buf
    return knr;
}

// Initialization of module
static int ptree_init(void) {
    long *syscall = (long *)0xc000d8c4;                  // Syscall table
    oldcall = (int (*)(void))(syscall[__NR_ptree]);      // Save the old one
    syscall[__NR_ptree] = (unsigned long)ptree_syscall;  // Set this one
    printk(KERN_INFO "PTREE MODULE LOADED\n");
    return 0;
}

// Exit of module
static void ptree_exit(void) {
    long *syscall = (long *)0xc000d8c4;
    syscall[__NR_ptree] = (unsigned long)oldcall;  // Restore the old one
    printk(KERN_INFO "PTREE MODULE EXIT\n");
}

module_init(ptree_init);
module_exit(ptree_exit);
