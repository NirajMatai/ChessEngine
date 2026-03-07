#include <iostream>
#include <string>
#include <sstream>
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

// --- THE BOARD & STATE ---
int board[128];
int sideToMove = WHITE;
int enPassantSquare = -1;
int castleWK = 0, castleWQ = 0, castleBK = 0, castleBQ = 0;

// --- MOVE OFFSETS ---
int knightOffsets[8] = {33, 31, 18, 14, -14, -18, -31, -33};
int kingOffsets[8]   = {17, 16, 15, 1, -1, -15, -16, -17};
int bishopOffsets[4] = {17, 15, -15, -17};
int rookOffsets[4]   = {16, 1, -1, -16};
int queenOffsets[8]  = {17, 16, 15, 1, -1, -15, -16, -17};

void clearBoard() {
    for (int i = 0; i < 128; i++) {
        board[i] = EMPTY;
    }
}

void parseFEN(string fen) {
    clearBoard();
    castleWK = 0; castleWQ = 0; castleBK = 0; castleBQ = 0;
    enPassantSquare = -1;

    stringstream ss(fen);
    string boardPart, colorPart, castlingPart, epPart;
    ss >> boardPart >> colorPart >> castlingPart >> epPart;

    int rank = 7, file = 0;
    for (char const &c : boardPart) {
        if (c == '/') {
            rank--; file = 0;
        } else if (isdigit(c)) {
            file += (c - '0');
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

    if (colorPart == "w") sideToMove = WHITE;
    else sideToMove = BLACK;

    if (castlingPart != "-") {
        for (char const &c : castlingPart) {
            if (c == 'K') castleWK = 1;
            else if (c == 'Q') castleWQ = 1;
            else if (c == 'k') castleBK = 1;
            else if (c == 'q') castleBQ = 1;
        }
    }

    if (epPart != "-") {
        int epFile = epPart[0] - 'a';
        int epRank = epPart[1] - '1';
        enPassantSquare = epRank * 16 + epFile;
    }
}

char getPieceChar(int piece) {
    switch (piece) {
        case EMPTY: return '.';
        case WHITE | PAWN: return 'P'; case WHITE | KNIGHT: return 'N';
        case WHITE | BISHOP: return 'B'; case WHITE | ROOK: return 'R';
        case WHITE | QUEEN: return 'Q'; case WHITE | KING: return 'K';
        case BLACK | PAWN: return 'p'; case BLACK | KNIGHT: return 'n';
        case BLACK | BISHOP: return 'b'; case BLACK | ROOK: return 'r';
        case BLACK | QUEEN: return 'q'; case BLACK | KING: return 'k';
        default: return '?';
    }
}

void printBoard() {
    cout << "\n  a b c d e f g h\n\n";
    for (int rank = 7; rank >= 0; rank--) {
        cout << rank + 1 << " ";
        for (int file = 0; file < 8; file++) {
            cout << getPieceChar(board[rank * 16 + file]) << " ";
        }
        cout << rank + 1 << "\n";
    }
    cout << "\n  a b c d e f g h\n\n";
}

void printGameState() {
    cout << "Side to move: " << (sideToMove == WHITE ? "White" : "Black") << endl;
    cout << "En Passant Square: " << (enPassantSquare == -1 ? "None" : to_string(enPassantSquare)) << endl;
    cout << "Castling Rights: ";
    cout << (castleWK ? "K" : "-"); cout << (castleWQ ? "Q" : "-");
    cout << (castleBK ? "k" : "-"); cout << (castleBQ ? "q" : "-") << endl << endl;
}

string squareToAlgebraic(int sq) {
    int rank = sq / 16;
    int file = sq % 16;
    string s = "";
    s += (char)('a' + file);
    s += (char)('1' + rank);
    return s;
}

void generateMoves() {
    // Hidden for clarity during the UCI step, but it's exactly the same logic as Phase 2, Step 4.5
}

// --- PHASE 3, STEP 1: THE UCI LOOP ---
void uciLoop() {
    string line;
    cout << "Engine is alive. Type 'uci' to begin.\n";

    // Infinite loop waiting for commands
    while (getline(cin, line)) {
        if (line == "uci") {
            cout << "id name ChromaticChess\n"; // Named after your YT channel vibe
            cout << "id author You\n";
            cout << "uciok\n";
        } 
        else if (line == "isready") {
            cout << "readyok\n";
        } 
        else if (line == "quit") {
            break; // Exits the loop and closes the engine
        }
    }
}

int main() {
    // We wipe out the hardcoded FEN tests. 
    // The engine's only job when booting up is to enter the communication loop.
    uciLoop();
    return 0;
}