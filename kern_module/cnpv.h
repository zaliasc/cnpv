#ifndef _SYSCALL_INTERCEPT_H_
#define _SYSCALL_INTERCEPT_H_

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/syscalls.h>
#include <linux/pid>
#include <linux/unistd.h>

#define STORE_SYSCALLPTR(table, name) sys_##name##_ptr = (void *)table[__NR_##name]
#define REPLACE_SYSCALL(table, name) table[__NR_##name] = (void *)my_##name
#define RECOVER_SYSCALLPTR(table, name) table[__NR_##name] = sys_##name##_ptr

// INFO: ``fn`` is short for ``__func__``.
#define fn_printk(level, fmt, ...) \
    printk(level "%s: " fmt, __func__, ##__VA_ARGS__)

// INFO: ``fm`` is short for ``__func__`` and ``module``.
#define fm_printk(level, fmt, ...) \
    printk(level "%s.%s: " fmt,    \
           THIS_MODULE->name, __func__, ##__VA_ARGS__)

// INFO: I only use ``pr_alert`` at present.
// TODO: When wanting more, e.g. ``pr_info``, I will add them.
#define fn_alert(fmt, ...) \
    fn_printk(KERN_ALERT, fmt, ##__VA_ARGS__)

#define fm_alert(fmt, ...) \
    fm_printk(KERN_ALERT, fmt, ##__VA_ARGS__)

void *sys_open_ptr;
void *sys_openat_ptr;
asmlinkage int 
my_open(const char __user *pathname, int flags, mode_t mode);

unsigned long **
acquire_syscall_table(void);

#endif