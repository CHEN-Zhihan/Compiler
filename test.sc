//tic tac toe

EMPTY = 0;
PLAYER = 1;
COMPUTER = 2;
array board[3][3] = EMPTY;

def print() {
    for (i = 0; i != 3; ++i;) {
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
        } else if (@board[0][0] == @COMPUTER){
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
    puts("begin");
    print();
    temp = canWin();
    puti(temp);
    if (temp != 0) {
        return temp;
    }
    if (!hasEmpty()) {
        puts("no empty available");
        return 0;
    }
    if (isComputer) {
        max = -2;
        for (i = 0; i != 3; ++i;) {
            for (j = 0; j != 3; ++j;) {
                if (@board[i][j] == @EMPTY) {
                    @board[i][j] = @COMPUTER;
                    puts_("I am computer, putting at ");
                    puti(i * 3 + j);
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
                    puts_("I am player, putting at ");
                    puti(i * 3 + j);
                    temp = minimax(true);
                    @board[i][j] = @EMPTY;
                    if (min < temp) {
                        min = temp;
                    }
                }
            }
        }
        return min;
    }
}

minimax(true);