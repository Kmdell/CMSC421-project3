#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>

MODULE_LICENSE("GPL");

dev_t dev = 0;
static struct class *dev_class;

/*
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
*/

int my_uevent(struct device *dev, struct kobj_uevent_env *env)
{
    add_uevent_var(env, "DEVMODE=%#o", 0444);
    return 0;
}

static int __init inspiration_init(void) {
    if ((alloc_chrdev_region(&dev, 0, 1, "my_device_driver")) < 0) {
        printk(KERN_ERR "Cannot map major number for device\n");
        return -ENOENT;
    }
    printk(KERN_INFO "Allocated Character Device\n");

    if((dev_class = class_create(THIS_MODULE, "my_device_class")) < 0) {
        printk(KERN_ERR "Cannot create class for device\n");
        return -ENOENT;
    }

    dev_class->dev_uevent = my_uevent;

    if(device_create(dev_class, NULL, dev, NULL, "inspiration") < 0) {
        printk(KERN_ERR "Cannot create the device\n");
        return -ENOENT;
    }

    printk(KERN_ALERT "Started the Inspirational Quote Machine\n");
    return 0;
}

static void __exit inspiration_exit(void) {
    device_destroy(dev_class, dev);
    class_destroy(dev_class);
    unregister_chrdev_region(dev, 1);
    printk(KERN_ALERT "Shutting Down the Inspirational Quote Machine\n");
}

module_init(inspiration_init);
module_exit(inspiration_exit);