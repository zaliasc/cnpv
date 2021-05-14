#include "cnpv.h"
#include "map.h"
#include "types.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zhuzhicheng");
MODULE_DESCRIPTION("Hook sys_call_open by change sys_open entry");

extern int check_permission(const char *pathname, int oflag);

char target[MAX_PATH] = "mytest";

// hook releative
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

asmlinkage int my_open(const char __user *pathname, int flags, mode_t mode) {
  int (*real_open)(const char __user *, int, mode_t) =
      (int (*)(const char __user *, int, mode_t))sys_open_ptr;

  char user_msg[256];
  memset(user_msg, 0, sizeof(user_msg));
  unsigned long copied =
      strncpy_from_user(user_msg, pathname, sizeof(user_msg));

  if (!strcmp(current->comm, target)) {
    printk("%s (pid=%d, comm=%s)\n", __func__, current->pid, current->comm);
    if (check_permission(user_msg, flags) == 1) {
      printk("check path %s success", user_msg);
    } else {
      printk("check path %s failed", user_msg);
      // return 0;
    }
  }
  return (*real_open)(pathname, flags, mode);
}

// kernel version 4.15
asmlinkage long my_openat(int dfd, const char __user *filename, int flags,
                          mode_t mode) {
  long (*real_openat)(int dfd, const char __user *filename, int flags,
                      mode_t mode) =
      (long (*)(int dfd, const char __user *filename, int flags,
                mode_t mode))sys_openat_ptr;

  char user_msg[256];
  memset(user_msg, 0, sizeof(user_msg));
  unsigned long copied =
      strncpy_from_user(user_msg, filename, sizeof(user_msg));

  if (user_msg[0] == '/' && !strcmp(current->comm, target)) {
    printk("%s (pid=%d, comm=%s)\n", __func__, current->pid, current->comm);
    if (check_permission(user_msg, flags) == 1) {
      printk("check path %s success", user_msg);
    } else {
      printk("check path %s failed", user_msg);
      // return 0;
    }
  }

  // printk("%s. proc:%s, pid:%d, dfd:%d, filename:[%s], copy ret:%d\n", __func__,
  //        current->group_leader->comm, current->tgid, dfd, user_filename, ret);
  return (*real_openat)(dfd, filename, flags, mode);
}

// kenel version > 4.17
// asmlinkage long my_openat(const struct pt_regs *regs) {
//   long (*real_openat)(const struct pt_regs *) =
//       (long (*)(const struct pt_regs *))sys_openat_ptr;

//   int dfd = regs->di;
//   char __user *filename = (char *)regs->si;
//   char user_filename[256] = {0};
//   int ret = raw_copy_from_user(user_filename, filename,
//   sizeof(user_filename));
//   // if (!strcmp(current->comm, target)) {
//     printk("%s. proc:%s, pid:%d, dfd:%d, filename:[%s], copy ret:%d\n",
//            __func__, current->group_leader->comm, current->tgid, dfd,
//            user_filename, ret);
//   // }
//   return (*real_openat)(regs);
// }

static void hook_init(void) {
  unsigned long **syscall_table = acquire_syscall_table();

  // disable_page_protection();
  set_addr_rw((unsigned long)syscall_table);

  // now replace the syscal
  STORE_SYSCALLPTR(syscall_table, open);
  REPLACE_SYSCALL(syscall_table, open);
  STORE_SYSCALLPTR(syscall_table, openat);
  REPLACE_SYSCALL(syscall_table, openat);
  // enable_page_protection();
  set_addr_ro((unsigned long)syscall_table);

  printk(KERN_INFO "starting intercept\n");

  return;
}

static void hook_reset(void) {
  void **syscall_table = (void **)acquire_syscall_table();

  // disable_page_protection();
  set_addr_rw((unsigned long)syscall_table);

  // now recover the syscall
  RECOVER_SYSCALLPTR(syscall_table, open);
  RECOVER_SYSCALLPTR(syscall_table, openat);

  // enable_page_protection();
  set_addr_ro((unsigned long)syscall_table);

  printk(KERN_INFO "Bye bye open_hook\n");

  return;
}

// ---------------------------------------------------------------------------

#define NETLINK_USER 31

struct hashmap *map = NULL;

struct myuser *user_t;

struct mapuser mapuser_t;

int user_compare(const void *a, const void *b, void *udata) {
  const struct mapuser *ua = a;
  const struct mapuser *ub = b;
  return strcmp(ua->pathname, ub->pathname);
}

bool user_iter(const void *item, void *udata) {
  const struct mapuser *user = item;
  printk("%s (age=%d)\n", user->pathname, user->permission);
  return true;
}

uint64_t user_hash(const void *item, uint64_t seed0, uint64_t seed1) {
  const struct mapuser *user = item;
  return hashmap_sip(user->pathname, strlen(user->pathname), seed0, seed1);
}

struct sock *nl_sk = NULL;

static void hello_nl_recv_msg(struct sk_buff *skb) {
  struct nlmsghdr *nlh;
  int pid;
  struct sk_buff *skb_out;
  int msg_size;
  char *msg = "Hello from kernel";
  int res;

  msg_size = strlen(msg);

  nlh = (struct nlmsghdr *)skb->data;

  user_t = (struct myuser *)nlmsg_data(nlh);

  if (map == NULL) {
    map = hashmap_new(sizeof(struct mapuser), 0, 0, 0, user_hash, user_compare,
                      NULL);
  }

  switch (user_t->type) {
  case MYUSER: {
    printk("Netlink received msg payload:path: %s\n, permission: %d",
           user_t->pathname, user_t->permission);
    strncpy(mapuser_t.pathname, user_t->pathname, MAX_PATH);
    mapuser_t.permission = user_t->permission;
    hashmap_set(map, &mapuser_t);
    printk("hash map count : %d", (int)hashmap_count(map));
    break;
  }
  case CMD: {
    if (!strcmp(user_t->pathname, "clear")) {
      hashmap_clear(map, 0);
      printk("clear hashmap");
      break;
    } else if (strstr(user_t->pathname, "tar-")) {
      strcpy(target, user_t->pathname + 4);
      printk("set tar: %s", target);
    }
  }
  case STR:
  default:
    break;
  }

  pid = nlh->nlmsg_pid; /*pid of sending process */

  skb_out = nlmsg_new(msg_size, 0);

  if (!skb_out) {
    printk(KERN_ERR "Failed to allocate new skb\n");
    return;
  }
  nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
  NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */
  strncpy(nlmsg_data(nlh), msg, msg_size);

  res = nlmsg_unicast(nl_sk, skb_out, pid);

  if (res < 0)
    printk(KERN_INFO "Error while sending bak to user\n");
}

static int __init cnpv_init(void) {

  printk("Entering: %s\n", __FUNCTION__);
  // This is for 3.6 kernels and above.
  struct netlink_kernel_cfg cfg = {
      .input = hello_nl_recv_msg,
  };

  nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
  // nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, 0,
  // hello_nl_recv_msg,NULL,THIS_MODULE);
  if (!nl_sk) {
    printk(KERN_ALERT "Error creating socket.\n");
    return -10;
  }

  hook_init();

  return 0;
}

static void __exit cnpv_exit(void) {

  printk(KERN_INFO "exiting hello module\n");

  netlink_kernel_release(nl_sk);

  hook_reset();
}

module_init(cnpv_init);
module_exit(cnpv_exit);