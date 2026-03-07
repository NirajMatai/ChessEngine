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

// --- PHASE 3, STEP 2: PARSING MOVES ---

// Helper to convert "e2" into array index
int algebraicToSquare(string alg) {
    int file = alg[0] - 'a';
    int rank = alg[1] - '1';
    return rank * 16 + file;
}

// Physically updates the C++ array based on the text move
void makeMove(string moveStr) {
    int src = algebraicToSquare(moveStr.substr(0, 2));
    int tgt = algebraicToSquare(moveStr.substr(2, 2));
    int piece = board[src];

    // Handle promotions sent by GUI (e.g., e7e8q)
    if (moveStr.length() == 5) {
        char promo = moveStr[4];
        int color = piece & (WHITE | BLACK);
        if (promo == 'q') piece = color | QUEEN;
        else if (promo == 'r') piece = color | ROOK;
        else if (promo == 'b') piece = color | BISHOP;
        else if (promo == 'n') piece = color | KNIGHT;
    }

    // Move the piece
    board[tgt] = piece;
    board[src] = EMPTY;

    // Swap turns
    sideToMove = (sideToMove == WHITE) ? BLACK : WHITE;
}

// --- PHASE 3, STEP 1 & 2: THE UCI LOOP ---
void uciLoop() {
    string line;
    cout << "Engine is alive. Type 'uci' to begin.\n";

    while (getline(cin, line)) {
        if (line == "uci") {
            cout << "id name TheGreatSage\n";
            cout << "id author You\n";
            cout << "uciok\n";
        } 
        else if (line == "isready") {
            cout << "readyok\n";
        } 
        else if (line.substr(0, 8) == "position") {
            // Setup the board state
            if (line.find("startpos") != string::npos) {
                parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
            } else if (line.find("fen") != string::npos) {
                size_t fenStart = line.find("fen ") + 4;
                size_t movesStart = line.find(" moves");
                string fen = line.substr(fenStart, movesStart - fenStart);
                parseFEN(fen);
            }

            // If the GUI sent a list of moves, play them on our array
            size_t movesStart = line.find("moves ");
            if (movesStart != string::npos) {
                string movesStr = line.substr(movesStart + 6);
                stringstream ss(movesStr);
                string singleMove;
                while (ss >> singleMove) {
                    makeMove(singleMove);
                }
            }
        }
        else if (line == "d") {
            // Custom debug command to see what the engine sees
            printBoard();
            printGameState();
        }
        else if (line == "quit") {
            break; 
        }
    }
}

int main() {
    uciLoop();
    return 0;
}