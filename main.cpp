#include <iostream>

using namespace std;

// --- PIECE & COLOR DEFINITIONS ---
const int EMPTY = 0;

const int PAWN   = 1;
const int KNIGHT = 2;
const int BISHOP = 3;
const int ROOK   = 4;
const int QUEEN  = 5;
const int KING   = 6;

const int WHITE = 8;
const int BLACK = 16;

// --- THE BOARD ---
int board[128];

// --- GAME STATE VARIABLES (Phase 2, Step 2) ---
int sideToMove = WHITE; 
int enPassantSquare = -1; // -1 means no en passant capture is currently possible

// Castling rights (1 means we CAN castle, 0 means we LOST the right)
int castleWK = 1; // White Kingside
int castleWQ = 1; // White Queenside
int castleBK = 1; // Black Kingside
int castleBQ = 1; // Black Queenside

// ----------------------------------------------

void clearBoard() {
    for (int i = 0; i < 128; i++) {
        board[i] = EMPTY;
    }
}

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

void printBoard() {
    cout << "\n  a b c d e f g h\n\n";
    for (int rank = 7; rank >= 0; rank--) {
        cout << rank + 1 << " ";
        for (int file = 0; file < 8; file++) {
            int square = rank * 16 + file;
            cout << getPieceChar(board[square]) << " ";
        }
        cout << rank + 1 << "\n";
    }
    cout << "\n  a b c d e f g h\n\n";
}

// Helper function to print the invisible game state
void printGameState() {
    cout << "Side to move: " << (sideToMove == WHITE ? "White" : "Black") << endl;
    cout << "En Passant Square: " << (enPassantSquare == -1 ? "None" : to_string(enPassantSquare)) << endl;
    cout << "Castling Rights: ";
    cout << (castleWK ? "K" : "-");
    cout << (castleWQ ? "Q" : "-");
    cout << (castleBK ? "k" : "-");
    cout << (castleBQ ? "q" : "-") << endl << endl;
}

int main() {
    clearBoard();
    printBoard();
    printGameState();
    return 0;
}