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

// --- THE BOARD & STATE ---
int board[128];
int sideToMove = WHITE; 
int enPassantSquare = -1;
int castleWK = 1, castleWQ = 1, castleBK = 1, castleBQ = 1; 

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
    int rank = 7, file = 0;

    for (char const &c : fen) {
        if (c == ' ') break; 
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
    cout << "--- GENERATING MOVES FOR " << (sideToMove == WHITE ? "WHITE" : "BLACK") << " ---\n";
    
    for (int square = 0; square < 128; square++) {
        if (square & 0x88) continue;
        
        int piece = board[square];
        if (piece == EMPTY) continue;
        
        int pieceColor = piece & (WHITE | BLACK);
        if (pieceColor != sideToMove) continue;

        int pieceType = piece & 7; 
        
        // --- PAWNS ---
        if (pieceType == PAWN) {
            if (sideToMove == WHITE) {
                // Single Push
                int target = square + 16;
                if ((target & 0x88) == 0 && board[target] == EMPTY) {
                    cout << "Pawn on " << squareToAlgebraic(square) << " pushes to " << squareToAlgebraic(target) << "\n";
                    // Double Push (Only if on Rank 2, meaning array index is between 16 and 23)
                    if (square >= 16 && square <= 23) {
                        int doubleTarget = square + 32;
                        if ((doubleTarget & 0x88) == 0 && board[doubleTarget] == EMPTY) {
                            cout << "Pawn on " << squareToAlgebraic(square) << " double-pushes to " << squareToAlgebraic(doubleTarget) << "\n";
                        }
                    }
                }
                
                // Captures
                int capLeft = square + 15;
                if ((capLeft & 0x88) == 0 && board[capLeft] != EMPTY && (board[capLeft] & BLACK)) {
                    cout << "Pawn on " << squareToAlgebraic(square) << " captures on " << squareToAlgebraic(capLeft) << "\n";
                }
                int capRight = square + 17;
                if ((capRight & 0x88) == 0 && board[capRight] != EMPTY && (board[capRight] & BLACK)) {
                    cout << "Pawn on " << squareToAlgebraic(square) << " captures on " << squareToAlgebraic(capRight) << "\n";
                }
            } 
            else { // BLACK PAWNS
                // Single Push
                int target = square - 16;
                if ((target & 0x88) == 0 && board[target] == EMPTY) {
                    cout << "Pawn on " << squareToAlgebraic(square) << " pushes to " << squareToAlgebraic(target) << "\n";
                    // Double Push (Only if on Rank 7, meaning array index is between 96 and 103)
                    if (square >= 96 && square <= 103) {
                        int doubleTarget = square - 32;
                        if ((doubleTarget & 0x88) == 0 && board[doubleTarget] == EMPTY) {
                            cout << "Pawn on " << squareToAlgebraic(square) << " double-pushes to " << squareToAlgebraic(doubleTarget) << "\n";
                        }
                    }
                }
                
                // Captures
                int capLeft = square - 17;
                if ((capLeft & 0x88) == 0 && board[capLeft] != EMPTY && (board[capLeft] & WHITE)) {
                    cout << "Pawn on " << squareToAlgebraic(square) << " captures on " << squareToAlgebraic(capLeft) << "\n";
                }
                int capRight = square - 15;
                if ((capRight & 0x88) == 0 && board[capRight] != EMPTY && (board[capRight] & WHITE)) {
                    cout << "Pawn on " << squareToAlgebraic(square) << " captures on " << squareToAlgebraic(capRight) << "\n";
                }
            }
        }
        
        // --- LEAPERS (Knight & King) ---
        else if (pieceType == KNIGHT || pieceType == KING) {
            int* offsets = (pieceType == KNIGHT) ? knightOffsets : kingOffsets;
            for (int i = 0; i < 8; i++) {
                int target = square + offsets[i];
                if ((target & 0x88) == 0) {
                    int targetPiece = board[target];
                    int targetColor = targetPiece & (WHITE | BLACK);
                    if (targetPiece == EMPTY || targetColor != sideToMove) {
                        cout << getPieceChar(piece) << " on " << squareToAlgebraic(square) << " moves to " << squareToAlgebraic(target);
                        if (targetPiece != EMPTY) cout << " (CAPTURE)";
                        cout << "\n";
                    }
                }
            }
        }
        
        // --- SLIDERS (Bishop, Rook, Queen) ---
        else if (pieceType == BISHOP || pieceType == ROOK || pieceType == QUEEN) {
            int* offsets;
            int numDirections;

            if (pieceType == BISHOP) { offsets = bishopOffsets; numDirections = 4; }
            else if (pieceType == ROOK) { offsets = rookOffsets; numDirections = 4; }
            else { offsets = queenOffsets; numDirections = 8; } 

            for (int i = 0; i < numDirections; i++) {
                int target = square + offsets[i];
                while ((target & 0x88) == 0) {
                    int targetPiece = board[target];
                    int targetColor = targetPiece & (WHITE | BLACK);

                    if (targetPiece == EMPTY) {
                        cout << getPieceChar(piece) << " on " << squareToAlgebraic(square) << " slides to " << squareToAlgebraic(target) << "\n";
                    } else if (targetColor != sideToMove) {
                        cout << getPieceChar(piece) << " on " << squareToAlgebraic(square) << " slides to " << squareToAlgebraic(target) << " (CAPTURE)\n";
                        break;
                    } else {
                        break;
                    }
                    target += offsets[i];
                }
            }
        }
    }
    cout << "-----------------------------------\n\n";
}

int main() {
    parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    printBoard();
    printGameState();
    
    generateMoves();
    
    return 0;
}