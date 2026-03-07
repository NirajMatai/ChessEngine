#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <ctime>
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
    for (int i = 0; i < 128; i++) board[i] = EMPTY;
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
            if (c == 'P') piece = WHITE | PAWN; else if (c == 'N') piece = WHITE | KNIGHT;
            else if (c == 'B') piece = WHITE | BISHOP; else if (c == 'R') piece = WHITE | ROOK;
            else if (c == 'Q') piece = WHITE | QUEEN; else if (c == 'K') piece = WHITE | KING;
            else if (c == 'p') piece = BLACK | PAWN; else if (c == 'n') piece = BLACK | KNIGHT;
            else if (c == 'b') piece = BLACK | BISHOP; else if (c == 'r') piece = BLACK | ROOK;
            else if (c == 'q') piece = BLACK | QUEEN; else if (c == 'k') piece = BLACK | KING;
            board[square] = piece;
            file++;
        }
    }

    if (colorPart == "w") sideToMove = WHITE; else sideToMove = BLACK;

    if (castlingPart != "-") {
        for (char const &c : castlingPart) {
            if (c == 'K') castleWK = 1; else if (c == 'Q') castleWQ = 1;
            else if (c == 'k') castleBK = 1; else if (c == 'q') castleBQ = 1;
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
    cout << "Castling Rights: " << (castleWK ? "K" : "-") << (castleWQ ? "Q" : "-") 
         << (castleBK ? "k" : "-") << (castleBQ ? "q" : "-") << endl << endl;
}

string squareToAlgebraic(int sq) {
    int rank = sq / 16;
    int file = sq % 16;
    string s = "";
    s += (char)('a' + file);
    s += (char)('1' + rank);
    return s;
}

int algebraicToSquare(string alg) {
    int file = alg[0] - 'a';
    int rank = alg[1] - '1';
    return rank * 16 + file;
}

void makeMove(string moveStr) {
    int src = algebraicToSquare(moveStr.substr(0, 2));
    int tgt = algebraicToSquare(moveStr.substr(2, 2));
    int piece = board[src];

    if (moveStr.length() == 5) {
        char promo = moveStr[4];
        int color = piece & (WHITE | BLACK);
        if (promo == 'q') piece = color | QUEEN;
        else if (promo == 'r') piece = color | ROOK;
        else if (promo == 'b') piece = color | BISHOP;
        else if (promo == 'n') piece = color | KNIGHT;
    }

    board[tgt] = piece;
    board[src] = EMPTY;
    sideToMove = (sideToMove == WHITE) ? BLACK : WHITE;
}

// --- PHASE 3, STEP 3: THE RANDOM MOVER ---
// Instead of void, it now returns a list of UCI move strings
vector<string> generateMoves() {
    vector<string> moves;

    for (int square = 0; square < 128; square++) {
        if (square & 0x88) continue;
        int piece = board[square];
        if (piece == EMPTY) continue;
        int pieceColor = piece & (WHITE | BLACK);
        if (pieceColor != sideToMove) continue;
        int pieceType = piece & 7;

        // A quick lambda helper to build the UCI string and auto-promote to Queen
        auto addMove = [&](int targetSquare) {
            string m = squareToAlgebraic(square) + squareToAlgebraic(targetSquare);
            if (pieceType == PAWN && (targetSquare >= 112 || targetSquare <= 7)) m += "q";
            moves.push_back(m);
        };

        if (pieceType == PAWN) {
            if (sideToMove == WHITE) {
                int target = square + 16;
                if ((target & 0x88) == 0 && board[target] == EMPTY) {
                    addMove(target);
                    if (square >= 16 && square <= 23 && board[square + 32] == EMPTY) addMove(square + 32);
                }
                if (((square + 15) & 0x88) == 0 && ((board[square + 15] != EMPTY && (board[square + 15] & BLACK)) || square + 15 == enPassantSquare)) addMove(square + 15);
                if (((square + 17) & 0x88) == 0 && ((board[square + 17] != EMPTY && (board[square + 17] & BLACK)) || square + 17 == enPassantSquare)) addMove(square + 17);
            } else { 
                int target = square - 16;
                if ((target & 0x88) == 0 && board[target] == EMPTY) {
                    addMove(target);
                    if (square >= 96 && square <= 103 && board[square - 32] == EMPTY) addMove(square - 32);
                }
                if (((square - 17) & 0x88) == 0 && ((board[square - 17] != EMPTY && (board[square - 17] & WHITE)) || square - 17 == enPassantSquare)) addMove(square - 17);
                if (((square - 15) & 0x88) == 0 && ((board[square - 15] != EMPTY && (board[square - 15] & WHITE)) || square - 15 == enPassantSquare)) addMove(square - 15);
            }
        }
        else if (pieceType == KNIGHT || pieceType == KING) {
            int* offsets = (pieceType == KNIGHT) ? knightOffsets : kingOffsets;
            for (int i = 0; i < 8; i++) {
                int target = square + offsets[i];
                if ((target & 0x88) == 0) {
                    int targetColor = board[target] & (WHITE | BLACK);
                    if (board[target] == EMPTY || targetColor != sideToMove) addMove(target);
                }
            }
        }
        else if (pieceType == BISHOP || pieceType == ROOK || pieceType == QUEEN) {
            int* offsets; int numDirections;
            if (pieceType == BISHOP) { offsets = bishopOffsets; numDirections = 4; }
            else if (pieceType == ROOK) { offsets = rookOffsets; numDirections = 4; }
            else { offsets = queenOffsets; numDirections = 8; }
            for (int i = 0; i < numDirections; i++) {
                int target = square + offsets[i];
                while ((target & 0x88) == 0) {
                    int targetColor = board[target] & (WHITE | BLACK);
                    if (board[target] == EMPTY) { addMove(target); } 
                    else if (targetColor != sideToMove) { addMove(target); break; } 
                    else { break; }
                    target += offsets[i];
                }
            }
        }
    }
    return moves;
}

void uciLoop() {
    string line;
    srand(time(0)); 
    cout << "Engine is alive. Type 'uci' to begin." << endl;

    while (getline(cin, line)) {
        if (line == "uci") {
            cout << "id name TheGreatSage" << endl;
            cout << "id author You" << endl;
            cout << "uciok" << endl;
        } 
        else if (line == "isready") {
            cout << "readyok" << endl;
        } 
        else if (line.substr(0, 8) == "position") {
            if (line.find("startpos") != string::npos) {
                parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
            } else if (line.find("fen") != string::npos) {
                size_t fenStart = line.find("fen ") + 4;
                size_t movesStart = line.find(" moves");
                string fen = line.substr(fenStart, movesStart - fenStart);
                parseFEN(fen);
            }

            size_t movesStart = line.find("moves ");
            if (movesStart != string::npos) {
                string movesStr = line.substr(movesStart + 6);
                stringstream ss(movesStr);
                string singleMove;
                while (ss >> singleMove) makeMove(singleMove);
            }
        }
        else if (line.substr(0, 2) == "go") {
            vector<string> moves = generateMoves();
            if (moves.size() > 0) {
                int randomIndex = rand() % moves.size();
                cout << "bestmove " << moves[randomIndex] << endl;
            } else {
                cout << "bestmove 0000" << endl; 
            }
        }
        else if (line == "d") {
            printBoard();
            printGameState();
        }
        else if (line == "quit") {
            break; 
        }
    }
}

int main() {
    // Force standard output to not buffer at all, just to be bulletproof
    setvbuf(stdout, NULL, _IONBF, 0); 
    uciLoop();
    return 0;
}