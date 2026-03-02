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

//MOVE OFFSETS
int knightOffsets[8] = {33, 31, 18, 14, -14, -18, -31, -33};
int kingOffsets[8]   = {17, 16, 15, 1, -1, -15, -16, -17};

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

// Helper to convert array index (e.g., 1) to chess square (e.g., "b1")
string squareToAlgebraic(int sq) {
    int rank = sq / 16;
    int file = sq % 16;
    string s = "";
    s += (char)('a' + file);
    s += (char)('1' + rank);
    return s;
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

// Phase 2, Step 4: Move Generation (Leapers only for now)
void generateMoves() {
    cout << "--- GENERATING MOVES FOR " << (sideToMove == WHITE ? "WHITE" : "BLACK") << " ---\n";
    
    for (int square = 0; square < 128; square++) {
        // Skip ghost squares
        if (square & 0x88) continue;
        
        int piece = board[square];
        if (piece == EMPTY) continue;
        
        int pieceColor = piece & (WHITE | BLACK);
        if (pieceColor != sideToMove) continue;

        // Bitwise trick to strip color and get just the piece type (1 to 6)
        int pieceType = piece & 7; 
        
        // KNIGHT MOVES
        if (pieceType == KNIGHT) {
            for (int i = 0; i < 8; i++) {
                int target = square + knightOffsets[i];
                
                // 0x88 check: Is target on the board?
                if ((target & 0x88) == 0) {
                    int targetPiece = board[target];
                    int targetColor = targetPiece & (WHITE | BLACK);
                    
                    // Valid if empty or has enemy piece
                    if (targetPiece == EMPTY || targetColor != sideToMove) {
                        cout << "Knight on " << squareToAlgebraic(square) << " jumps to " << squareToAlgebraic(target);
                        if (targetPiece != EMPTY) cout << " (CAPTURE)";
                        cout << "\n";
                    }
                }
            }
        }
        
        // KING MOVES
        else if (pieceType == KING) {
            for (int i = 0; i < 8; i++) {
                int target = square + kingOffsets[i];
                if ((target & 0x88) == 0) {
                    int targetPiece = board[target];
                    int targetColor = targetPiece & (WHITE | BLACK);
                    
                    if (targetPiece == EMPTY || targetColor != sideToMove) {
                        cout << "King on " << squareToAlgebraic(square) << " steps to " << squareToAlgebraic(target);
                        if (targetPiece != EMPTY) cout << " (CAPTURE)";
                        cout << "\n";
                    }
                }
            }
        }
    }
    cout << "-----------------------------------\n\n";
}

int main() {
    // Load the standard starting chess position
    parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    printBoard();
    generateMoves();
    printGameState();
    
    return 0;
}