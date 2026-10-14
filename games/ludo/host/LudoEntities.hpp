#pragma once

#include <string>
#include <array>
#include <random>

namespace gamehub {
namespace core {

enum class PawnState {
    IN_YARD = 0,
    ON_TRACK = 1,
    IN_HOME = 2,
    FINISHED = 3
};

/**
 * @brief Domain entity representing an individual Ludo token/pawn.
 */
class Pawn {
public:
    int id{0};
    int step{-1}; // -1: Yard, 0..50: Track, 51..55: Home stretch, 56: Finished
    PawnState state{PawnState::IN_YARD};

    int getTrackCell(int startTile) const {
        if (state == PawnState::ON_TRACK && step >= 0 && step <= 50) {
            return (startTile + step) % 52;
        }
        return -1;
    }

    bool canMove(int dice) const {
        if (state == PawnState::FINISHED) return false;
        if (state == PawnState::IN_YARD) return (dice == 6);
        return (step + dice <= 56);
    }

    void rollout() {
        state = PawnState::ON_TRACK;
        step = 0;
    }

    void returnToYard() {
        state = PawnState::IN_YARD;
        step = -1;
    }

    void advance(int dice) {
        if (state == PawnState::IN_YARD && dice == 6) {
            rollout();
            return;
        }
        step += dice;
        if (step == 56) {
            state = PawnState::FINISHED;
        } else if (step > 50) {
            state = PawnState::IN_HOME;
        }
    }
};

using LudoPawn = Pawn; // Backward compatibility alias

/**
 * @brief Domain entity encapsulating the six-sided die.
 */
class Dice {
public:
    explicit Dice(unsigned int seed = 0) {
        if (seed == 0) {
            std::random_device rd;
            m_rng.seed(rd());
        } else {
            m_rng.seed(seed);
        }
    }

    int roll() {
        std::uniform_int_distribution<int> dist(1, 6);
        m_value = dist(m_rng);
        m_rolled = true;
        return m_value;
    }

    void force(int val) {
        m_value = val;
        m_rolled = true;
    }

    void reset() {
        m_value = 0;
        m_rolled = false;
    }

    int getValue() const { return m_value; }
    bool isRolled() const { return m_rolled; }

private:
    int m_value{0};
    bool m_rolled{false};
    std::mt19937 m_rng;
};

/**
 * @brief Domain entity defining track geography, star safe squares, and player start tiles.
 */
class LudoBoard {
public:
    static bool isSafeSquare(int trackCell) {
        static constexpr int SAFE_SQUARES[] = {0, 8, 13, 21, 26, 34, 39, 47};
        for (int s : SAFE_SQUARES) {
            if (trackCell == s) return true;
        }
        return false;
    }

    static int getStartTileForPlayer(int playerIdx) {
        static constexpr int START_TILES[] = {0, 13, 26, 39};
        if (playerIdx >= 0 && playerIdx < 4) {
            return START_TILES[playerIdx];
        }
        return 0;
    }
};

struct LudoPlayer {
    int id{1};
    std::string name{"Player"};
    std::string color{"red"};
    bool isBot{false};
    int startTile{0};
    std::array<Pawn, 4> pawns{};
    int finishedCount{0};
};

} // namespace core
} // namespace gamehub
