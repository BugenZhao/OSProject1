#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <linux/unistd.h>

MODULE_LICENSE("Dual BSD/GPL");

#define __NR_hellocall 356

static int (*oldcall)(void);

static int sys_hellocall(int n, char *str) {
    printk("    uid = %d, str = '%s'\n", n, str);
    return n;
}

static int my_init(void) {
    long *syscall = (long *)0xc000d8c4;
    oldcall = (int (*)(void))(syscall[__NR_hellocall]);
    syscall[__NR_hellocall] = (unsigned long)sys_hellocall;
    printk("    HELLO MODULE LOADED\n");
    return 0;
}

static void my_exit(void) {
    long *syscall = (long *)0xc000d8c4;
    syscall[__NR_hellocall] = (unsigned long)oldcall;
    printk("    HELLO MODULE EXIT\n");
}

module_init(my_init);
module_exit(my_exit);
