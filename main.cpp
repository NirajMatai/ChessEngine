#include <iostream>

using namespace std;

// Piece and Color Definitions
const int EMPTY = 0;

const int PAWN   = 1;
const int KNIGHT = 2;
const int BISHOP = 3;
const int ROOK   = 4;
const int QUEEN  = 5;
const int KING   = 6;

const int WHITE = 8;
const int BLACK = 16;

// The 0x88 board array
int board[128];

void clearBoard() {
    for (int i = 0; i < 128; i++) {
        board[i] = EMPTY;
    }
}

// Converts the integer piece value to its corresponding ASCII character
char getPieceChar(int piece) {
    switch (piece) {
        case EMPTY: return '.';
        case WHITE | PAWN: return 'P';
        case WHITE | KNIGHT: return 'N';
        case WHITE | BISHOP: return 'B';
        case WHITE | ROOK: return 'R';
        case WHITE | QUEEN: return 'Q';
        case WHITE | KING: return 'K';
        case BLACK | PAWN: return 'p';
        case BLACK | KNIGHT: return 'n';
        case BLACK | BISHOP: return 'b';
        case BLACK | ROOK: return 'r';
        case BLACK | QUEEN: return 'q';
        case BLACK | KING: return 'k';
        default: return '?';
    }
}

// Prints the 0x88 array as a standard 8x8 chessboard
void printBoard() {
    cout << "\n  a b c d e f g h\n\n";
    
    // Loop from Rank 8 (top) down to Rank 1 (bottom)
    for (int rank = 7; rank >= 0; rank--) {
        cout << rank + 1 << " "; // Print the rank number on the left
        
        for (int file = 0; file < 8; file++) {
            // Calculate the 0x88 array index
            int square = rank * 16 + file;
            cout << getPieceChar(board[square]) << " ";
        }
        
        cout << rank + 1 << "\n"; // Print the rank number on the right
    }
    
    cout << "\n  a b c d e f g h\n\n";
}

int main() {
    clearBoard();
    printBoard();
    return 0;
}