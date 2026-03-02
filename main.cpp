#include <iostream>

using namespace std;

//PIECE & COLOR DEFINITIONS

const int EMPTY = 0;

const int PAWN   = 1;
const int KNIGHT = 2;
const int BISHOP = 3;
const int ROOK   = 4;
const int QUEEN  = 5;
const int KING   = 6;

//Colors
const int WHITE = 8;  // Binary: 01000
const int BLACK = 16; // Binary: 10000

//THE BOARD
//The 0x88 board array (128 slots)
int board[128];

//Function to clear the board
void clearBoard() {
    for (int i = 0; i < 128; i++) {
        board[i] = EMPTY;
    }
}

int main() {
    cout << "Engine starts." << endl;
    
    //Test
    clearBoard();
    cout << "Board cleared. 128 slots ready." << endl;
    
    //Math test to show how pieces combine
    int myPiece = WHITE | KNIGHT;
    cout << "White Knight integer value: " << myPiece << endl;
    
    return 0;
}