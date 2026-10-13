#pragma once

#include <gamehub/core/Game.hpp>
#include <array>
#include <string>
#include <cstdint>

namespace gamehub {
namespace core {

enum class PawnState {
    IN_YARD = 0,
    ON_TRACK = 1,
    IN_HOME = 2,
    FINISHED = 3
};

struct LudoPawn {
    int id{0};
    int step{-1}; // -1: Yard, 0..50: Track, 51..55: Home stretch, 56: Finished
    PawnState state{PawnState::IN_YARD};

    int getTrackCell(int startTile) const {
        if (state == PawnState::ON_TRACK && step >= 0 && step <= 50) {
            return (startTile + step) % 52;
        }
        return -1;
    }
};

struct LudoPlayer {
    int id{1};
    std::string name{"Player"};
    std::string color{"red"};
    bool isBot{false};
    int startTile{0};
    std::array<LudoPawn, 4> pawns{};
    int finishedCount{0};
};

class LudoGame : public Game {
public:
    explicit LudoGame(bool isSolo = false, int numPlayers = 4, bool isQuick = true);
    ~LudoGame() override = default;

    void init() override;
    void update(float dt) override;
    void handleInput(PlayerId player, const std::string& input) override;
    std::string serializeState() const override;
    std::string serializeStateForPlayer(PlayerId id) const override;
    bool isFinished() const override { return m_finished; }
    int getWinner() const override { return m_winner; }
    void setPlayerCount(int count) override;
    bool isRealTime() const override { return true; }

    static bool isSafeSquare(int trackCell);

    // Testing helpers & state setters
    void setPawnStepForTesting(int playerIdx, int pawnIdx, int step);
    void setDiceForTesting(int dice) { m_currentDice = dice; m_diceRolled = true; }
    void rollDiceForTesting(int forcedRoll);
    void setCurrentTurnForTesting(int seatId) { m_currentTurn = seatId; }
    void setConsecutiveSixesForTesting(int c) { m_consecutiveSixes = c; }
    void setTurnTimerForTesting(float t) { m_turnTimer = t; }
    void setBotDelay(float d) { m_botDelay = d; }
    void setQuickMode(bool q);

    int getCurrentDice() const { return m_currentDice; }
    bool isDiceRolled() const { return m_diceRolled; }
    int getCurrentTurn() const { return m_currentTurn; }
    int getConsecutiveSixes() const { return m_consecutiveSixes; }
    int getPawnCount() const { return m_pawnCount; }
    bool isQuick() const { return m_isQuick; }
    const LudoPlayer& getPlayer(int idx) const { return m_players[idx]; }

    bool canPawnMove(int playerIdx, int pawnIdx, int dice) const;
    int getLegalMovesCount(int playerIdx, int dice) const;

private:
    void rollDice(int forcedRoll = 0);
    bool executeMove(int playerIdx, int pawnIdx);
    void nextTurn();
    int pickBestBotPawn(int playerIdx, int dice) const;

    bool m_isSolo{false};
    int m_numPlayers{4};
    bool m_isQuick{true};
    int m_pawnCount{2}; // 2 for Quick, 4 for Classic

    int m_currentTurn{1}; // 1-based seat ID (1..4)
    int m_currentDice{0};
    bool m_diceRolled{false};
    int m_consecutiveSixes{0};

    float m_turnTimer{20.0f};
    float m_botTimer{0.5f};
    float m_botDelay{0.5f};

    bool m_finished{false};
    int m_winner{0};
    std::string m_lastAction{"Game started"};

    std::array<LudoPlayer, 4> m_players{};
};

} // namespace core
} // namespace gamehub
