# Issue 04: Polymorphic Bot Strategy Hierarchy Decoupling

Status: closed

## Description
Refactor AI bot algorithms out of concrete game classes and decouple `PlayerController` from TicTacToe. Implement a clean, game-agnostic or polymorphic `IBotStrategy` hierarchy adhering to the GoF Strategy pattern and Open-Closed Principle (OCP).

## Acceptance Criteria
1. `IBotStrategy` interfaces established for bot-enabled games (Connect Four Minimax, Tron Voronoi, Pong Kinematics, Battleship Parity, Domino Heuristic).
2. Concrete bot algorithms are decoupled from the game model classes and injected via dependency injection.
3. Unit tests verify that strategies evaluate legal moves given state representations.
