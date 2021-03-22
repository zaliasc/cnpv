#define _GNU_SOURCE

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/syscalls.h>
#include <linux/unistd.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hook sys_call_open by change sys_open entry");
MODULE_AUTHOR("curtis");

void	**sys_call_table_ptr;
unsigned long copied;

asmlinkage int (*original_open)( const char __user *, int , mode_t);

asmlinkage int new_open( const char __user * pathname, int flags, mode_t mode )
{
  char user_msg[256];
  printk("%s\n",__FUNCTION__);
  memset(user_msg,0,sizeof(user_msg));
  copied = strncpy_from_user(user_msg, pathname, sizeof(user_msg));
  printk("copied:%ld\n", copied);
  printk("pathname%s\n",user_msg);
  printk("------\n");
  return ( *original_open )( pathname, flags, mode );
}

static int __init open_hook_init(void)
{
  write_cr0(read_cr0() & (~0x10000));
	sys_call_table_ptr = (void**)kallsyms_lookup_name("sys_call_table");
	original_open = sys_call_table_ptr[__NR_open];
	sys_call_table_ptr[__NR_open] = new_open;
	write_cr0(read_cr0() | (0x10000));
	return 0;
}

static void __exit open_hook_exit(void)
{
  write_cr0(read_cr0() & (~0x10000));
	sys_call_table_ptr[__NR_open] = original_open;
	write_cr0(read_cr0() | (0x10000));
	printk("Bye bye open_hook\n");
}

module_init(open_hook_init);
module_exit(open_hook_exit);