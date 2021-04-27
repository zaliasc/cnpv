#include "cnpv.h"

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hook sys_call_open by change sys_open entry");

/* The sys_call_table is read-only => must make it RW before replacing a syscall
 */
void set_addr_rw(unsigned long addr) {

  unsigned int level;
  pte_t *pte = lookup_address(addr, &level);

  if (pte->pte & ~_PAGE_RW)
    pte->pte |= _PAGE_RW;
}

/* Restores the sys_call_table as read-only */
void set_addr_ro(unsigned long addr) {

  unsigned int level;
  pte_t *pte = lookup_address(addr, &level);

  pte->pte = pte->pte & ~_PAGE_RW;
}

unsigned long **acquire_syscall_table(void) {
  return (unsigned long **)kallsyms_lookup_name("sys_call_table");
}

unsigned long copied;

asmlinkage int my_open(const char __user *pathname, int flags, mode_t mode) {
  int (*real_open)(const char __user *, int, mode_t) =
      (int (*)(const char __user *, int, mode_t))sys_open_ptr;

  char user_msg[256];
  memset(user_msg, 0, sizeof(user_msg));
  copied = strncpy_from_user(user_msg, pathname, sizeof(user_msg));
  printk("%s\n", __FUNCTION__);
  printk("copied : %ld\n", copied);
  printk("pathname : %s\n", user_msg);
  printk("%s (pid=%d, comm=%s)\n", __func__, current->pid, current->comm);

  // struct pid *ppid;      //定义pid结构指针
  // struct task_struct *p; //定义进程控制块指针
  // struct task_struct *pos;

  // ppid = current->pid;

  // ppid = find_get_pid(
  //     _pid); //_pid是用户空间传入的进程号，该函数根据进程号找到对应pid struct
  // p = pid_task(ppid, PIDTYPE_PID); //根据pid struct找到对应进程控制块

  printk("------\n");

  return (*real_open)(pathname, flags, mode);
}

static int __init open_hook_init(void) {
  unsigned long **syscall_table = acquire_syscall_table();

  // disable_page_protection();
  set_addr_rw((unsigned long)syscall_table);

  // now replace the syscal
  STORE_SYSCALLPTR(syscall_table, open);
  REPLACE_SYSCALL(syscall_table, open);

  // enable_page_protection();
  set_addr_ro((unsigned long)syscall_table);

  printk(KERN_INFO "starting intercept\n");

  return 0;
}

static void __exit open_hook_exit(void) {
  void **syscall_table = (void **)acquire_syscall_table();

  // disable_page_protection();
  set_addr_rw((unsigned long)syscall_table);

  // now recover the syscall
  RECOVER_SYSCALLPTR(syscall_table, open);

  // enable_page_protection();
  set_addr_ro((unsigned long)syscall_table);

  printk(KERN_INFO "Bye bye open_hook\n");

  return;
}

module_init(open_hook_init);

module_exit(open_hook_exit);