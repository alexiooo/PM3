#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>


using namespace std;

#define WORLD_SIZE 1000

// Random number generator
class RNG {
    private:
    static int random_number;

    public:

    static void set_seed(int seed) {
        RNG::random_number = seed;
    }

    static int get_random_number() {
        RNG::random_number = (221 * RNG::random_number + 1) % WORLD_SIZE;
        return RNG::random_number;
    }
};

// Define storage for variable
int RNG::random_number;

class Life {
    
    const int VIEW_WIDTH = 80;
    const int VIEW_HEIGHT = 25;

    // Position of the view
    int view_x = 0;
    int view_y = 0;

    // Position of the cursor relative
    // to the top left of the view
    int cursor_x = 0;
    int cursor_y = 0;

    bool board[WORLD_SIZE][WORLD_SIZE] = { 0 };
    // Used to fill the actual board when computing
    // the next generation
    bool temp_board[WORLD_SIZE][WORLD_SIZE] = { 0 };

    // Parameters
    
    // The amount of random cells picked and made alive
    // when the board is filled randomly.
    int random_count = 0;
    
    static bool isValidPosition(int x, int y) {
        return x >= 0 && x < WORLD_SIZE
            && y >= 0 && y < WORLD_SIZE;
    }

    bool isAlive(int x, int y) {
        // Edges are dead by default
        return Life::isValidPosition(x, y) && board[x][y];
    }

    int countLiveNeighbours(int x, int y) {
        return isAlive(x - 1, y - 1)
            + isAlive(x, y - 1)
            + isAlive(x + 1, y - 1)
            + isAlive(x - 1, y)
            + isAlive(x + 1, y)
            + isAlive(x - 1, y + 1)
            + isAlive(x, y + 1)
            + isAlive(x + 1, y + 1);
    }

    public:

    void nextGeneration() {
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

    void moveCursor(int x, int y) {
        cursor_x += x;
        cursor_y += y;
    }

    void toggleCursor() {
        if(Life::isValidPosition(cursor_x, cursor_y)) {
            board[cursor_x][cursor_y] = !board[cursor_x][cursor_y];
        }
    }

    void makeRandomAlive() {

        // Clear board
        killAll();

        // Then fill cells at random
        for(int i = 0; i < random_count; i++)
        {
            int x = RNG::get_random_number();
            int y = RNG::get_random_number();

            board[x][y] = true;
        }
    }

    void printView() {
        
        for(int y = view_y; y < view_y + VIEW_HEIGHT; y++)
        {
            for(int x = view_x; x < view_x + VIEW_WIDTH; x++)
            {
                if(board[x][y]) cout << "x";
                else cout << " ";
            }

            cout << endl;
        }
    }

    void fillViewFromFile(ifstream& file)
    {
        killAll();

        int symbol;

        int x = view_x;
        int y = view_y;

        while((symbol = file.get()) != EOF)
        {
            if(symbol == '\n')
            {
                x = view_x;
                y += 1;    
            }
            else
            {
                if(Life::isValidPosition(x, y))
                {
                    if(symbol == 'x') board[x][y] = true;
                    else board[x][y] = false;
                }

                x += 1;
            }
        }
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
    ifstream file("glidergun.txt");

    Life* game = new Life();

    game->fillViewFromFile(file);

    for(;;) {
        game->printView(); 
        usleep(1);
        game->nextGeneration();
    }

    return 0;
}
