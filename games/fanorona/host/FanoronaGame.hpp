#pragma once

#include <gamehub/core/Game.hpp>
#include <array>
#include <vector>
#include <string>
#include <memory>
#include <unordered_set>

namespace gamehub {
namespace core {

/**
 * @brief Represents an explicit directed connection in the Fanorona board graph.
 */
struct FanoronaEdge {
    int to{0};
    int dr{0};
    int dc{0};

    bool operator==(const FanoronaEdge& other) const {
        return to == other.to && dr == other.dr && dc == other.dc;
    }
};

/**
 * @brief Metadata about the last executed move for smooth client animation.
 */
struct FanoronaLastMove {
    int id{0};
    int player{0};
    int from{-1};
    int to{-1};
    std::string captureType;
    std::vector<int> captured;
};

/**
 * @brief Authoritative headless engine for Fanorona Malagasy (Fanoron-tsivy 5x9).
 */
class FanoronaGame : public Game {
public:
    static constexpr int ROWS = 5;
    static constexpr int COLS = 9;
    static constexpr int NUM_INTERSECTIONS = 45;
    static constexpr int CENTER_NODE = 22; // row 2, col 4

    explicit FanoronaGame(bool isSolo = false, int startingPlayer = 2);
    ~FanoronaGame() override = default;

    // Game interface
    void init() override;
    void update(float dt) override;
    void handleInput(PlayerId id, const std::string& commandJson) override;
    std::string serializeState() const override;
    bool isFinished() const override { return m_finished; }
    int getWinner() const override { return m_winner; }
    void setPlayerCount(int count) override;

    // Topology & Graph
    static bool isStrongNode(int node);
    static bool isStrongNode(int r, int c);
    const std::vector<FanoronaEdge>& getEdges(int node) const;
    bool isConnected(int from, int to) const;
    FanoronaEdge getEdge(int from, int to) const;

    // Board & State accessors
    const std::array<int, NUM_INTERSECTIONS>& getBoard() const { return m_board; }
    int getPieceAt(int node) const;
    int getPieceAt(int r, int c) const;
    void setPieceAt(int node, int piece) { if (node >= 0 && node < NUM_INTERSECTIONS) m_board[node] = piece; }
    int getTurn() const { return m_turn; }
    void setTurn(int turn) { m_turn = turn; }
    int getStartingPlayer() const { return m_startingPlayer; }
    void setStartingPlayer(int player);
    const std::string& getWinReason() const { return m_winReason; }
    const std::string& getLastAction() const { return m_lastAction; }

    // Multi-capture state
    bool isInMultiCapture() const { return m_inMultiCapture; }
    int getActivePiece() const { return m_activePiece; }
    const std::vector<int>& getVisitedPositions() const { return m_visitedPositions; }
    std::pair<int, int> getLastDirection() const { return m_lastDirection; }

    // Captures (Rule 3.4 & 3.5: separate testable mechanisms)
    std::vector<int> getApproachCaptures(int from, int to) const;
    std::vector<int> getWithdrawalCaptures(int from, int to) const;

    // Move legality & rules
    bool hasAnyLegalCaptures(int player) const;
    bool hasAnyLegalMoves(int player) const;
    bool canMakeMove(int player, int from, int to, std::string* error = nullptr) const;

    // Successive captures check for an active piece
    bool canContinueCaptureFrom(int node, const std::pair<int, int>& lastDir, const std::vector<int>& visited) const;
    std::vector<int> getValidSuccessiveMoves(int node) const;

    // Core movement execution
    bool executeMove(int player, int from, int to, const std::string& captureType = "");

    // Victory conditions (Rule 3.9)
    bool checkAllStonesCaptured(int& winner, std::string& reason) const;
    bool checkImmobilization(int player, int& winner, std::string& reason) const;
    void checkGameState();

    // Solo bot execution
    void executeBotTurn();
    bool executeBotStep();

    // Last move info for animation
    const FanoronaLastMove& getLastMove() const { return m_lastMove; }

private:
    void initGraph();
    void resetBoard();

    bool m_isSolo{false};
    int m_startingPlayer{2}; // Player 2 (claires / White) starts traditionally
    int m_turn{2};
    bool m_finished{false};
    int m_winner{0}; // 0 = ongoing, 1 = p1 win, 2 = p2 win, -1 = draw
    std::string m_winReason;
    std::string m_lastAction;

    // Move tracking & animation metadata
    int m_moveCounter{0};
    FanoronaLastMove m_lastMove;

    // 45 intersections: 0 = empty, 1 = Player 1 (foncées), 2 = Player 2 (claires)
    std::array<int, NUM_INTERSECTIONS> m_board{0};

    // Explicit graph of sommets et connexions
    std::array<std::vector<FanoronaEdge>, NUM_INTERSECTIONS> m_adj;

    // Successive captures tracking
    bool m_inMultiCapture{false};
    int m_activePiece{-1};
    std::vector<int> m_visitedPositions;
    std::pair<int, int> m_lastDirection{0, 0};
};

} // namespace core
} // namespace gamehub
