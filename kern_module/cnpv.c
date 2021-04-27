#include "cnpv.h"

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hook sys_call_open by change sys_open entry");

static void process_pair(json_value *pair) {
  if (pair->type != json_object) {
    log_err("parse pair failed");
  }

  for (int i = 0; i < pair->u.array.length; i++) {
    if (!strcmp(pair->u.object.values[i].name, "PATH")) {
      log_info("1111");
    } else if (!strcmp(pair->u.object.values[i].name, "AUTHORITY")) {
      log_info("2222");
    }
  }
}

static void process_array(json_value *array) {
  if (array->type != json_array) {
    log_err("parse array failed");
  }

  for (int i = 0; i < array->u.array.length; i++) {
    process_pair(array->u.array.values[i]);
  }
}

static void process_root(json_value *root) {
  if (root->type != json_object) {
    log_err("parse root failed");
  }

  for (int i = 0; i < root->u.object.length; i++) {
    // if (strstr(root->u.object.values[i].name, program_invocation_short_name))
    // {
    process_array(root->u.object.values[i].value);
    // }
  }

  return;
}

char config_path[] = "/home/zhuzhicheng/project/cnpv/ns_agent/config.json";

void config_init(void) {
  mm_segment_t fs;
  loff_t pos;
  fs = get_fs();
  set_fs(KERNEL_DS);

  struct kstat stat;
  int res = vfs_stat((const char __user *)config_path, &stat);
  int file_size = (int)stat.size;
  char * file_contents = (char *)kmalloc(file_size, GFP_KERNEL);

  struct file *fp;
  fp = filp_open(config_path, O_RDWR | O_CREAT, 0644);
  if (IS_ERR(fp)) {
    printk("create file error/n");
    return ;
  }
  pos = 0;
  vfs_read(fp, file_contents, file_size, &pos);
  printk("read: %s/n", file_contents);
  filp_close(fp, NULL);

  set_fs(fs);

  json_char *json;
  json_value *value;

  json = (json_char *)file_contents;

  value = json_parse(json, file_size);

  if (value == NULL) {
    log_err("Unable to parse config_file!");
    kfree(file_contents);
  }
  process_root(value);
  json_value_free(value);
  kfree(file_contents);
}

// -------------------------------------------------------------------------


/* The sys_call_table is read-only => must make it RW before replacing a syscall
 */

extern void config_init(void);

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

  config_init();

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