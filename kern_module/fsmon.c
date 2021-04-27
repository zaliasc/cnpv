# include <linux/module.h>
# include <linux/kernel.h>
# include <linux/syscalls.h>

# include "helper.h"

MODULE_LICENSE("GPL");

unsigned long **sct;

asmlinkage long fake_open(const char __user *filename, int flags, umode_t mode);

asmlinkage long (*real_open)(const char __user *filename, int flags, umode_t mode);

static int entry_point(void) {
    fm_alert("%s\n", "Greetings the World!");

    /* No consideration on failure. */
    sct = get_sct();

    disable_wp();
    HOOK_SCT(sct, open);
    enable_wp();

    return 0;
}

static void exit_point(void) {
    disable_wp();
    UNHOOK_SCT(sct, open);
    enable_wp();

    fm_alert("%s\n", "Farewell the World!");

    return;
}

module_init(entry_point);
module_exit(exit_point);

asmlinkage long fake_open(const char __user *filename, int flags, umode_t mode)
{
    if ((flags & O_CREAT) && strcmp(filename, "/dev/null") != 0) {
        fm_alert("open: %s\n", filename);
    }

    return real_open(filename, flags, mode);
}


