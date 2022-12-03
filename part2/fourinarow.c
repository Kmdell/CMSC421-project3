#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>

MODULE_LICENSE("GPL");

bool read = false;
bool game_started = false;

char* command = "";
char *output = "";

static ssize_t four_read(struct file *file, char __user *buf, size_t count, loff_t *ppos);
static ssize_t four_write(struct file *file, char __user *buf, size_t count, loff_t *ppos);
static int __init fourinarow_init(void);
static void __exit fourinarow_exit(void);

static const struct file_operations ins_fops = {
    .owner = THIS_MODULE,
    .read = four_read,
    .write = four_write,
};

static struct miscdevice ins_dev = {
    .minor = 0,
    .name = "fourinarow",
    .fops = &ins_fops,
    .mode = 0666,
};

static const char *board[] = {
    "00000000\n",
    "00000000\n",
    "00000000\n",
    "00000000\n",
    "00000000\n",
    "00000000\n",
    "00000000\n",
    "00000000\n"
};

static int __init fourinarow_init(void) {
    if (misc_register(&ins_dev) < 0) {
        printk(KERN_ERR "Cannot acquire four in a row device\n");
        return -ENOENT;
    }

    read = false;
    game_started = false;

    printk(KERN_ALERT "Started the four in a row machine\n");
    return 0;
}

