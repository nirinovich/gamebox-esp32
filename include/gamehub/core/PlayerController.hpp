#pragma once

#include <cstdint>
#include <string>
#include <array>
#include "GameTypes.hpp"

namespace gamehub {
namespace core {

class PlayerController {
public:
    virtual ~PlayerController() = default;
    virtual int getMove(const std::array<int, 9>& board, int playerMark) = 0;
};

class HumanController : public PlayerController {
public:
    int getMove(const std::array<int, 9>& /*board*/, int /*playerMark*/) override {
        // Human moves are supplied asynchronously via handleInput
        return -1;
    }
};

class BotController : public PlayerController {
public:
    int getMove(const std::array<int, 9>& board, int playerMark) override {
        return findBestMove(board, playerMark);
    }

    static int findBestMove(std::array<int, 9> board, int botMark) {
        int opponentMark = (botMark == 1) ? 2 : 1;
        int bestScore = -1000;
        int bestMove = -1;

        for (int i = 0; i < 9; ++i) {
            if (board[i] == 0) {
                board[i] = botMark;
                int score = minimax(board, 0, false, botMark, opponentMark);
                board[i] = 0;
                if (score > bestScore) {
                    bestScore = score;
                    bestMove = i;
                }
            }
        }
        return bestMove;
    }

private:
    static int evaluate(const std::array<int, 9>& b, int botMark, int opponentMark) {
        // Rows, Columns, Diagonals
        const int wins[8][3] = {
            {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, // rows
            {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, // cols
            {0, 4, 8}, {2, 4, 6}             // diags
        };

        for (auto& w : wins) {
            if (b[w[0]] != 0 && b[w[0]] == b[w[1]] && b[w[1]] == b[w[2]]) {
                if (b[w[0]] == botMark) return +10;
                else if (b[w[0]] == opponentMark) return -10;
            }
        }
        return 0;
    }

    static bool isMovesLeft(const std::array<int, 9>& b) {
        for (int i = 0; i < 9; ++i) if (b[i] == 0) return true;
        return false;
    }

    static int minimax(std::array<int, 9>& b, int depth, bool isMax, int botMark, int opponentMark) {
        int score = evaluate(b, botMark, opponentMark);
        if (score == 10) return score - depth;
        if (score == -10) return score + depth;
        if (!isMovesLeft(b)) return 0;

        if (isMax) {
            int best = -1000;
            for (int i = 0; i < 9; ++i) {
                if (b[i] == 0) {
                    b[i] = botMark;
                    best = (std::max)(best, minimax(b, depth + 1, !isMax, botMark, opponentMark));
                    b[i] = 0;
                }
            }
            return best;
        } else {
            int best = 1000;
            for (int i = 0; i < 9; ++i) {
                if (b[i] == 0) {
                    b[i] = opponentMark;
                    best = (std::min)(best, minimax(b, depth + 1, !isMax, botMark, opponentMark));
                    b[i] = 0;
                }
            }
            return best;
        }
    }
};

} // namespace core
} // namespace gamehub
