#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");

static int __init inspiration_init(void) {
    printk(KERN_INFO "Starting the Inspirational Quote Machine");
    return 0;
}

static void __exit inspiration_exit(void) {
    printk(KERN_ALERT "Shutting Down the Inspirational Quote Machine");
}

module_init(inspiration_init);
module_exit(inspiration_exit);