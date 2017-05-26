EMPTY = 0;
PLAYER = 1;
COMPUTER = 2;
array board[3][3] = EMPTY;
def minimax(isComputer) {
    if (isComputer) {
        max = -2;
        return max;
    } else {
        min = 2;
        return min;
    }
}

minimax(true);