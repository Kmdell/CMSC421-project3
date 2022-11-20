#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");

static const char *quotes[] = {
    "\"Out of the mountain of despair, a stone of hope.\" - Martin Luther King Jr.\n",
    "\"When you have a dream, you've got to grab it and never let go.\" - Carol Burnett",
    "\"Nothing is impossible. The word itself says 'I'm possible!\" - Audrey Hepburn",
    "\"There is nothing impossible to they who will try.\" - Alexander the Great",
    "\"The bad news is time flies. The good news is you're the pilot.\" - Michael Altshuler",
    "\"Life has got all those twists and turns. You've got to hold on tight and off you go.\" - Nicole Kidman",
    "\"\" - ",
    "\"\" - ",
    "\"\" - ",
    "\"\" - "
}

static int __init inspiration_init(void) {
    printk(KERN_INFO "Starting the Inspirational Quote Machine\n");
    return 0;
}

static void __exit inspiration_exit(void) {
    printk(KERN_ALERT "Shutting Down the Inspirational Quote Machine\n");
}

module_init(inspiration_init);
module_exit(inspiration_exit);