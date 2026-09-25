# 03: Turn-Based Game & Bot Controller Strategy (Solo Tic-Tac-Toe vs Minimax)

**What to build:** The `PlayerController` strategy pattern and turn-based game engine hosting Solo Tic-Tac-Toe against an unbeatable Minimax `BotController`. Selecting "Play Solo Tic-Tac-Toe" initializes a 3x3 board where tapping empty cells dispatches moves to `HumanController`, triggers the Host's automated bot response, evaluates three-in-a-row or draw outcomes, and syncs the board state to the client.

**Blocked by:** 02 (Core Polymorphic Game Architecture & Solo Snake)

**Status:** resolved

- [x] `PlayerController` strategy interface decoupling input ingestion from game logic.
- [x] `HumanController` consumes moves received over WebSocket; `BotController` executes optimal Minimax decision algorithm.
- [x] `TicTacToeGame` implements turn alternation, move validation, win evaluation (rows, cols, diagonals), and draw detection.
- [x] Client interface presents a clickable 3x3 grid showing X and O symbols and announces win, loss, or draw.
- [x] Tests at the Room seam verify move alternation, win conditions, and that `BotController` never loses a game.
