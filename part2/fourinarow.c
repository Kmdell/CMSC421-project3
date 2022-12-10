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
const char ZERO = '0';

bool read = false;
bool write = false;
bool gameStarted = false;
bool playerTurn = false;

char gameBoard[8][9] = {
    {'0', '0', '0' ,'0', '0', '0', '0', '0', '\n'},
    {'0', '0', '0' ,'0', '0', '0', '0', '0', '\n'},
    {'0', '0', '0' ,'0', '0', '0', '0', '0', '\n'},
    {'0', '0', '0' ,'0', '0', '0', '0', '0', '\n'},
    {'0', '0', '0' ,'0', '0', '0', '0', '0', '\n'},
    {'0', '0', '0' ,'0', '0', '0', '0', '0', '\n'},
    {'0', '0', '0' ,'0', '0', '0', '0', '0', '\n'},
    {'0', '0', '0' ,'0', '0', '0', '0', '0', '\n'}
};
char player = ' ';

char *output = "";
int outputLength = 0;

static ssize_t four_read(struct file *file, char __user *buf, size_t count, loff_t *ppos);
static ssize_t four_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos);
static void resetBoard(void);
static void computerTurn(void);
static void dropPiece(int col);
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

    /* Resets the board*/
    printk(KERN_ALERT "Reseting the Game gameBoard\n");
    for (ii = 0; ii < 8; ii++) {
        for (jj = 0; jj < 8; jj++) {
            gameBoard[ii][jj] = ZERO;
        }
    }

    /* if the player decides to be Red then the computer goes*/
    if (player == 'R') {
        playerTurn = false;
        computerTurn();
    } else {
        playerTurn = true;
    }

    gameStarted = true;
    output = (char *)OK;
    outputLength = OKLENGTH;
}

static void computerTurn() {
    int index = 0;
    bool placed = false;

    printk(KERN_ALERT "Computer taking its turn\n");

    while (placed == false) {
        /* get random placement for computer marker*/
        get_random_bytes(&index, sizeof(index));

        if (index < 0) {
            index *= -1;
        }
        
        index %= 8;

        /* try to put it in that row if not available then try again until you can find something*/
        dropPiece(index);

        if (output == OK) {
            placed = true;
        }
    }
}

static void dropPiece(int col) {
    int ii = 0;

    /* sets output to invalid column if nothing is found*/
    output = (char *)INVCOL;
    outputLength = INVLENGTH;

    /* check to see if the */
    while (ii < 8) {
        if (gameBoard[ii][col] == ZERO) {
            /* if a place is found we drop the piece*/
            if (player == 'R') {
                gameBoard[ii][col] = (playerTurn ? 'R' : 'Y');
            } else {
                gameBoard[ii][col] = (playerTurn ? 'Y' : 'R');
            }

            ii = 8;
            playerTurn = !playerTurn;
            output = (char*)OK;
            outputLength = OKLENGTH;
        }

        ii++;
    }
}

static void checkWinCondition() {
    int ii = 0, jj = 0, kk = 0;
    char winner = ' ';
    bool tied = true;
    int row = 0;
    printk(KERN_INFO "Checking win condition");

    /* TODO: Building win condition checking system*/
    for (ii = 0; ii < 8; ii++) {
        for (jj = 0; jj < 5; jj++) {
            if (gameBoard[ii][jj] != '0') {
                /* check to the left*/
                row = 0;
                for (kk = 0; ii - kk >= 0 && kk < 4; kk++) {
                    if (gameBoard[ii][jj] == gameBoard[ii - kk][jj]) {
                        row++;
                    }
                }
                if (row == 4) {
                    printk(KERN_INFO "Winner found and set\n");
                    winner = gameBoard[ii][jj];
                }

                row = 0;
                /* check to the left and down*/
                for (kk = 0; ii - kk >= 0 && kk < 4; kk++) {
                    if (gameBoard[ii][jj] == gameBoard[ii - kk][jj + kk]) {
                        row++;
                    }
                }
                if (row == 4) {
                    printk(KERN_INFO "Winner found and set\n");
                    winner = gameBoard[ii][jj];
                }

                row = 0;
                /* check downwards*/
                for (kk = 0; kk < 4; kk++) {
                    if (gameBoard[ii][jj] == gameBoard[ii][jj + kk]) {
                        row++;
                    }
                }
                if (row == 4) {
                    printk(KERN_INFO "Winner found and set\n");
                    winner = gameBoard[ii][jj];
                }

                row = 0;
                /* check down to the right*/
                for (kk = 0; ii + kk < 8 && kk < 4; kk++) {
                    if (gameBoard[ii][jj] == gameBoard[ii + kk][jj + kk]) {
                        row++;
                    }
                }
                if (row == 4) {
                    printk(KERN_INFO "Winner found and set\n");
                    winner = gameBoard[ii][jj];
                }

                row = 0;
                /* check to the right*/
                for (kk = 0; ii + kk < 8 && kk < 4; kk++) {
                    if (gameBoard[ii][jj] == gameBoard[ii + kk][jj]) {
                        row++;
                    }
                }
                if (row == 4) {
                    printk(KERN_INFO "Winner found and set\n");
                    winner = gameBoard[ii][jj];
                }
            }
        }
    }

    /* check to see if there is a winner*/
    if (winner != ' ') {
        if (winner == player) {
            output = (char *)WIN;
            outputLength = WINLENGTH;
            gameStarted = false;
            return;
        } else {
            output = (char *)LOSE;
            outputLength = LOSELENGTH;
            gameStarted = false;
            return;
        }
    }

    /* check for a tie condition*/
    for (ii = 0; ii < 8; ii++) {
        if (gameBoard[7][ii] == ZERO) {
            tied = false;
        }
    }

    /* check to see if the bottom row is full so call it tied*/
    if (tied == true) {
        output = (char *)TIE;
        outputLength = TIELENGTH;
        gameStarted = false;
        return;
    }
}

static ssize_t four_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    int ii = 0, jj = 0, index = 0;
    char outputbuf[72];
    
    /* If the read variable is set to true then that means this was already run and finish its*/
    if (read == true) {
        read = false;
        return 0;
    }

    /* Checks that the buffer is accessible*/
    if (!access_ok(buf, outputLength)) {
        printk(KERN_ERR "Could not access buffer for the reading\n");
        return -EFAULT;
    }

    /* print the board out other wise print out the other outputs*/
    if (output == *gameBoard) {
        for (ii = 0; ii < 8; ii++) {
            for (jj = 0; jj < 9; jj++) {
                outputbuf[index] = gameBoard[ii][jj];
                index++;
            }
        }

        /* Put the ouput into the buffer*/
        for (ii = 0; ii < outputLength; ii++) {
            if (put_user(outputbuf[ii], buf + ii)) {
                printk(KERN_ERR "Failed to write to the read buffer\n");
                return -EFAULT;
            }
        }
    } else {
        /* Put the ouput into the buffer*/
        for (ii = 0; ii < outputLength; ii++) {
            if (put_user(output[ii], buf + ii)) {
                printk(KERN_ERR "Failed to write to the read buffer\n");
                return -EFAULT;
            }
        }
    }

    /* Set to true so that we can stop the output*/
    read = true;
    return outputLength;
}

static ssize_t four_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos) {
    int ii = 0;
    char input[9];
    /* Handle newline*/
    printk("Count: %ld\n", count);
    if (write == true && count == 1) {
        printk(KERN_INFO "Handled the newline\n");
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
        printk(KERN_ERR "FAILED to access buffer\n");
        output = (char *)INVFAU;
        outputLength = INVLENGTH;
        return count;
    }

    /* Put the ouput into the buffer*/
    for (ii = 0; ii < count; ii++) {
        if (get_user(input[ii], buf + ii)) {
            printk(KERN_ERR "FAILED to read the the data from the userspace pointer\n");
            output = (char *)INVFAU;
            outputLength = INVLENGTH;
            return count;
        }
    }

    /* set write to true so that we know we grabbed a command*/
    write = true;
    input[ii] = '\0';

    printk(KERN_INFO "Input string: %s", input);

    /* check for the reset command*/
    if (strncmp(input, "RESET", 5) == 0) {
        /* Invaldi command was given*/
        if (input[5] != ' ') {
            printk(KERN_ERR "Invalid command\n");
            output = (char *)INVCMD;
            outputLength = INVLENGTH;
            return count;
        }

        /* Input validation for the color wanted by player*/
        if (input[6] != 'Y' && input[6] != 'R') {
            printk(KERN_ERR "Invalid argument for RESET\n");
            output = (char *)INVARG;
            outputLength = INVLENGTH;
            return count;
        }

        /* set the player marker to R and reset the board*/
        printk(KERN_INFO "RESET command given\n");
        player = input[6];
        resetBoard();
        return count;
    }

    /* check for the gameBoard command*/
    if (strncmp(input, "BOARD\n", 6) == 0) {
        printk(KERN_INFO "BOARD command given\n");
        output = *gameBoard;
        outputLength = 72;
        return count;
    }

    /* check for the drop piece command*/
    if (strncmp(input, "DROPC", 5) == 0) {
        /* print invalid command if the */
        if (input[5] != ' ') {
            printk(KERN_ERR "Invalid command given\n");
            output = (char *)INVCMD;
            outputLength = INVLENGTH;
            return count;
        }

        /* Do input valid for the columns*/
        if (input[6] < 'A' || input[6] > 'H') {
            printk(KERN_ERR "Invalid argument given for DROPC\n");
            output = (char *)INVARG;
            outputLength = INVLENGTH;
            return count;
        }

        /* make sure that the game is started*/
        if (gameStarted == false) {
            printk(KERN_ERR "No game is in progress\n");
            output = (char *)NOGAME;
            outputLength = NOGAMELENGTH;
            return count;
        }

        /* if not player turn then print error*/
        if (playerTurn == false) {
            printk(KERN_ERR "Not the players turn\n");
            output = (char *)OOT;
            outputLength = OOTLENGTH;
            return count;
        }

        /* Dropping the piece in the column*/
        printk(KERN_INFO "DROPC command was given\n");
        dropPiece((int)(input[6] - 'A'));
        checkWinCondition();
        return count;
    }

    /* check the computer turn command*/
    if (strncmp(input, "CTURN\n", 6) == 0) {
        /* if the game isnt started then output NOGAME*/
        if (gameStarted == false) {
            printk(KERN_ERR "No game is in progress\n");
            output = (char *)NOGAME;
            outputLength = NOGAMELENGTH;
            return count;
        }
        
        /* if its the player turn then output error*/
        if (playerTurn == true) {
            printk(KERN_ERR "Not the computers turn\n");
            output = (char *)OOT;
            outputLength = OOTLENGTH;
            return count;
        }

        /* computer takes its turn*/
        printk(KERN_INFO "Computer takes turn\n");
        computerTurn();
        checkWinCondition();
        return count;
    }

    output = (char *)INVCMD;
    outputLength = INVLENGTH;
    return count;
}

static int __init fourinarow_init(void) {
    /* Started the new device*/
    if (misc_register(&four_dev) < 0) {
        printk(KERN_ERR "Cannot acquire four in a row device\n");
        return -ENOENT;
    }
    /* Initializing variables that are necessary for the program*/
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