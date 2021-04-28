#include <linux/module.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <net/sock.h>
#include <linux/string.h>

#include "map.h"
#include "types.h"

#define NETLINK_USER 31

struct hashmap *map = NULL;

struct myuser * user_t ;

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

  printk(KERN_INFO "Entering: %s\n", __FUNCTION__);

  msg_size = strlen(msg);

  nlh = (struct nlmsghdr *)skb->data;

  user_t = (struct myuser *)nlmsg_data(nlh);

  if (map == NULL) {
    map = hashmap_new(sizeof(struct mapuser), 0, 0, 0, user_hash, user_compare,
                      NULL);
  }

  switch (user_t->type) {
  case MYUSER:
    printk(KERN_INFO "Netlink received msg payload:path: %s\n, permission: %d",
           user_t->pathname, user_t->permission);
    strncpy(mapuser_t.pathname, user_t->pathname, MAX_PATH);
    mapuser_t.permission = user_t->permission;
    hashmap_set(map, &mapuser_t);
    printk("hash map count : %d", (int)hashmap_count(map));
    break;
  case CMD:
  case STR:
  default:
    break;
  }

  // printk(KERN_INFO "Netlink received msg payload:%s\n",
  //        (char *)nlmsg_data(nlh));

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

static int __init hello_init(void) {

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

  return 0;
}

static void __exit hello_exit(void) {

  printk(KERN_INFO "exiting hello module\n");
  netlink_kernel_release(nl_sk);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");