#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>

MODULE_LICENSE("GPL");

/* The constants of strings that we want to be able to edit*/
const char *OK = "OK\n";
const int OKLENGTH = 3;
const char *TURN = "OOT\n";
const int TURNLENGTH = 4;
const char *NOGAME = "NOGAME\n";
const int NOGAMELENGTH = 7;
const char *WIN = "WIN\n";
const int WINLENGTH = 4;
const char *TIE = "TIE\n";
const int TIELENGTH = 4;
const char *LOSE = "LOSE\n";
const int LOSELENGTH = 5;
const char *INVCMD = "INVCMD\n";
const char *INVARG = "INVARG\n";
const char *INVCOL = "INVCOL\n";
const char *INVFAU = "EFAULT\n";
const int INVLENGTH = 7;

bool read = false;
bool game_started = false;

static char *board[] = {
    "00000000\n",
    "00000000\n",
    "00000000\n",
    "00000000\n",
    "00000000\n",
    "00000000\n",
    "00000000\n",
    "00000000\n"
};

char input[8];
char *output = "";

static ssize_t four_read(struct file *file, char __user *buf, size_t count, loff_t *ppos);
static ssize_t four_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos);
static int __init fourinarow_init(void);
static void __exit fourinarow_exit(void);

static const struct file_operations four_fops = {
    .owner = THIS_MODULE,
    .read = four_read,
    .write = four_write
};

static struct miscdevice four_dev = {
    .minor = 0,
    .name = "fourinarow",
    .fops = &four_fops,
    .mode = 0666,
};

static ssize_t four_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    int ii = 0;
    int length = 0;
    
    /* If the read variable is set to true then that means this was already run and finish its*/
    if (read == true) {
        read = false;
        return 0;
    }

    /* Checks that the buffer is accessible*/
    if (!access_ok(buf, count)) {
        return -EFAULT;
    }return count;

    /* Change length depending on output*/
    if (OK == output) {
        length = OKLENGTH;
    } else if (INVCMD == output) {
        length = INVLENGTH;
    }

    /* Put the ouput into the buffer*/
    for (ii = 0; ii < length; ii++) {
        if (put_user(output[ii], buf + ii)) {
            return -EFAULT;
        }
    }

    /* Set to true so that we can stop the output*/
    read = true;

    return length;
}

static ssize_t four_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos) {
    int ii = 0;
    char input[9];
        
    /* Simple check for length of the command entered*/
    if (count > 8) {
        printk(KERN_ERR "Invalid length\n");
        output = (char *)INVCMD;
        return count;
    }

    /* Check to see if user pointer is valid*/
    if (!access_ok(buf, count)) {
        output = (char *)INVFAU;
        return count;
    }

    /* Put the ouput into the buffer*/
    for (ii = 0; ii < count; ii++) {
        if (get_user(input[ii], buf + ii)) {
            output = (char *)INVFAU;
            return count;
        }
    }
    input[ii + 1] = '\0';

    printk(KERN_INFO "Input string: %s", input);

    output = (char *)OK;
    return count;
}

static int __init fourinarow_init(void) {
    if (misc_register(&four_dev) < 0) {
        printk(KERN_ERR "Cannot acquire four in a row device\n");
        return -ENOENT;
    }

    read = false;
    game_started = false;

    printk(KERN_ALERT "Started the Four in a Row Machine\n");
    return 0;
}

static void __exit fourinarow_exit(void) {
    misc_deregister(&four_dev);
    printk(KERN_ALERT "Shutting down the Four in the Row Machine\n");
}

module_init(fourinarow_init);
module_exit(fourinarow_exit);