#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono> // Added for accurate millisecond timing
using namespace std;
using namespace std::chrono;

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

// --- TIME MANAGEMENT VARIABLES ---
long long startTimeMs = 0;
long long allocatedTimeMs = 0;
bool timeIsUp = false;
long nodesSearched = 0;

long long getTimeMs() {
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

void checkTime() {
    if (timeIsUp) return;
    if (getTimeMs() - startTimeMs > allocatedTimeMs) {
        timeIsUp = true;
    }
}

// --- MOVE OFFSETS ---
int knightOffsets[8] = {33, 31, 18, 14, -14, -18, -31, -33};
int kingOffsets[8]   = {17, 16, 15, 1, -1, -15, -16, -17};
int bishopOffsets[4] = {17, 15, -15, -17};
int rookOffsets[4]   = {16, 1, -1, -16};
int queenOffsets[8]  = {17, 16, 15, 1, -1, -15, -16, -17};

// --- PIECE-SQUARE TABLES (Right-side up for 0x88) ---
const int pawnTable[128] = {
     0,  0,  0,  0,  0,  0,  0,  0,   0,0,0,0,0,0,0,0,
     5, 10, 10,-20,-20, 10, 10,  5,   0,0,0,0,0,0,0,0,
     5, -5,-10,  0,  0,-10, -5,  5,   0,0,0,0,0,0,0,0,
     0,  0,  0, 20, 20,  0,  0,  0,   0,0,0,0,0,0,0,0,
     5,  5, 10, 25, 25, 10,  5,  5,   0,0,0,0,0,0,0,0,
    10, 10, 20, 30, 30, 20, 10, 10,   0,0,0,0,0,0,0,0,
    50, 50, 50, 50, 50, 50, 50, 50,   0,0,0,0,0,0,0,0,
     0,  0,  0,  0,  0,  0,  0,  0,   0,0,0,0,0,0,0,0
};

const int knightTable[128] = {
    -50,-40,-30,-30,-30,-30,-40,-50,   0,0,0,0,0,0,0,0,
    -40,-20,  0,  5,  5,  0,-20,-40,   0,0,0,0,0,0,0,0,
    -30,  5, 10, 15, 15, 10,  5,-30,   0,0,0,0,0,0,0,0,
    -30,  0, 15, 20, 20, 15,  0,-30,   0,0,0,0,0,0,0,0,
    -30,  5, 15, 20, 20, 15,  5,-30,   0,0,0,0,0,0,0,0,
    -30,  0, 10, 15, 15, 10,  0,-30,   0,0,0,0,0,0,0,0,
    -40,-20,  0,  0,  0,  0,-20,-40,   0,0,0,0,0,0,0,0,
    -50,-40,-30,-30,-30,-30,-40,-50,   0,0,0,0,0,0,0,0
};

const int bishopTable[128] = {
    -20,-10,-10,-10,-10,-10,-10,-20,   0,0,0,0,0,0,0,0,
    -10,  0,  0,  0,  0,  0,  0,-10,   0,0,0,0,0,0,0,0,
    -10,  0,  5, 10, 10,  5,  0,-10,   0,0,0,0,0,0,0,0,
    -10,  5,  5, 10, 10,  5,  5,-10,   0,0,0,0,0,0,0,0,
    -10,  0, 10, 10, 10, 10,  0,-10,   0,0,0,0,0,0,0,0,
    -10, 10, 10, 10, 10, 10, 10,-10,   0,0,0,0,0,0,0,0,
    -10,  5,  0,  0,  0,  0,  5,-10,   0,0,0,0,0,0,0,0,
    -20,-10,-10,-10,-10,-10,-10,-20,   0,0,0,0,0,0,0,0
};

const int rookTable[128] = {
      0,  0,  5, 10, 10,  5,  0,  0,   0,0,0,0,0,0,0,0,
     -5,  0,  0,  0,  0,  0,  0, -5,   0,0,0,0,0,0,0,0,
     -5,  0,  0,  0,  0,  0,  0, -5,   0,0,0,0,0,0,0,0,
     -5,  0,  0,  0,  0,  0,  0, -5,   0,0,0,0,0,0,0,0,
     -5,  0,  0,  0,  0,  0,  0, -5,   0,0,0,0,0,0,0,0,
     -5,  0,  0,  0,  0,  0,  0, -5,   0,0,0,0,0,0,0,0,
      5, 10, 10, 10, 10, 10, 10,  5,   0,0,0,0,0,0,0,0,
      0,  0,  0,  0,  0,  0,  0,  0,   0,0,0,0,0,0,0,0
};

const int queenTable[128] = {
    -20,-10,-10, -5, -5,-10,-10,-20,   0,0,0,0,0,0,0,0,
    -10,  0,  0,  0,  0,  0,  0,-10,   0,0,0,0,0,0,0,0,
    -10,  0,  5,  5,  5,  5,  0,-10,   0,0,0,0,0,0,0,0,
     -5,  0,  5,  5,  5,  5,  0, -5,   0,0,0,0,0,0,0,0,
      0,  0,  5,  5,  5,  5,  0, -5,   0,0,0,0,0,0,0,0,
    -10,  5,  5,  5,  5,  5,  0,-10,   0,0,0,0,0,0,0,0,
    -10,  0,  5,  0,  0,  0,  0,-10,   0,0,0,0,0,0,0,0,
    -20,-10,-10, -5, -5,-10,-10,-20,   0,0,0,0,0,0,0,0
};

const int kingTable[128] = {
     20, 30, 10,  0,  0, 10, 30, 20,   0,0,0,0,0,0,0,0,
     20, 20,  0,  0,  0,  0, 20, 20,   0,0,0,0,0,0,0,0,
    -10,-20,-20,-20,-20,-20,-20,-10,   0,0,0,0,0,0,0,0,
    -20,-30,-30,-40,-40,-30,-30,-20,   0,0,0,0,0,0,0,0,
    -30,-40,-40,-50,-50,-40,-40,-30,   0,0,0,0,0,0,0,0,
    -30,-40,-40,-50,-50,-40,-40,-30,   0,0,0,0,0,0,0,0,
    -30,-40,-40,-50,-50,-40,-40,-30,   0,0,0,0,0,0,0,0,
    -30,-40,-40,-50,-50,-40,-40,-30,   0,0,0,0,0,0,0,0
};

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
        if (castlingPart.find('K') != string::npos) castleWK = 1;
        if (castlingPart.find('Q') != string::npos) castleWQ = 1;
        if (castlingPart.find('k') != string::npos) castleBK = 1;
        if (castlingPart.find('q') != string::npos) castleBQ = 1;
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
        for (int file = 0; file < 8; file++) cout << getPieceChar(board[rank * 16 + file]) << " ";
        cout << rank + 1 << "\n";
    }
    cout << "\n  a b c d e f g h\n\n";
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
    int pieceType = piece & 7;

    if (pieceType == KING && abs(tgt - src) == 2) {
        if (tgt == src + 2) { 
            board[tgt - 1] = board[src + 3];
            board[src + 3] = EMPTY;
        } else if (tgt == src - 2) { 
            board[tgt + 1] = board[src - 4];
            board[src - 4] = EMPTY;
        }
    }

    if (pieceType == PAWN && tgt == enPassantSquare) {
        if (sideToMove == WHITE) board[tgt - 16] = EMPTY;
        else board[tgt + 16] = EMPTY;
    }

    enPassantSquare = -1;

    if (pieceType == PAWN && abs(tgt - src) == 32) {
        enPassantSquare = (sideToMove == WHITE) ? src + 16 : src - 16;
    }

    if (pieceType == KING) {
        if (sideToMove == WHITE) { castleWK = 0; castleWQ = 0; }
        else { castleBK = 0; castleBQ = 0; }
    }
    if (pieceType == ROOK) {
        if (src == 7) castleWK = 0;
        else if (src == 0) castleWQ = 0;
        else if (src == 119) castleBK = 0;
        else if (src == 112) castleBQ = 0;
    }
    if (tgt == 7) castleWK = 0;
    else if (tgt == 0) castleWQ = 0;
    else if (tgt == 119) castleBK = 0;
    else if (tgt == 112) castleBQ = 0;

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

int evaluate() {
    int score = 0;
    for (int square = 0; square < 128; square++) {
        if (square & 0x88) continue; 
        int piece = board[square];
        if (piece == EMPTY) continue;

        int pieceType = piece & 7;
        int pieceColor = piece & (WHITE | BLACK);
        int value = 0;

        int pstSquare = (pieceColor == WHITE) ? square : square ^ 0x70;

        if (pieceType == PAWN) value = 100 + pawnTable[pstSquare];
        else if (pieceType == KNIGHT) value = 300 + knightTable[pstSquare];
        else if (pieceType == BISHOP) value = 300 + bishopTable[pstSquare]; 
        else if (pieceType == ROOK) value = 500 + rookTable[pstSquare];
        else if (pieceType == QUEEN) value = 900 + queenTable[pstSquare];
        else if (pieceType == KING) value = 10000 + kingTable[pstSquare]; 

        if (pieceColor == WHITE) score += value;
        else score -= value;
    }
    return score;
}

bool isSquareAttacked(int square, int attackerColor) {
    if (attackerColor == WHITE) {
        if (!((square - 17) & 0x88) && board[square - 17] == (WHITE | PAWN)) return true;
        if (!((square - 15) & 0x88) && board[square - 15] == (WHITE | PAWN)) return true;
    } else {
        if (!((square + 17) & 0x88) && board[square + 17] == (BLACK | PAWN)) return true;
        if (!((square + 15) & 0x88) && board[square + 15] == (BLACK | PAWN)) return true;
    }

    for (int i = 0; i < 8; i++) {
        int target = square + knightOffsets[i];
        if (!(target & 0x88) && board[target] == (attackerColor | KNIGHT)) return true;
    }

    for (int i = 0; i < 8; i++) {
        int target = square + kingOffsets[i];
        if (!(target & 0x88) && board[target] == (attackerColor | KING)) return true;
    }

    for (int i = 0; i < 4; i++) {
        int target = square + bishopOffsets[i];
        while (!(target & 0x88)) {
            int piece = board[target];
            if (piece != EMPTY) {
                if (piece == (attackerColor | BISHOP) || piece == (attackerColor | QUEEN)) return true;
                break; 
            }
            target += bishopOffsets[i];
        }
    }

    for (int i = 0; i < 4; i++) {
        int target = square + rookOffsets[i];
        while (!(target & 0x88)) {
            int piece = board[target];
            if (piece != EMPTY) {
                if (piece == (attackerColor | ROOK) || piece == (attackerColor | QUEEN)) return true;
                break; 
            }
            target += rookOffsets[i];
        }
    }
    return false;
}

vector<string> generateMoves() {
    vector<string> moves;
    for (int square = 0; square < 128; square++) {
        if (square & 0x88) continue;
        int piece = board[square];
        if (piece == EMPTY) continue;
        int pieceColor = piece & (WHITE | BLACK);
        if (pieceColor != sideToMove) continue;
        int pieceType = piece & 7;

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
            
            if (pieceType == KING) {
                if (sideToMove == WHITE && square == 4) { 
                    if (castleWK && board[5] == EMPTY && board[6] == EMPTY) {
                        if (!isSquareAttacked(4, BLACK) && !isSquareAttacked(5, BLACK) && !isSquareAttacked(6, BLACK)) addMove(6); 
                    }
                    if (castleWQ && board[3] == EMPTY && board[2] == EMPTY && board[1] == EMPTY) {
                        if (!isSquareAttacked(4, BLACK) && !isSquareAttacked(3, BLACK) && !isSquareAttacked(2, BLACK)) addMove(2); 
                    }
                } else if (sideToMove == BLACK && square == 116) { 
                    if (castleBK && board[117] == EMPTY && board[118] == EMPTY) {
                        if (!isSquareAttacked(116, WHITE) && !isSquareAttacked(117, WHITE) && !isSquareAttacked(118, WHITE)) addMove(118); 
                    }
                    if (castleBQ && board[115] == EMPTY && board[114] == EMPTY && board[113] == EMPTY) {
                        if (!isSquareAttacked(116, WHITE) && !isSquareAttacked(115, WHITE) && !isSquareAttacked(114, WHITE)) addMove(114); 
                    }
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

int search(int depth, int alpha, int beta, bool isMaximizing) {
    // Throttle the time check so we don't slow down the engine by checking the clock every single microsecond
    if ((nodesSearched++ & 2047) == 0) checkTime();
    if (timeIsUp) return 0; // Emergency abort

    if (depth == 0) return evaluate();

    vector<string> moves = generateMoves();
    int legalMovesCount = 0;
    int bestScore = isMaximizing ? -100000 : 100000;

    for (string move : moves) {
        int backupBoard[128];
        for(int i=0; i<128; i++) backupBoard[i] = board[i];
        int backupSide = sideToMove;
        int backupEP = enPassantSquare;
        int backupCWK = castleWK, backupCWQ = castleWQ, backupCBK = castleBK, backupCBQ = castleBQ;

        makeMove(move);

        int kingSide = backupSide;
        int kingSq = -1;
        for (int i = 0; i < 128; i++) {
            if (!(i & 0x88) && board[i] == (kingSide | KING)) { kingSq = i; break; }
        }

        if (isSquareAttacked(kingSq, kingSide == WHITE ? BLACK : WHITE)) {
            for(int i=0; i<128; i++) board[i] = backupBoard[i];
            sideToMove = backupSide;
            enPassantSquare = backupEP;
            castleWK = backupCWK; castleWQ = backupCWQ; castleBK = backupCBK; castleBQ = backupCBQ;
            continue;
        }

        legalMovesCount++;
        int score = search(depth - 1, alpha, beta, !isMaximizing);

        for(int i=0; i<128; i++) board[i] = backupBoard[i];
        sideToMove = backupSide;
        enPassantSquare = backupEP;
        castleWK = backupCWK; castleWQ = backupCWQ; castleBK = backupCBK; castleBQ = backupCBQ;

        if (timeIsUp) return 0; // Abandon ship if time ran out mid-branch

        if (isMaximizing) {
            if (score > bestScore) bestScore = score;
            if (bestScore > alpha) alpha = bestScore;
        } else {
            if (score < bestScore) bestScore = score;
            if (bestScore < beta) beta = bestScore;
        }

        if (beta <= alpha) break; 
    }

    if (legalMovesCount == 0) {
        int kingSq = -1;
        for (int i = 0; i < 128; i++) {
            if (!(i & 0x88) && board[i] == (sideToMove | KING)) { kingSq = i; break; }
        }
        if (isSquareAttacked(kingSq, sideToMove == WHITE ? BLACK : WHITE)) 
            return isMaximizing ? -10000 - depth : 10000 + depth; 
        else 
            return 0; 
    }

    return bestScore;
}

string getBestMove() {
    string globalBestMove = "0000";
    int maxDepth = 64; // The theoretical maximum it will try to reach

    // ITERATIVE DEEPENING: Search depth 1, then depth 2, then depth 3... until time is up
    for (int depth = 1; depth <= maxDepth; depth++) {
        string currentDepthBestMove = "0000";
        bool isMaximizing = (sideToMove == WHITE);
        int bestScore = isMaximizing ? -100000 : 100000;
        int alpha = -100000;
        int beta = 100000;

        vector<string> moves = generateMoves();

        for (string move : moves) {
            int backupBoard[128];
            for(int i=0; i<128; i++) backupBoard[i] = board[i];
            int backupSide = sideToMove;
            int backupEP = enPassantSquare;
            int backupCWK = castleWK, backupCWQ = castleWQ, backupCBK = castleBK, backupCBQ = castleBQ;

            makeMove(move);

            int kingSq = -1;
            for (int i = 0; i < 128; i++) {
                if (!(i & 0x88) && board[i] == (backupSide | KING)) { kingSq = i; break; }
            }

            if (isSquareAttacked(kingSq, backupSide == WHITE ? BLACK : WHITE)) {
                for(int i=0; i<128; i++) board[i] = backupBoard[i];
                sideToMove = backupSide;
                enPassantSquare = backupEP;
                castleWK = backupCWK; castleWQ = backupCWQ; castleBK = backupCBK; castleBQ = backupCBQ;
                continue;
            }

            if (currentDepthBestMove == "0000") currentDepthBestMove = move; 

            int score = search(depth - 1, alpha, beta, !isMaximizing);

            for(int i=0; i<128; i++) board[i] = backupBoard[i];
            sideToMove = backupSide;
            enPassantSquare = backupEP;
            castleWK = backupCWK; castleWQ = backupCWQ; castleBK = backupCBK; castleBQ = backupCBQ;

            if (timeIsUp) break; // If time ran out, this score is garbage. Bail out.

            if (isMaximizing) {
                if (score > bestScore) { bestScore = score; currentDepthBestMove = move; }
                if (bestScore > alpha) alpha = bestScore;
            } else {
                if (score < bestScore) { bestScore = score; currentDepthBestMove = move; }
                if (bestScore < beta) beta = bestScore;
            }
        }

        if (timeIsUp) break; // Keep the best move from the PREVIOUS fully completed depth
        
        globalBestMove = currentDepthBestMove;

        // If we found a forced mate, no need to keep searching deeper
        if (bestScore > 9000 || bestScore < -9000) break;
    }
    
    return globalBestMove;
}

long perft(int depth) {
    if (depth == 0) return 1;

    long nodes = 0;
    vector<string> moves = generateMoves();

    for (string move : moves) {
        int backupBoard[128];
        for(int i=0; i<128; i++) backupBoard[i] = board[i];
        int backupSide = sideToMove;
        int backupEP = enPassantSquare;
        int backupCWK = castleWK, backupCWQ = castleWQ, backupCBK = castleBK, backupCBQ = castleBQ;

        makeMove(move);

        int kingSq = -1;
        for (int i = 0; i < 128; i++) {
            if (!(i & 0x88) && board[i] == (backupSide | KING)) { kingSq = i; break; }
        }
        
        if (!isSquareAttacked(kingSq, backupSide == WHITE ? BLACK : WHITE)) {
            nodes += perft(depth - 1);
        }

        for(int i=0; i<128; i++) board[i] = backupBoard[i];
        sideToMove = backupSide;
        enPassantSquare = backupEP;
        castleWK = backupCWK; castleWQ = backupCWQ; castleBK = backupCBK; castleBQ = backupCBQ;
    }
    return nodes;
}

void uciLoop() {
    string line;
    srand(time(0)); 

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
            // --- TIME PARSING ---
            int wtime = 0, btime = 0, movetime = -1;
            
            size_t wpos = line.find("wtime ");
            if (wpos != string::npos) wtime = stoi(line.substr(wpos + 6));
            
            size_t bpos = line.find("btime ");
            if (bpos != string::npos) btime = stoi(line.substr(bpos + 6));
            
            size_t mpos = line.find("movetime ");
            if (mpos != string::npos) movetime = stoi(line.substr(mpos + 9));

            // Calculate how much time we are allowed to spend on this specific move
            if (movetime != -1) {
                allocatedTimeMs = movetime - 50; // Give a 50ms buffer so we don't lag out
            } else {
                int timeLeft = (sideToMove == WHITE) ? wtime : btime;
                allocatedTimeMs = (timeLeft / 30) - 50; // Basic rule: expect the game to last ~30 more moves
                if (allocatedTimeMs < 100) allocatedTimeMs = 100; // Never spend less than 0.1s
            }

            startTimeMs = getTimeMs();
            timeIsUp = false;
            nodesSearched = 0;

            // We don't pass depth anymore, the clock controls it!
            string bestMove = getBestMove();
            cout << "bestmove " << bestMove << "\n";
        }
        else if (line == "d") {
            printBoard();
        }
        else if (line == "eval") {
            int score = evaluate();
            cout << "Current evaluation: " << score << " centipawns\n";
        }
        else if (line.substr(0, 5) == "perft") {
            int depth = stoi(line.substr(6));
            long start = clock();
            long nodes = perft(depth);
            long end = clock();
            double time_taken = double(end - start) / CLOCKS_PER_SEC;
            cout << "Nodes searched: " << nodes << endl;
            cout << "Time taken: " << time_taken << "s" << endl;
        }
        else if (line == "quit") {
            break; 
        }
    }
}

int main() {
    setvbuf(stdout, NULL, _IONBF, 0); 
    uciLoop();
    return 0;
}