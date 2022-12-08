#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/string.h>
#include <linux/random.h>

MODULE_LICENSE("GPL");

/* The constants of strings that we want to be able to edit*/
const char *OK = "OK\n";
const int OKLENGTH = 3;
const char *OOT = "OOT\n";
const int OOTLENGTH = 4;
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
bool write = false;
bool gameStarted = false;

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
char player = ' ';

char *output = "";
int outputLength = 0;

static ssize_t four_read(struct file *file, char __user *buf, size_t count, loff_t *ppos);
static ssize_t four_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos);
static void resetBoard(void);
static void computerTurn(char marker);
static void dropPiece(char column);
static void checkWinCondition(void);
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

static void resetBoard() {
    int ii = 0, jj = 0;
    gameStarted = true;
    for (ii = 0; ii < 8; ii++) {
        for (jj = 0; jj < 8; jj++) {
            board[ii][jj] = 0;
        }
    }
    if (player == 'R') {
        computerTurn('Y');
    }
}

static void computerTurn(char marker) {
    int index = 0;
    int ii = 0;

    /* get random placement for computer marker*/
    get_random_bytes(&index, sizeof(index));
    if (index < 0) {
        index *= -1;
    }
    index %= 8;

    /* try to put it in that row if not available then try again until you can find something*/
    while (ii < 8) {
        if (board[index][ii] != '0') {
            get_random_bytes(&index, sizeof(index));
            if (index < 0) {
                index *= -1;
            }
            index %= 8;
            ii++;
        } else {
            board[index][ii] = marker;
            ii = 8;
        }
    }
}

static void dropPiece(char col) {
    int ii = 0;
    int column = (int)(col - 'A');

    /* sets output to invalid column if nothing is found*/
    output = (char *)INVCOL;
    outputLength = INVLENGTH;

    /* check to see if the */
    while (ii < 8) {
        if (board[column][ii] != '0') {
            ii++;
        } else {
            /* if a place is found we drop the piece*/
            if (player == 'R') {
                board[column][ii] = 'Y';
            } else {
                board[column][ii] = 'R';
            }
            output = (char*)OK;
            outputLength = OKLENGTH;
        }
    }
}

static void checkWinCondition() {
    printk(KERN_INFO "Checking win condition");
}

static ssize_t four_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    int ii = 0;
    printk(KERN_ALERT "Initialized the variables\n");
    
    /* If the read variable is set to true then that means this was already run and finish its*/
    if (read == true) {
        read = false;
        return 0;
    }

    printk(KERN_ALERT "Checked read\n");

    /* Checks that the buffer is accessible*/
    if (!access_ok(buf, count)) {
        printk(KERN_ERR "Could not access buffer for the reading\n");
        return -EFAULT;
    }

    printk(KERN_ALERT "Checked pointer access\n");

    /* Put the ouput into the buffer*/
    for (ii = 0; ii < outputLength; ii++) {
        if (put_user(output[ii], buf + ii)) {
            printk(KERN_ERR "Failed to write to the read buffer\n");
            return -EFAULT;
        }
    }

    printk(KERN_ALERT "Wrote out to the console\n");

    /* Set to true so that we can stop the output*/
    read = true;
    return outputLength;
}

static ssize_t four_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos) {
    int ii = 0;
    char input[9];
    /* Handle newline*/
    printk("Count: %ld", count);
    if (write == true && count == 1) {
        write = false;
        return count;
    }
        
    /* Simple check for length of the command entered*/
    if (count > 8 || count < 6) {
        printk(KERN_ERR "Invalid length\n");
        output = (char *)INVCMD;
        outputLength = INVLENGTH;
        return count;
    }

    /* Check to see if user pointer is valid*/
    if (!access_ok(buf, count)) {
        output = (char *)INVFAU;
        outputLength = INVLENGTH;
        return count;
    }

    /* Put the ouput into the buffer*/
    for (ii = 0; ii < count; ii++) {
        if (get_user(input[ii], buf + ii)) {
            output = (char *)INVFAU;
            outputLength = INVLENGTH;
            return count;
        }
    }

    /* set write to true so that we know we grabbed a command*/
    write = true;
    input[ii] = '\0';

    /* check for the reset command*/
    if (strncmp(input, "RESET", 5)) {
        if (input[5] == ' ') {
            if (input[6] == 'Y' || input[6] == 'R') {
                player = input[6];
                resetBoard();
                return count;
            } else {
                output = (char *)INVARG;
                outputLength = INVLENGTH;
                return count;
            }
        } else {
            output = (char *)INVCMD;
            outputLength = INVLENGTH;
            return count;
        }
    }

    /* check for the board command*/
    if (strncmp(input, "BOARD\n", 6)) {
        output = *board;
        return count;
    }

    /* check for the drop piece command*/
    if (strncmp(input, "DROPC", 5)) {
        if (input[5] == ' ') {
            if (input[6] <= 'A' || input[6] >= 'H') {
                if (gameStarted == false) {
                    output = (char *)NOGAME;
                    outputLength = NOGAMELENGTH;
                    return count;
                }
                dropPiece(input[6]);
                checkWinCondition();
                return count;
            } else {
                output = (char *)INVARG;
                outputLength = INVLENGTH;
                return count;
            }
        } else {
            output = (char *)INVCMD;
            outputLength = INVLENGTH;
            return count;
        }
    }

    /* check the computer turn command*/
    if (strncmp(input, "CTURN\n", 6)) {
        if (gameStarted == false) {
            output = (char *)NOGAME;
            outputLength = NOGAMELENGTH;
            return count;
        }
        if (player == 'R') {
            computerTurn('Y');
        } else {
            computerTurn('R');
        }
        checkWinCondition();
    }

    printk(KERN_INFO "Input string: %s", input);

    output = (char *)OK;
    outputLength = OKLENGTH;
    return count;
}

static int __init fourinarow_init(void) {
    if (misc_register(&four_dev) < 0) {
        printk(KERN_ERR "Cannot acquire four in a row device\n");
        return -ENOENT;
    }

    read = false;
    write = false;
    gameStarted = false;

    printk(KERN_ALERT "Started the Four in a Row Machine\n");
    return 0;
}

static void __exit fourinarow_exit(void) {
    misc_deregister(&four_dev);
    printk(KERN_ALERT "Shutting down the Four in the Row Machine\n");
}

module_init(fourinarow_init);
module_exit(fourinarow_exit);