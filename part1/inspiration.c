#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/random.h>

MODULE_LICENSE("GPL");

static ssize_t ins_read(struct file *file, char __user *buf, size_t count, loff_t *ppos);
static int __init inspiration_init(void);
static void __exit inspiration_exit(void);

dev_t dev = 0;
static const struct file_operations ins_fops = {
    .owner = THIS_MODULE,
    .read = ins_read,
};

static struct miscdevice ins_dev = {
    .minor = 0,
    .name = "inspiration",
    .fops = &ins_fops,
    .mode = 0444,
};

static const char *quotes[] = {
    "\"Out of the mountain of despair, a stone of hope.\" - Martin Luther King Jr.\n",
    "\"When you have a dream, you've got to grab it and never let go.\" - Carol Burnett\n",
    "\"Nothing is impossible. The word itself says 'I'm possible!\" - Audrey Hepburn\n",
    "\"There is nothing impossible to they who will try.\" - Alexander the Great\n",
    "\"The bad news is time flies. The good news is you're the pilot.\" - Michael Altshuler\n",
    "\"Life has got all those twists and turns. You've got to hold on tight and off you go.\" - Nicole Kidman\n",
    "\"Be courageous. Challenge orthodoxy. Stand up for what you believe in. When you are in your rocking chair talking to your grandchildren many years from now, be sure you have a good story to tell.\" - Amal Clooney\n",
    "\"You make a choice: continue living your life feeling muddled in this abyss of self-misunderstanding, or you find your identity independent of it. You draw your own box.\" - Duchess Meghan\n",
    "\"I just want you to know that if you are out there and you are being really hard on yourself right now for something that has happened ... it's normal. That is what is going to happen to you in life. No one gets through unscathed. We are all going to have a few scratches on us. Please be kind to yourselves and stand up for yourself, please.\" - Taylor Swift\n",
    "\"Success is not final, failure is not fatal: it is the courage to continue that counts.\" - Winston Churchill\n"
};

static const int quoteLen[] = {
    77,
    82,
    79,
    75,
    86,
    104,
    213,
    189,
    360,
    110
};

static ssize_t ins_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    int index = 0;
    int i = 0;
    if (*ppos > 0) {
        return 0;
    }
    get_random_bytes(&index, sizeof(index));
    if (index < 0) {
        index *= -1;
    }
    index = index % 10;

    printk(KERN_INFO "Index of the randomly selected message: %d\n", index);
    printk(KERN_INFO "The message selected: %s", quotes[index]);
    printk(KERN_INFO "The length of the message: %d\n", quoteLen[index]);
    
    if (!access_ok(buf, count)) {
        return -EFAULT;
    }

    for (i = 0; i < quoteLen[index]; i++) {
        if(put_user(quotes[index][i], buf + i)) {
            return -EFAULT;
        }
    }

    *ppos += quoteLen;

    return quoteLen[index];
}

static int __init inspiration_init(void) {
    if (misc_register(&ins_dev) < 0) {
        printk(KERN_ERR "Cannot acquire inspiration device\n");
        return -ENOENT;
    }

    printk(KERN_ALERT "Started the Inspirational Quote Machine\n");
    return 0;
}

static void __exit inspiration_exit(void) {
    misc_deregister(&ins_dev);
    printk(KERN_ALERT "Shutting Down the Inspirational Quote Machine\n");
}

module_init(inspiration_init);
module_exit(inspiration_exit);