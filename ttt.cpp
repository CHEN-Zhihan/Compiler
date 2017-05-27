#include <iostream>
#include <string>
using std::string;
using std::cout;
using std::cin;
using std::endl;

int board[3][3];
constexpr int EMPTY = 0;
constexpr int PLAYER = 1;
constexpr int COMPUTER = 2;
int move = 0;
void print() {
    for (int i = 0; i != 3; ++i) {
        for (int j = 0; j != 3; ++j) {
            cout << board[i][j] << " ";
        }
        cout << endl;
    }
}

bool hasEmpty() {
    for (int i = 0; i != 3; ++i) {
        for (int j = 0; j != 3; ++j) {
            if (board[i][j] == EMPTY) {
                return true;
            }
        }
    }
    return false;
}

int canWin() {
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2]) {
        if (board[0][0] == PLAYER) {
            return -1;
        }
        if (board[0][0] == COMPUTER) {
            return 1;
        }
    }
    if (board[2][0] == board[1][1] && board[1][1] == board[0][2]) {
        if (board[2][0] == PLAYER) {
            return -1;
        }
        if (board[2][0] == COMPUTER) {
            return 1;
        }
    }
    for (int i = 0; i != 3; ++i) { // increment operation;
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2]) {
            if (board[i][0] == COMPUTER) {
                return 1;
            }
            if (board[i][0] == PLAYER) {
                return -1;
            }
        }
    }
    for (int i = 0; i != 3; ++i) {
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i]) {
            if (board[0][i] == COMPUTER) {
                return 1;
            }
            if (board[0][i] == PLAYER) {
                return -1;
            }
        }
    }
    return 0;
}
int minimax(bool isComputer) {
    int temp = canWin();
    if (temp != 0) {
        return temp;
    }
    if (!hasEmpty()) {
        return 0;
    }
    if (isComputer) {
        int max = -2;
        for (int i = 0; i != 3; ++i) {
            for (int j = 0; j != 3; ++j) {
                if (board[i][j] == EMPTY) {
                    board[i][j] = COMPUTER;
                    temp = minimax(false);
                    board[i][j] = EMPTY;
                    if (temp > max) {
                        max = temp;
                        move = i * 3 + j;                        
                    }
                }
            }
        }
        return max;
    } else {
        int min = 2;
        for (int i = 0; i != 3; ++i) {
            for (int j = 0; j != 3; ++j) {
                if (board[i][j] == EMPTY) {
                    board[i][j] = PLAYER;
                    temp = minimax(true);
                    board[i][j] = EMPTY;
                    if (min < temp) {
                        min = temp;
                        move = i * 3 + j;
                    }
                }
            }
        }
        return min;
    }
}

void set() {
    int i = 0, j = 0;
    while (true) {
        int input;
        cin >> input;
        i = input / 3;
        j = input % 3;
        if (board[i][j] != EMPTY) {
            cout << "Cannot put your mark here!" << endl;
        } else {
            break;
        }
    }
    board[i][j] = PLAYER;
}

char getChar(int i, int j) {
    if (board[i][j] == EMPTY) {
        return (i * 3 + j) + '0';
    }
    if (board[i][j] == PLAYER) {
        return 'O';
    }
    return'X';
}

void paint() {
    string bound = "=================";
    string left = "|| ";
    string mid = " || ";
    string right = " ||";
    cout << bound << endl;
    cout << left << getChar(0, 0) << mid << getChar(0, 1) << mid << getChar(0, 2) << right << endl;
    cout << bound << endl;
    cout << left << getChar(1, 0) << mid << getChar(1, 1) << mid << getChar(1, 2) << right << endl;
    cout << bound << endl;
    cout << left << getChar(2, 0) << mid << getChar(2, 1) << mid << getChar(2, 2) << right << endl;
    cout << bound << endl;        
}

int main(int argc, const char * argv[]) {
    paint();
    while (true) {
        set();
        paint();
        if (canWin() == -1) {
            cout << "You win!" << endl;
            break;
        }
        if (!hasEmpty()) {
            cout << "Draw!" << endl;
            break;
        }
        minimax(true);
        board[move / 3][move % 3] = COMPUTER;
        paint();
        if (canWin() == 1) {
            cout << "You lose!" << endl;
            break;
        }
        if (!hasEmpty()) {
            cout << "Draw!" << endl;
            break;
        }
    }
}