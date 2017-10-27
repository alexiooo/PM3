#include <iostream>

using namespace std;

// Reads a character from stdin, skipping leading Enters
char readChar () {
    char kar;
    while ( (kar = cin.get()) == '\n' ) {}
    return kar;
}

// Reads a number from stdin, stops interpreting when maxDigits has been reached
int readNum( int maxDigits ) {
    char kar;
    int num = 0;
    int maximum = maxDigits * 10;

    kar = readChar();
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
    cout << "(S)top (H)eelschoon S(c)hoon (P)arameters "
}


int main()
{
    printMenu();
    return 0;
}
