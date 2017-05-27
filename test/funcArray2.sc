//tic tac toe
EMPTY = 0;
PLAYER = 1;
COMPUTER = 2;
array board[3][3] = EMPTY;

def gg2(i, j, k) {
    @board[i][j] = k;
}

def gg(x[][], i, j, k) {
    x[i][j] = k;
}

def f(x[][]) {
    x[2][2] = 5;
    if (@board[2][2] == 5) {
        puts("correct!");
    }
    gg(x, 1, 1, 1);
    puti(x[1][1]);
    puti(@board[1][1]);
    gg(@board, 2, 2, 2);
    puti(x[2][2]);
    puti(@board[2][2]);
}

if (true) {
    f(board);
    puti(board[1][1]);
    puti(board[2][2]);
}

gg(board, 0, 0, 3);
puti(board[0][0]);