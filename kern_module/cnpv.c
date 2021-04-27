#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pid.h>

static int __init myled_init(void) {
  printk("myled drvier init !\n");
  return 0;
}

static int __exit myled_exit(void) {
  printk("myled drvier exit !\n");
  return 0;
}

module_init(myled_init);
module_exit(myled_exit);
MODULE_LICENSE("GPL");