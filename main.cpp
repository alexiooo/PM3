#include <iostream>
#include <cstring>

using namespace std;

#define WORLD_SIZE 1000

// Random number generator
class RNG {
    static int random_number;

    public:

    static void set_seed(int seed) {
        random_number = seed;
    }

    static int get_random_number() {
        random_number = (221 * random_number + 1) % WORLD_SIZE;
        return random_number;
    }
};

class Life {
    
    const int VIEW_WIDTH = 80;
    const int VIEW_HEIGHT = 25;

    // Position of the view
    int view_x = 0;
    int view_y = 0;

    bool board[WORLD_SIZE][WORLD_SIZE] = { 0 };
    // Used to fill the actual board when computing
    // the next generation
    bool temp_board[WORLD_SIZE][WORLD_SIZE] = { 0 };

    // Parameters
    
    // The amount of random cells picked and made alive
    // when the board is filled randomly.
    int random_count = 0;
    
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
