#include <iostream>
#include <string>

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

// --- GAME STATE VARIABLES ---
int sideToMove = WHITE; 
int enPassantSquare = -1;

int castleWK = 1; 
int castleWQ = 1; 
int castleBK = 1; 
int castleBQ = 1; 

void clearBoard() {
    for (int i = 0; i < 128; i++) {
        board[i] = EMPTY;
    }
}

// Phase 2, Step 3: Parse FEN string to set up the board
void parseFEN(string fen) {
    clearBoard();
    
    int rank = 7; // Start at the 8th rank (top of board)
    int file = 0; // Start at the 'a' file (left of board)

    for (char const &c : fen) {
        if (c == ' ') break; // Stop parsing after the pieces section

        if (c == '/') {
            rank--;
            file = 0;
        } else if (isdigit(c)) {
            file += (c - '0'); // Add empty squares
        } else {
            int square = rank * 16 + file;
            int piece = EMPTY;

            if (c == 'P') piece = WHITE | PAWN;
            else if (c == 'N') piece = WHITE | KNIGHT;
            else if (c == 'B') piece = WHITE | BISHOP;
            else if (c == 'R') piece = WHITE | ROOK;
            else if (c == 'Q') piece = WHITE | QUEEN;
            else if (c == 'K') piece = WHITE | KING;
            else if (c == 'p') piece = BLACK | PAWN;
            else if (c == 'n') piece = BLACK | KNIGHT;
            else if (c == 'b') piece = BLACK | BISHOP;
            else if (c == 'r') piece = BLACK | ROOK;
            else if (c == 'q') piece = BLACK | QUEEN;
            else if (c == 'k') piece = BLACK | KING;

            board[square] = piece;
            file++;
        }
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
    // Load the standard starting chess position
    parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    printBoard();
    printGameState();
    
    return 0;
}