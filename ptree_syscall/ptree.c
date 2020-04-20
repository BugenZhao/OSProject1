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
static int task_to_prinfo(const struct task_struct *task, struct prinfo *info) {
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
// @kernbuf: kernel buffer of output `prinfo`s
// @knr: `nr` in kernel space
// @copied_cnt: # of `prinfo`s that we've copied
// @traversed_cnt: # of `prinfo`s that we've traverses
// @return: 0 if success, negative if failed
static int dfs(struct task_struct *task, struct prinfo *kernbuf, int knr,
               int *copied_cnt, int *traversed_cnt) {
    struct list_head *ptr;
    struct prinfo info;
    int ret;

    if (task == NULL) return;

    // Convert `task`
    if (task_to_prinfo(task, &info) != 0) {
        printk(KERN_ERR "error task_to_prinfo\n");
        return -1;
    }

    if (*copied_cnt < knr) {
        // Buffer is not full, write it
        kernbuf[*copied_cnt] = info;
        (*copied_cnt)++;
    }
    (*traversed_cnt)++;  // Traversed one more

    // For every child, call `dfs()`
    list_for_each(ptr, &task->children) {
        ret = dfs(list_entry(ptr, struct task_struct, sibling), kernbuf, knr,
                  copied_cnt, traversed_cnt);
        if (ret < 0) return ret;  // Error occured, do not search any more
    }

    return 0;
}

// Get real process count
// Note: may not be equals to our result
static int proc_count(void) {
    struct task_struct *p;
    int count = 0;
    for_each_process(p) { count++; }
    return count;
}

// Ptree system call
// @buf: user buffer to store the array of `prinfo`s
// @nr: user buffer to store the number of `prinfo`s
static int ptree_syscall(struct prinfo *buf, int *nr) {
    int traversed_cnt = 0;
    int copied_cnt = 0;
    int real_cnt = 0;
    int knr = 0;
    int dfs_ret = 0;
    struct prinfo *kernbuf;

    if (buf == NULL) return 0;

    // Get nr from user space
    if (copy_from_user(&knr, nr, sizeof(int)) != 0) {
        printk(KERN_ERR "error copy_from_user: nr\n");
        return 0;
    }

    // Allocate kernel buffer
    kernbuf = kmalloc(sizeof(struct prinfo) * knr, 0);
    if (kernbuf == NULL) {
        printk(KERN_INFO "error kmalloc: kernbuf\n");
        return 0;
    }

    // Begin the search
    write_lock_irq(&task_lock);  // Lock the task_struct
    dfs_ret = dfs(&init_task, kernbuf, knr, &copied_cnt, &traversed_cnt);
    real_cnt = proc_count();       // Get real process count
    write_unlock_irq(&task_lock);  // Unlock

    // Do some testing...
    //
    // Now we have serveral `count`s:
    // @traversed_cnt
    // @copied_cnt
    // @real_cnt: real process count
    // @knr: nr (in kernel space)
    //
    // Print them
    printk(KERN_INFO "traversed_cnt=%d\n", traversed_cnt);
    printk(KERN_INFO "copied_cnt   =%d\n", copied_cnt);
    printk(KERN_INFO "real_cnt     =%d\n", real_cnt);
    printk(KERN_INFO "knr          =%d\n", knr);

    printk(KERN_INFO "dfs_ret      =%d\n", dfs_ret);

    // Warn if something wrong
    WARN_ON(dfs_ret != 0);  // DFS did not complete
    WARN_ON(traversed_cnt != real_cnt);
    WARN_ON(copied_cnt != knr                 // Buffer size is enough...
            && copied_cnt != traversed_cnt);  // but we copied part of them

    if (dfs_ret == 0) {
        if (copy_to_user(buf, kernbuf, sizeof(struct prinfo) * knr) != 0) {
            // Copy the results to user buffers
            printk(KERN_INFO "error copy_to_user: buf\n");
            traversed_cnt = 0;
        } else if (copy_to_user(nr, &copied_cnt, sizeof(int)) != 0) {
            // Store `copied_cnt` to `nr`
            printk(KERN_ERR "error copy_to_user: nr\n");
            traversed_cnt = 0;
        }
    } else {
        traversed_cnt = 0;
    }
    
    kfree(kernbuf);        // Free buffer
    return traversed_cnt;  // Return `traversed_cnt`
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
