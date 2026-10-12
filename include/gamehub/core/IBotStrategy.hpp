#pragma once

#include <array>
#include <vector>
#include <utility>
#include <cstdint>
#include <string>
#include <limits>
#include <algorithm>

namespace gamehub {
namespace core {

/**
 * @brief GoF Strategy Pattern interface for Connect Four bot decision-making.
 */
class IConnectFourStrategy {
public:
    virtual ~IConnectFourStrategy() = default;
    virtual int computeMove(const std::array<std::array<int, 6>, 7>& board, int botMark) = 0;
};

/**
 * @brief Concrete Minimax strategy adapter for Connect Four.
 */
class ConnectFourMinimaxStrategy : public IConnectFourStrategy {
public:
    static constexpr int COLS = 7;
    static constexpr int ROWS = 6;
    static constexpr int DEFAULT_DEPTH = 5;

    explicit ConnectFourMinimaxStrategy(int depth = DEFAULT_DEPTH) : m_depth(depth) {}

    int computeMove(const std::array<std::array<int, 6>, 7>& board, int botMark) override {
        int bestScore = std::numeric_limits<int>::min();
        int bestCol = 3; // Center preference
        auto tempBoard = board;

        for (int c = 0; c < COLS; ++c) {
            int r = -1;
            for (int row = 0; row < ROWS; ++row) {
                if (tempBoard[c][row] == 0) { r = row; break; }
            }
            if (r == -1) continue;

            tempBoard[c][r] = botMark;
            int score = minimax(tempBoard, m_depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), false, botMark);
            tempBoard[c][r] = 0;

            if (score > bestScore) {
                bestScore = score;
                bestCol = c;
            }
        }
        return bestCol;
    }

private:
    int m_depth;

    static bool checkWin(const std::array<std::array<int, 6>, 7>& b, int p) {
        // Horizontal
        for (int c = 0; c <= COLS-4; ++c)
            for (int r = 0; r < ROWS; ++r)
                if (b[c][r]==p && b[c+1][r]==p && b[c+2][r]==p && b[c+3][r]==p) return true;
        // Vertical
        for (int c = 0; c < COLS; ++c)
            for (int r = 0; r <= ROWS-4; ++r)
                if (b[c][r]==p && b[c][r+1]==p && b[c][r+2]==p && b[c][r+3]==p) return true;
        // Diagonal /
        for (int c = 0; c <= COLS-4; ++c)
            for (int r = 0; r <= ROWS-4; ++r)
                if (b[c][r]==p && b[c+1][r+1]==p && b[c+2][r+2]==p && b[c+3][r+3]==p) return true;
        // Diagonal backslash
        for (int c = 0; c <= COLS-4; ++c)
            for (int r = 3; r < ROWS; ++r)
                if (b[c][r]==p && b[c+1][r-1]==p && b[c+2][r-2]==p && b[c+3][r-3]==p) return true;
        return false;
    }

    static bool isBoardFull(const std::array<std::array<int, 6>, 7>& b) {
        for (int c = 0; c < COLS; ++c)
            if (b[c][ROWS-1] == 0) return false;
        return true;
    }

    static int scoreWindow(int a, int b, int c, int d, int p) {
        int opp = (p == 1) ? 2 : 1;
        int countP = (a==p) + (b==p) + (c==p) + (d==p);
        int countOpp = (a==opp) + (b==opp) + (c==opp) + (d==opp);
        int countEmpty = (a==0) + (b==0) + (c==0) + (d==0);

        if (countP == 4) return 100;
        if (countP == 3 && countEmpty == 1) return 5;
        if (countP == 2 && countEmpty == 2) return 2;
        if (countOpp == 3 && countEmpty == 1) return -4;
        return 0;
    }

    static int heuristicScore(const std::array<std::array<int, 6>, 7>& b, int p) {
        int score = 0;
        for (int r = 0; r < ROWS; ++r) if (b[3][r] == p) score += 3;
        for (int r = 0; r < ROWS; ++r)
            for (int c = 0; c <= COLS-4; ++c)
                score += scoreWindow(b[c][r], b[c+1][r], b[c+2][r], b[c+3][r], p);
        for (int c = 0; c < COLS; ++c)
            for (int r = 0; r <= ROWS-4; ++r)
                score += scoreWindow(b[c][r], b[c][r+1], b[c][r+2], b[c][r+3], p);
        for (int c = 0; c <= COLS-4; ++c)
            for (int r = 0; r <= ROWS-4; ++r)
                score += scoreWindow(b[c][r], b[c+1][r+1], b[c+2][r+2], b[c+3][r+3], p);
        for (int c = 0; c <= COLS-4; ++c)
            for (int r = 3; r < ROWS; ++r)
                score += scoreWindow(b[c][r], b[c+1][r-1], b[c+2][r-2], b[c+3][r-3], p);
        return score;
    }

    static int minimax(std::array<std::array<int, 6>, 7>& b, int depth, int alpha, int beta, bool maximizing, int botMark) {
        int humanMark = (botMark == 1) ? 2 : 1;
        if (checkWin(b, botMark)) return 1000 + depth;
        if (checkWin(b, humanMark)) return -(1000 + depth);
        if (isBoardFull(b) || depth == 0) return heuristicScore(b, botMark) - heuristicScore(b, humanMark);

        if (maximizing) {
            int best = std::numeric_limits<int>::min();
            for (int c = 0; c < COLS; ++c) {
                int r = -1;
                for (int row = 0; row < ROWS; ++row) if (b[c][row] == 0) { r = row; break; }
                if (r == -1) continue;
                b[c][r] = botMark;
                best = (std::max)(best, minimax(b, depth-1, alpha, beta, false, botMark));
                b[c][r] = 0;
                alpha = (std::max)(alpha, best);
                if (alpha >= beta) break;
            }
            return best;
        } else {
            int best = std::numeric_limits<int>::max();
            for (int c = 0; c < COLS; ++c) {
                int r = -1;
                for (int row = 0; row < ROWS; ++row) if (b[c][row] == 0) { r = row; break; }
                if (r == -1) continue;
                b[c][r] = humanMark;
                best = (std::min)(best, minimax(b, depth-1, alpha, beta, true, botMark));
                b[c][r] = 0;
                beta = (std::min)(beta, best);
                if (alpha >= beta) break;
            }
            return best;
        }
    }
};

/**
 * @brief GoF Strategy Pattern interface for Tron light cycle bot decision-making.
 */
class ITronStrategy {
public:
    virtual ~ITronStrategy() = default;
    virtual std::pair<int, int> computeMove(const std::array<int8_t, 900>& grid, int bx, int by, int curDx, int curDy) = 0;
};

/**
 * @brief GoF Strategy Pattern interface for Pong kinematic bot decision-making.
 */
class IPongStrategy {
public:
    virtual ~IPongStrategy() = default;
    virtual float computeTargetY(float bx, float by, float bvx, float bvy, float paddleX, float courtHeight) = 0;
};

/**
 * @brief GoF Strategy Pattern interface for Battleship targeting strategy.
 */
class IBattleshipStrategy {
public:
    virtual ~IBattleshipStrategy() = default;
    virtual std::pair<int, int> computeShot(const std::array<std::array<bool, 10>, 10>& shotsFired,
                                            std::vector<std::pair<int, int>>& targetQueue) = 0;
};

/**
 * @brief GoF Strategy Pattern interface for Domino bot tile selection.
 */
class IDominoStrategy {
public:
    virtual ~IDominoStrategy() = default;
    virtual int chooseTile(const std::vector<std::pair<int, int>>& hand, int leftEnd, int rightEnd, bool& playAtRight) = 0;
};

} // namespace core
} // namespace gamehub
