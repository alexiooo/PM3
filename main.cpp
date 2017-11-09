#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <unistd.h>


using namespace std;

#define WORLD_SIZE 1000

// Random number generator
class RNG {
    private:
    static long random_number;

    public:

    static void set_seed(long seed) {
        RNG::random_number = seed;
    }

    static long get_random_number() {
        return (RNG::random_number = (221 * RNG::random_number + 1) % WORLD_SIZE);
    }
};

// Define storage for variable
long RNG::random_number;

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

    // Parameters
    // Size of changes in view or cursor position
    int view_step_size = 1;
    int cursor_step_size = 1;
    // Characters for respectively dead or alive cells
    char dead_cell = ' ';
    char live_cell = 'x';
    // The percentage (out of 100) of random cells picked and made alive
    // when the view is filled randomly.
    int random_percentage = 50;

    Life() {
        time_t rawtime;
        time(&rawtime);

        struct tm date_now = *localtime(&rawtime);

        RNG::set_seed(date_now.tm_sec);
    }

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
        view_x += x*view_step_size;
        view_y += y*view_step_size;
    }

    void resetCursor() {
        cursor_x = view_x;
        cursor_y = view_y;
    }

    void moveCursor(int x, int y) {
        int new_x = cursor_x + x*cursor_step_size;
        int new_y = cursor_y + y*cursor_step_size;

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

    // Fills the view with random cells (determined by random_percentage)
    void makeRandomAlive() {
        int threshold = random_percentage * WORLD_SIZE / 100;

        // Then fill cells at random
        for(int x=view_x; x<view_x+VIEW_WIDTH; x++)
        for(int y=view_y; y<view_y+VIEW_HEIGHT; y++)
        {
            board[x][y] = RNG::get_random_number() < threshold;
        }
    }

    void printView(bool print_cursor) {

        cout << "Coordinates of view: (" << view_x << ", " << view_y << ")    ";
        cout << "Stepsize: " << view_step_size << "/" << cursor_step_size << ", ";
        cout << "Random Filling: " << random_percentage << "%, ";
        cout << "Live cells: '" << live_cell << "', ";
        cout << "Dead cells: '" << dead_cell << "'" << endl;

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
                if(Life::positionWithinWorld(x, y) && board[x][y])
                    cout << live_cell;
                else if(Life::positionOnEdge(x, y)) cout << "#";
                else cout << dead_cell;
            }

            cout << endl;
        }
    }

    void fillViewFromFile(string filename)
    {
        int symbol;

        int x = view_x;
        int y = view_y;

        ifstream file(filename);

        killAll();

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

    int get_maximum_cursor_step_size() {
        return (
            Life::VIEW_HEIGHT < Life::VIEW_WIDTH ?
                                Life::VIEW_HEIGHT
                                : Life::VIEW_WIDTH
        ) - 1;
    }

};

class Menu {

    private:
        // Gives the first non-newline character on stdin
        static char read_character()
        {
            char kar;

            while((kar = cin.get()) == '\n'){};

            return kar;
        }

        // Reads a number from stdin, stops when number would exceed maximum
        static int read_number(int maximum)
        {
            int num = 0;

            char kar = read_character();
            do {
                if ('0' <= kar && kar <= '9')
                {
                    int new_num = num * 10;
                    new_num += kar - '0';
                    if(new_num <= maximum)
                    {
                        num = new_num;
                    } else {
                        // Prevent extra numbers from being seen as menu-input
                        while(cin.get() != '\n'){}
                        return num;
                    }
                }
                kar = cin.get();
            } while(kar != '\n');
            return num;
        }

        static void print_cursor_menu()
    {
        cout << "Use w,a,s,d to move cursor around" << endl;
        cout << "Press space to toggle cell" << endl;
        cout << "Press b to return to menu" << endl;
    }

    // handles input, returns whether screen should be redrawn
    static bool input_cursor_menu(Life *game, char input)
    {
        switch(input)\
        {
            case 'w':
                game->moveCursor(0, -1);
               break;
            case 'a':
                game->moveCursor(-1, 0);
                break;
            case 's':
                game->moveCursor(0, 1);
                break;
            case 'd':
                game->moveCursor(1, 0);
                break;

            case ' ':
                game->toggleCursor();
                break;
            case 'b':
                Menu::current_menu = Menu::MAIN;
                break;
        }
        return false;
    }

    static void print_param_menu(Life *game) {
        cout << "Change parameters or press b to go back" << endl;
        cout << "Numbers will be read as long as they don't exceed the given maximum,"
            << " further digits are ignored" << endl;
        cout << "1.View Step Size (" << game->view_step_size << ") ";
        cout << "2.Cursor Step Size (" << game->cursor_step_size << ") ";
        cout << "3.Random Filling Percentage (" << game->random_percentage << ") ";
        cout << "4.Alive Char ('" << game->live_cell << "') ";
        cout << "5.Dead Char ('" << game->dead_cell << "') " << endl;
    }

    // handles input, returns whether screen should be redrawn
    static bool input_param_menu(Life *game, char input){
        switch(input){
            case '1':
                game->printView(false);
                cout << endl << endl;
                cout << "Give a new step size (0-500) for view changes:"
                    << endl;
                game->view_step_size = read_number(500);
                break;

            case '2':
                game->printView(false);
                cout << endl << endl;
                cout << "Give a new step size (0-" << game->get_maximum_cursor_step_size()
                    <<") for cursor changes:" << endl;
                game->cursor_step_size = read_number( game->get_maximum_cursor_step_size() );
                break;

            case '3':
                game->printView(false);
                cout << endl << endl;
                cout << "Give a percentage (0-100) for random filling:" << endl;
                game->random_percentage = read_number(100);
                break;

            case '4':
                game->printView(false);
                cout << endl << endl;
                cout << "Give a new character for live cells:" << endl;
                game->live_cell = read_character();
                break;

            case '5':
                game->printView(false);
                cout << endl << endl;
                cout << "Give a new character for dead cells:" << endl;
                game->dead_cell = read_character();
                break;

            case 'b':
                current_menu = MAIN;
                return false;

            default:
                return false;
        }
        return true;
    };

    static void print_main_menu()
    {
        cout << "1. Exit ";
        cout << "2. Clean world ";
        cout << "3. Clean view ";
        cout << "4. Change parameters ";
        cout << "5. Random ";
        cout << "6. Toggle using cursor ";
        cout << "7. Load glidergun.txt " << endl;
        cout << " 8. Compute one generation ";
        cout << "9. Run Game of Life " << endl;
        cout << "Use w,a,s,d to move view around" << endl;
    }

    // handles input, returns whether screen should be redrawn
    static bool input_main_menu(Life *game, char input)
    {
        switch(input)
        {
            // Movements
            case 'w':
                game->moveView(0, -1);
                break;
            case 'a':
                game->moveView(-1, 0);
                break;
            case 's':
                game->moveView(0, 1);
                break;
            case 'd':
                game->moveView(1, 0);
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
                current_menu = PARAM;
                break;
            case '5':
                game->makeRandomAlive();
                break;
            case '6':
                game->resetCursor();
                current_menu = CURSOR;
                break;
            case '7':
                game->fillViewFromFile("gliderGun.txt");
                break;
            case '8':
                game->nextGeneration();
                break;
            case '9':
                for(;;)
                {
                    game->nextGeneration();
                    game->printView(false);
                    cout << endl << endl << endl;
                    usleep(200000); // Sleep for 50ms
                }
                break;
        }
        return false;
    }

    public:

    static int current_menu;

    static const int MAIN = 0;
    static const int CURSOR = 1;
    static const int PARAM = 2;

    static void print_menu(Life *game)
    {
        switch(current_menu){
            case CURSOR:
                print_cursor_menu();
                break;

            case PARAM:
                print_param_menu(game);
                break;

            case MAIN:
            default:
                print_main_menu();
                break;
        }
    }

    // Handles input for the active menu
    // Returns whether the screen should be redrawn
    static bool handle_input(Life *game, char input){
        if(input == '\n')
        {
            return true;
        } else {
            switch(current_menu){
                case CURSOR:
                    return input_cursor_menu(game, input);
                    break;

                case PARAM:
                    return input_param_menu(game, input);
                    break;

                case MAIN:
                default:
                    return input_main_menu(game, input);
                    break;
            }
        }
    }
};

int Menu::current_menu = Menu::MAIN;

int main()
{
    Life *game = new Life();
    Menu::current_menu = Menu::MAIN;

    game->printView(false);
    Menu::print_menu(game);

    for(;;) {
        char input = cin.get();
        if(Menu::handle_input( game, input) ) {
            game->printView( Menu::current_menu == Menu::CURSOR );
            Menu::print_menu(game);
        };
    }

    return 0;
}
