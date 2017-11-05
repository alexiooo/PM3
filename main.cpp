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
    
    static bool positionWithinWorld(int x, int y) {
        return x >= 0 && x < WORLD_SIZE
            && y >= 0 && y < WORLD_SIZE;
    }
 
    // Whether the position is on the edge of the world,
    // the edge of the world is one character wide
    static bool positionOnEdge(int x, int y) {
        return ((x == -1 || x == WORLD_SIZE) && y >= -1 && y <= WORLD_SIZE) // Vertical edges
            || ((y == -1 || y == WORLD_SIZE) && x >= -1 && x <= WORLD_SIZE); // Horizontal edges

    }

    bool positionWithinView(int x, int y) {
        return x >= view_x && x <= view_x + VIEW_WIDTH
            && y >= view_y && y <= view_y + VIEW_HEIGHT;
    }

    bool isAlive(int x, int y) {
        // Edges are dead by default
        return Life::positionWithinWorld(x, y) && board[x][y];
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

    void resetCursor() {
        cursor_x = view_x;
        cursor_y = view_y;
    }

    void moveCursor(int x, int y) {
        int new_x = cursor_x + x;
        int new_y = cursor_y + y;

        if(positionWithinView(new_x, new_y))
        {
            cursor_x = new_x;
            cursor_y = new_y;
        }
    }

    void toggleCursor() {
        if(Life::positionWithinWorld(cursor_x, cursor_y)) {
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

    void printView(bool print_cursor) {
        
        if(print_cursor)
        {
            for(int x = view_x; x < cursor_x && x < VIEW_WIDTH; x++)
            {
                cout << " "; 
            }
            cout << " |" << endl;
        }


        for(int y = view_y; y < view_y + VIEW_HEIGHT; y++)
        {
            if(print_cursor)
            {
                if(y == cursor_y) cout << "-";
                else cout << " ";
            }

            for(int x = view_x; x < view_x + VIEW_WIDTH; x++)
            {
                if(Life::positionWithinWorld(x, y) && board[x][y]) cout << "x";
                else if(Life::positionOnEdge(x, y)) cout << "#";
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
                if(Life::positionWithinWorld(x, y))
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

void print_menu(bool using_cursor)
{
    if(using_cursor)
    {
        cout << "Use w,a,s,d to move cursor around" << endl;
        cout << "Press space to toggle cell" << endl;
        cout << "Press b to return to menu" << endl;
    }
    else
    {
        cout << "1. Exit ";
        cout << "2. Clean world ";
        cout << "3. Clean view ";
        cout << "4. Change parameters ";
        cout << "5. Random ";
        cout << "6. Toggle using cursor ";
        cout << "7. Load glidergun.txt ";
        cout << "8. Compute one generation ";
        cout << "9. Run Game of Life " << endl;
        cout << "Use w,a,s,d to move view around" << endl;
    }
}

void move_cursor_or_view(Life *game, bool use_cursor, int x, int y)
{
    if(use_cursor)
    {
        game->moveCursor(x, y); 
    }
    else
    {
        game->moveView(x, y);
    }
}

int main()
{
    ifstream gliderGun("glidergun.txt");

    Life *game = new Life();
    bool using_cursor = false;

    game->printView(using_cursor);
    print_menu(using_cursor);

    for(;;) {
        char input = cin.get();
        
        switch(input)
        {
            case '\n':
                game->printView(using_cursor);
                print_menu(using_cursor);
                break;

            // Movements
            case 'w':
                move_cursor_or_view(game, using_cursor, 0, -1);
                break;
            case 'a':
                move_cursor_or_view(game, using_cursor, -1, 0);
                break;
            case 's':
                move_cursor_or_view(game, using_cursor, 0, 1);
                break;
            case 'd':
                move_cursor_or_view(game, using_cursor, 1, 0);
                break;

            // Menu
            case '1': exit(0);

            case '2':
                game->killAll();
                break;
            case '3':
                game->killView();
                break;
            case '4':
                break; // Todo
            case '5':
                game->makeRandomAlive();
                break;
            case '6':
                game->resetCursor();
                using_cursor = true;
                break;
            case '7':
                game->fillViewFromFile(gliderGun);
                break;
            case '8':
                game->nextGeneration();
                break;
            case '9':
                while(true)
                {
                    game->nextGeneration();
                    game->printView(using_cursor);
                    usleep(200000); // Sleep for 50ms
                }
                break;

            case ' ':
                if(using_cursor) game->toggleCursor();
                break;
            case 'b':
                using_cursor = false;
                break;
        }
    }

    return 0;
}
