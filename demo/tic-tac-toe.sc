//tic tac toe
// Adopted minimax algorithm. You should expect that 
// the best result would be a draw. Guaranteed no win.
EMPTY = 0;
PLAYER = 1;
COMPUTER = 2;
array board[3][3] = EMPTY;

//    function definition starts with a def
def print() {
    for (i = 0; i != 3; ++i;) { // increment operation. Also decrement operation is supported. Support inline.
        for (j = 0; j != 3; ++j;) {
            puti_(@board[i][j]);
            puts_(" ");
        }
        puts("");
    }
}

def hasEmpty() {
    for (i = 0; i != 3; ++i;) {
        for (j = 0; j != 3; ++j;) {
            if (@board[i][j] == @EMPTY) {
                return true;
            }
        }
    }
    return false;
}

def canWin() {
    if (@board[0][0] == @board[1][1] && @board[1][1] == @board[2][2]) {
        if (@board[0][0] == @PLAYER) {
            return -1;
        }
        if (@board[0][0] == @COMPUTER){
            return 1;
        }
    }
    if (@board[2][0] == @board[1][1] && @board[1][1] == @board[0][2]) {
        if (@board[2][0] == @PLAYER) {
            return -1;
        } else if (@board[2][0] == @COMPUTER){
            return 1;
        }
    }
    for (i = 0; i != 3; ++i;) { // increment operation;
        if (@board[i][0] == @board[i][1] && @board[i][1] == @board[i][2]) {
            if (@board[i][0] == @COMPUTER) {
                return 1;
            }
            if (@board[i][0] == @PLAYER) {
                return -1;
            }
        }
    }
    for (i = 0; i != 3; ++i;) {
        if (@board[0][i] == @board[1][i] && @board[1][i] == @board[2][i]) {
            if (@board[0][i] == @COMPUTER) {
                return 1;
            }
            if (@board[0][i] == @PLAYER) {
                return -1;
            }
        }
    }
    return 0;
}

def minimax(isComputer) {
    temp = canWin();
    if (temp != 0) {
        return temp;
    }
    if (!hasEmpty()) { // negation operator. No guarantee on integer and other types.
        return 0;
    }
    if (isComputer) {
        max = -2;
        for (i = 0; i != 3; ++i;) {
            for (j = 0; j != 3; ++j;) {
                if (@board[i][j] == @EMPTY) {
                    @board[i][j] = @COMPUTER;
                    temp = minimax(false);
                    @board[i][j] = @EMPTY;
                    if (temp > max) {
                        max = temp;
                    }
                }
            }
        }
        return max;
    } else {
        min = 2;
        for (i = 0; i != 3; ++i;) {
            for (j = 0; j != 3; ++j;) {
                if (@board[i][j] == @EMPTY) {
                    @board[i][j] = @PLAYER;
                    temp = minimax(true);
                    @board[i][j] = @EMPTY;
                    if (min > temp) {
                        min = temp;
                    }
                }
            }
        }
        return min;
    }
}

def set(x[][3]) { // passing array into function requires subscription operator to indicate dimension.
    i = j = 0; // chained assignment;
    while (true) {
        puts_("Enter your input: ");
        input = geti(); // geti as a function that returns an integer.
        i = input / 3;
        j = input % 3;
        if (x[i][j] != @EMPTY || input < 0 || input >= 9) {
            puts("Cannot put your mark here!");
        } else {
            break;
        }
    }
    x[i][j] = @PLAYER;
}

def getChar(i, j) {
    if (@board[i][j] == @EMPTY) {
        return (i * 3 + j) + '0';
    }
    if (@board[i][j] == @PLAYER) {
        return 'O';
    }
    return 'X';
}

def paint() {
    bound = "=================";
    left = "|| ";
    mid = " || ";
    right = " ||";
    puts(bound);
    puts_(left);
    putc_(getChar(0, 0));
    puts_(mid);
    putc_(getChar(0, 1));
    puts_(mid);
    putc_(getChar(0, 2));
    puts(right);
    puts(bound);
    puts_(left);
    putc_(getChar(1, 0));
    puts_(mid);
    putc_(getChar(1, 1));
    puts_(mid);
    putc_(getChar(1, 2));
    puts(right);
    puts(bound);
    puts_(left);
    putc_(getChar(2, 0));
    puts_(mid);
    putc_(getChar(2, 1));
    puts_(mid);
    putc_(getChar(2, 2));
    puts(right);
    puts(bound);
}

def getMove() {
    best = -2;
    index = 0;
    for (i = 0; i != 3; ++i;) {
        for (j = 0; j != 3; ++j;) {
            if (@board[i][j] == @EMPTY) {
                @board[i][j] = @COMPUTER;
                value = minimax(false);
                @board[i][j] = @EMPTY;
                if (value > best) {
                    index = i * 3 + j;
                    best = value;
                }
            }
        }
    }
    return index;
}


def main() {
    paint();
    while (true) {
        set(@board);
        if (canWin() == -1) {
            puts("You win!!!");
            break;
        }
        if (!hasEmpty()) {
            puts("Draw!!!");
            break;
        }
        index = getMove();
        @board[index / 3][index % 3] = @COMPUTER;
        paint();
        if (canWin() == 1) {
            puts("You lose!!!");
            break;
        }
        if (!hasEmpty()) {
            puts("Draw!!!");
            break;
        }
    }
}

main();