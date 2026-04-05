# Chess Engine in C++ (0x88 + UCI)

A chess engine built from scratch in C++ to understand how engines work internally — from board representation to search algorithms.

The engine uses the classic 0x88 board representation, supports full move generation, and communicates with GUI software like Arena via the UCI protocol.

---

##  Features

- 0x88 board representation
- FEN parsing (load any position)
- Full move generation:
  - Pawns (push, capture, promotion, en passant)
  - Knights, Bishops, Rooks, Queens, King
- UCI protocol support (works with Arena Chess GUI)
- Random move generator → upgraded to Minimax-based decision making
- Evaluation function:
  - Material count
  - Piece-square tables (positional scoring)
- Plays complete games via GUI

---

##  Engine Logic

### Move Generation
Efficient move generation using:
- Offset-based movement (for leapers and sliders)
- 0x88 boundary checks for fast validation

### Evaluation Function
- Material-based scoring (centipawns)
- Positional bonuses using piece-square tables
- Positive score → White advantage  
- Negative score → Black advantage  

### Search (Minimax)
- Recursive game tree exploration
- Simulates future positions up to a fixed depth
- Chooses the move with the best evaluated outcome

---

##  Demo

Engine connected to Arena GUI via UCI:

<img width="960" height="496" alt="image" src="https://github.com/user-attachments/assets/758f5195-6146-4971-b8fc-71d14412674a" />


---

##  How to Run

### Compile
```bash
g++ main.cpp -o engine
./engine
```
##  Use with Arena GUI

1. Open Arena Chess GUI
2. Go to **Engines → Install New Engine**
3. Select your compiled engine executable (`engine.exe` or `./engine`)
4. Choose UCI protocol when prompted
5. Start a new game
