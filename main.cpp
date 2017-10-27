#include <iostream>
#include <cstring>

using namespace std;

#define WORLD_SIZE 1000

class Life {
    
    const int VIEW_WIDTH = 80;
    const int VIEW_HEIGHT = 25;

    // Position of the view
    int view_x = 0;
    int view_y = 0;

    bool board[WORLD_SIZE][WORLD_SIZE] = { 0 };
    bool temp_board[WORLD_SIZE][WORLD_SIZE] = { 0 };

    
    bool isAlive(int x, int y) {
        // Edges are dead
        if(x < 0 || x > WORLD_SIZE - 1
            || y < 0 || y > WORLD_SIZE - 1) return false;
        
        return board[x][y];
    }

    int countLiveNeighbours(int x, int y) {
        return isAlive(x - 1, y - 1)
            + isAlive(x, y - 1)
            + isAlive(x + 1, y - 1)
            + isAlive(x - 1, y)
            + isAlive(x, y)
            + isAlive(x + 1, y)
            + isAlive(x - 1, y + 1)
            + isAlive(x, y + 1)
            + isAlive(x + 1, y + 1);
    }

    public:

    bool nextGeneration() {
        // Produce the next board according
        // to the rules of Game of Life
        for(int x = 0; x < WORLD_SIZE; x++)
        for(int y = 0; y < WORLD_SIZE; y++)
        {
            int count = countLiveNeighbours(x, y);

            if(isAlive(x, y)) {
                if(count == 2 || count == 3) temp_board[x][y] = true;
                else temp_board[x][y] = false;
            }
            else if(count == 3)
            {
                temp_board[x][y] = true;
            }
        }

        memcpy(board, temp_board, sizeof(bool) * WORLD_SIZE * WORLD_SIZE); 
    }

    void killAll() {
        for(int x = 0; x < WORLD_SIZE; x++)
        for(int y = 0; y < WORLD_SIZE; y++)
        {
            board[x][y] = false;
        }
    }

    void killView() {
        for(int x = view_x; x < view_x + VIEW_WIDTH; x++)
        for(int y = view_y; y < view_y + VIEW_HEIGHT; y++)
        {
            board[x][y] = false;
        }
    }

    void moveView(int x, int y) {
        view_x += x;
        view_y += y;
    }
};

// Reads a character from stdin, skipping leading Enters
char read_char () {
    char kar;
    while ( (kar = cin.get()) == '\n' ) {}
    return kar;
}

// Reads a number from stdin, stops interpreting when maxDigits has been reached
int read_num(int max_digits) {
    int num = 0;
    int maximum = max_digits * 10;

    char kar = read_char();
    while ( kar != '\n' ) {
        if ( num < maximum && '0' <= kar && kar <= 9 ){
            num *= 10;
            num += kar - '0';
        }
        kar = cin.get();
    }
    return num;
}

// Prints all screen elements
void draw() {
    cout << string(100, '-') << endl;
    //drawLife()
    cout << string (100, '-') << endl;
    cout << endl; // Toon paramaterwaarden
    cout << "(S)top (H)eelschoon S(c)hoon (P)arameters " << endl;
}


int main()
{
    return 0;
}
