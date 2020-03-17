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

static int task2prinfo(const struct task_struct *task, struct prinfo *info) {
    if (task == NULL) return -1;

    memset(info, 0, sizeof(struct prinfo));
    if (task->parent) info->parent_pid = task->parent->pid;
    info->pid = task->pid;
    info->state = task->state;
    info->uid = task->cred->uid;
    memcpy(info->comm, task->comm, TASK_COMM_LEN);

    if (!list_empty(&task->children))
        info->first_child_pid =
            list_entry(task->children.next, struct task_struct, sibling)->pid;
    if (!list_empty(&task->sibling))
        info->next_sibling_pid =
            list_entry(task->sibling.next, struct task_struct, sibling)->pid;

    return 0;
}

static void dfs(struct task_struct *task, struct prinfo *infos, int *idx) {
    struct list_head *ptr;
    // struct task_struct *tmp_task;

    if (task == NULL) return;

    if (task2prinfo(task, infos + (*idx)) != 0)
        printk(KERN_ERR "task2prinfo error\n");
    (*idx)++;

    list_for_each(ptr, &task->children) {
        dfs(list_entry(ptr, struct task_struct, sibling), infos, idx);
    }
}

static int ptree_syscall(struct prinfo *buf, int *nr) {
    struct prinfo *kbuf;
    int knr = 0;

    kbuf = kmalloc(sizeof(struct prinfo) * MAX_PRO_CNT, 0);

    read_lock(&task_lock);
    dfs(&init_task, kbuf, &knr);
    read_unlock(&task_lock);

    if (copy_to_user(buf, kbuf, sizeof(struct prinfo) * knr) != 0)
        printk(KERN_ERR "copy prinfos error\n");
    if (copy_to_user(nr, &knr, sizeof(int)) != 0)
        printk(KERN_ERR "copy nr error\n");

    kfree(kbuf);
    return knr;
}

static int ptree_init(void) {
    long *syscall = (long *)0xc000d8c4;
    oldcall = (int (*)(void))(syscall[__NR_ptree]);
    syscall[__NR_ptree] = (unsigned long)ptree_syscall;
    printk(KERN_INFO "PTREE MODULE LOADED\n");
    return 0;
}

static void ptree_exit(void) {
    long *syscall = (long *)0xc000d8c4;
    syscall[__NR_ptree] = (unsigned long)oldcall;
    printk(KERN_INFO "PTREE MODULE EXIT\n");
}

module_init(ptree_init);
module_exit(ptree_exit);
