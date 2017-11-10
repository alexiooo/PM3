//  Derde opdracht voor Programmeermethoden
//  Auteurs:
//        Alex Keizer (s2046253), eerstejaars Informatica (2017)
//        Léon van Velzen (s2037033), eerstejaars Informatica (2017)
//
//  Gecompileerd onder:
//      Windows 10 met MinGW64 GCC versie 6.3.0
//      Debian GNU/Linux 9 GCC versie 6.3.0
//
//  Aan gewerkt in de periode 27/10/2017 tot 10/11/2017
//
//  Onze implementatie van John Conways Game Of Life op een 1000x1000 wereld
//  Gebruikers zien een 80x25 vershuifbare view van deze wereld
//  Er zijn een aantal opties om de wereld te vullen, vervolgens kan ofwel
//   generatie per generatie ofwel continu Life gedraaid worden

#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <cstdlib>


using namespace std;

// Random number generator
class RNG {
    private:
    static long random_number;

    static const long MULT = 48271UL;

    public:

    static const long MOD =2147483647UL;

    static void setSeed(long seed) {
        RNG::random_number = seed % MOD;
    }

    static long getRandomNumber() {
        return RNG::random_number = (MULT * RNG::random_number) % MOD;
    }
};

// Define storage for variable
long RNG::random_number;

class Life {
    const static int WORLD_SIZE = 1000;

    const int VIEW_WIDTH = 80;
    const int VIEW_HEIGHT = 25;

    // Position of the view
    int view_x = 0;
    int view_y = 0;

    // Absolute position of the cursor
    int cursor_x = 0;
    int cursor_y = 0;

    int generation = 0;

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

    Life() {

        time_t rawtime;
        time(&rawtime);

        struct tm date_now = *localtime(&rawtime);

        RNG::setSeed(date_now.tm_sec);
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
            else
            {
                temp_board[x][y] = false;
            }
        }

        generation++;
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
            if(positionWithinWorld(x,y))
                board[x][y] = false;
        }
    }

    // Moves the view, won't go further than -1 or WORLD_SIZE
    void moveView(int x, int y) {
        int new_x = view_x + x*view_step_size;
        int new_y = view_y + y*view_step_size;

        if(new_x < 0) view_x = -1;
        else if(new_x > WORLD_SIZE - VIEW_WIDTH) view_x = WORLD_SIZE - VIEW_WIDTH + 1;
        else view_x = new_x;

        if(new_y < 0) view_y = -1;
        else if(new_y > WORLD_SIZE - VIEW_HEIGHT) view_y = WORLD_SIZE - VIEW_HEIGHT + 1;
        else view_y = new_y;
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
        killAll();

        int threshold = random_percentage * RNG::MOD / 100;

        // Then fill cells at random
        for(int x=view_x; x<view_x+VIEW_WIDTH; x++)
        for(int y=view_y; y<view_y+VIEW_HEIGHT; y++)
        {
            if(positionWithinWorld(x,y)){
                board[x][y] = RNG::getRandomNumber() < threshold;
            }
        }
    }

    void printView(bool print_cursor) {

        cout << "Coordinates of view: (" << view_x << ", " << view_y << ")  ";
        cout << "Gen: " << generation << "    ";
        cout << "Stepsize: " << view_step_size << "/" << cursor_step_size << ", ";
        cout << "Alive: '" << live_cell << "', ";
        cout << "Dead: '" << dead_cell << "'" << endl;

        if(print_cursor)
        {
            for(int x = view_x; x < cursor_x && x < view_x + VIEW_WIDTH; x++)
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
                if(Life::positionWithinWorld(x, y) && board[x][y]) cout << live_cell;
                else if(Life::positionOnEdge(x, y)) cout << "#";
                else cout << dead_cell;
            }

            cout << endl;
        }
    }

    void fillViewFromFile(string filename) {

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

    int getMaximumCursorStepSize() {

        if(Life::VIEW_HEIGHT < Life::VIEW_WIDTH)
        {
            return Life::VIEW_HEIGHT - 1;
        }
        else
        {
            return Life::VIEW_WIDTH - 1;
        }
    }


    int getViewStepSize() {return view_step_size;}
    int getCursorStepSize() {return cursor_step_size;}
    char getDeadCell() {return dead_cell;}
    char getLiveCell(){return live_cell;}
    int getRandomPercentage(){return random_percentage;}
    void setViewStepSize(int new_size) {

        if(0 <= new_size && new_size <= 500)
            view_step_size = new_size;
    }

    void setCursorStepSize(int new_size) {

        if(0 <= new_size && new_size <= getMaximumCursorStepSize())
            cursor_step_size = new_size;
    }

    void setDeadCell(char new_char) {

        if(new_char != '\t' && new_char != '\n')
            dead_cell = new_char;
    }

    void setLiveCell(char new_char) {

        if(new_char != '\t' && new_char != '\n')
            live_cell = new_char;
    }

    void setRandomPercentage(int new_percentage) {

        if(0 <= new_percentage && new_percentage <= 100)
            random_percentage = new_percentage;
    }

};

class Menu {

    private:

    // Gives the first non-newline character on stdin
    static char readCharacter() {

        char kar;

        while((kar = cin.get()) == '\n'){};

        return kar;
    }

    // Reads a number from stdin, stops when number would exceed maximum
    static int readNumber(int maximum) {

        int num = 0;

        char kar = readCharacter();

        do {
            if ('0' <= kar && kar <= '9')
            {
                int new_num = num * 10;
                new_num += kar - '0';
                if(new_num <= maximum)
                {
                    num = new_num;
                }
                else {
                    // Prevent extra numbers from being seen as menu-input
                    while(cin.get() != '\n'){}
                    return num;
                }
            }

            kar = cin.get();

        } while(kar != '\n');

        return num;
    }

    static void printCursorMenu() {

        cout << "Use w,a,s,d to move cursor around" << endl;
        cout << "Press space to toggle cell" << endl;
        cout << "Press b to return to menu" << endl;
    }

    // handles input, returns whether screen should be redrawn
    static bool inputCursorMenu(Life *game, char input) {

        switch(input)
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

    static void printParamMenu(Life *game) {
        cout << endl;
        cout << "Change parameters or press b to go back" << endl;
        cout << "1.View Step Size (" << game->getViewStepSize() << ") ";
        cout << "2.Cursor Step Size (" << game->getCursorStepSize() << ") ";
        cout << "3.Random Filling Percentage (" << game->getRandomPercentage()
                << ") ";
        cout << "4.Alive Char ('" << game->getLiveCell() << "') ";
        cout << "5.Dead Char ('" << game->getDeadCell() << "') " << endl;
    }

    // handles input, returns whether screen should be redrawn
    static bool inputParamMenu(Life *game, char input) {

        switch(input)
        {
            case '1':
                game->printView(false);
                cout << endl;
                cout << "Numbers will be read as long as they don't exceed the "
                    << "given maximum, further digits are ignored" << endl;
                cout << "Give a new step size (0-500) for view changes:"
                    << endl;
                game->setViewStepSize(readNumber(500));
                break;

            case '2':
                game->printView(false);
                cout << endl;
                cout << "Numbers will be read as long as they don't exceed the "
                    << "given maximum, further digits are ignored" << endl;
                cout << "Give a new step size (0-" << game->getMaximumCursorStepSize()
                    <<") for cursor changes:" << endl;
                game->setCursorStepSize(
                        readNumber(game->getMaximumCursorStepSize())
                    );
                break;

            case '3':
                game->printView(false);
                cout << endl;
                cout << "Numbers will be read as long as they don't exceed the "
                    << "given maximum, further digits are ignored" << endl;
                cout << "Give a percentage (0-100) for random filling:" << endl;
                game->setRandomPercentage(readNumber(100));
                break;

            case '4':
                game->printView(false);
                cout << endl << endl;
                cout << "Give a new character for live cells: "
                    << "(Tab is not allowed)" << endl;
                game->setLiveCell(readCharacter());
                break;

            case '5':
                game->printView(false);
                cout << endl << endl;
                cout << "Give a new character for dead cells: "
                    << "(Tab is not allowed)" << endl;
                game->setDeadCell(readCharacter());
                break;

            case 'b':
                current_menu = MAIN;
                return false;

            default:
                return false;
        }
        return true;
    };

    static void printMainMenu() {

        cout << "1. Exit ";
        cout << "2. Clean world ";
        cout << "3. Clean view ";
        cout << "4. Change parameters ";
        cout << "5. Random ";
        cout << "6. Toggle using cursor ";
        cout << "7. Load glidergun.txt";
        cout << " 8. Compute one generation ";
        cout << "9. Run Game of Life " << endl;
        cout << "Use w,a,s,d to move view around" << endl;
    }

    // handles input, returns whether screen should be redrawn
    static bool inputMainMenu(Life *game, char input) {

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

                    // Sleep for 50ms (POSIX only)
                    struct timespec sleep = {0};
                    sleep.tv_sec = 0;
                    sleep.tv_nsec =  50000000L;
                    nanosleep(&sleep, (struct timespec *)NULL);
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

    static void printMenu(Life *game) {

        switch(current_menu)
        {
            case CURSOR:
                printCursorMenu();
                break;

            case PARAM:
                printParamMenu(game);
                break;

            case MAIN:
            default:
                printMainMenu();
                break;
        }
    }

    // Handles input for the active menu
    // Returns whether the screen should be redrawn
    static bool handleInput(Life *game, char input) {

        if(input == '\n')
        {
            return true;
        }
        else
        {
            switch(current_menu)
            {
                case CURSOR:
                    return inputCursorMenu(game, input);
                    break;

                case PARAM:
                    return inputParamMenu(game, input);
                    break;

                case MAIN:
                default:
                    return inputMainMenu(game, input);
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
    Menu::printMenu(game);

    for(;;) {
        char input = cin.get();

        if(Menu::handleInput(game, input))
        {
            game->printView(Menu::current_menu == Menu::CURSOR);
            Menu::printMenu(game);
        }
    }

    return 0;
}
