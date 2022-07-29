#include <iostream>
#include <ctime>
#include <ncurses.h>
#include <math.h>

#define UP 122 // Z
#define DOWN 115 // S
#define LEFT 113 // Q
#define RIGHT 100 // D

int generate_number() {
    srand(time(NULL));
    return (rand() % 10 > 6 ? 4 : 2);
}
// display prettyfier: each box has the same width
std::string fill(int x, int i) {
    int n = i - (int)(log10(x+1) + 1);
    std::string s = "";

    for (int i = 0; i < n; i++) {
    s+= " ";
    }
    return s;
}

bool equal_grid(int a[4][4], int b[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (a[i][j] != b[i][j]) return false;
        }
    }
    return true;
}

class Game {
    private:
        int board[4][4]; // 4x4 game board
        
        // returns the highest square value on the board
        int max_box() { 
            int m = 0;
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    if (board[i][j] > m) m = board[i][j];
                }
            }
            return m;
        }

        void add_number() {
            srand(time(NULL));
            while(true) {
                int x = rand() % 4;
                int y = rand() % 4;

                if (board[x][y] == 0) {
                    board[x][y] = generate_number();
                    return;
                }
            }
        }
        // # GRID MOVEMENT #
        void slide_left(int row[4]) {
            int arr[4] = {0,0,0,0};
            int pos = 0;
            for (int i = 0; i < 4; i++) {
                if (row[i] > 0) {
                    arr[pos] = row[i];
                    pos++;
                }
            }
            for (int i = 0; i < 4; i++) {
                row[i] = arr[i];
            }
        }

        void merge_left(int row[4]) {
            for (int i = 0; i < 3; i++) {
                if (row[i] == row[i+1]) {
                    row[i] *= 2;
                    row[i+1] = 0;
                }
            }
        }

        // slides and merges a row to the left
        void operate(int row[4]) {
            slide_left(row);
            merge_left(row);
            slide_left(row);
        }
        
        void move_left() {
            for (int i = 0; i < 4; i++) {
                operate(board[i]);
            }
        }

        void reverse_row(int row[4]) {
            int arr[4] = {0,0,0,0};
            for (int i = 0; i < 4; i++) {
                arr[3-i] = row[i];
            }
            for (int i = 0; i < 4; i++) {
                row[i] = arr[i];
            }
        }

        void reverse_board() {
            for (int i = 0; i < 4; i++) {
                reverse_row(board[i]);
            }
        }

        void flip_board() {
            int grid[4][4];
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    grid[i][j] = board[j][i];
                }
            }
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    board[i][j] = grid[i][j];
                }
            }
        }

        bool is_move_possible(int code) {
            int copy[4][4];
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    copy[i][j] = board[i][j];
                }
            }
            
            move(code); 
            
            // resets board
            if (!equal_grid(board, copy)) {
                for (int i = 0; i < 4; i++) {
                    for (int j = 0; j < 4; j++) {
                        board[i][j] = copy[i][j];
                    }
                }

                return true;
            }
            return false;
        }
        
    public:
        // sets each square to 0
        void init_board() {
            for (int i = 0; i < 4/*sizeof(board)*/; i++) {
                for (int j = 0; j < 4/*sizeof(board)*/; j++) {
                    board[i][j] = 0;
                }
            }
            add_number();
            add_number();
        }
        
        // displays the board in a 4x4 grid
        void display() {
            int space = (int)(log10(max_box())) +1;
            for (int i = 0; i < 4/*sizeof(board)*/; i++) {
                for (int j = 0; j < 4/*sizeof(board)*/; j++) {
                    printw("%d %s", board[i][j], fill(board[i][j], space).data());
                }
                printw("\n");
            }
            printw("\n");
        }
        
        // returns true if a box's value exceeds 2048
        bool is_won() {
            return max_box() >= 2048;
        }

        bool is_lost() {
            return !(is_move_possible(LEFT) || is_move_possible(RIGHT)
                || is_move_possible(UP) || is_move_possible(DOWN));
        }

        void move(int code) {
            switch(code) {
                case LEFT:
                    move_left();
                    break;
                case RIGHT:
                    reverse_board();
                    move_left();
                    reverse_board(); 
                    break;
                case UP:
                    flip_board();
                    move_left();
                    flip_board();
                    break;
                case DOWN:
                    flip_board();
                    move(RIGHT);
                    flip_board();
                    break;
                default:
                    std::cout << "not a valid key" << std::endl;
            }
        }

        void play(int code) {
            if (is_move_possible(code)) {
                move(code);
                add_number();
            }
        }
};

int main() {
    Game game;
    game.init_board();

    initscr();
    game.display();

    while(!game.is_won() && !game.is_lost()) {
        game.play(getch());
        clear();
        game.display();
        refresh();
    }

    endwin();
}
