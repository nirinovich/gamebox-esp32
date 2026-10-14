#pragma once

#include <gamehub/core/Game.hpp>
#include <gamehub/core/Player.hpp>
#include <gamehub/core/PlayerController.hpp>
#include <gamehub/core/IGamePhaseState.hpp>
#include <array>
#include <vector>
#include <string>
#include <memory>
#include <cstdint>

#include "LudoEntities.hpp"
#include "LudoCommands.hpp"
#include "LudoStates.hpp"
#include "LudoBotController.hpp"

namespace gamehub {
namespace core {

class LudoGame : public Game {
public:
    explicit LudoGame(bool isSolo = false, int numPlayers = 4, bool isQuick = true);
    ~LudoGame() override = default;

    // --- Core Game Interface ---
    void init() override;
    void update(float dt) override;
    void handleInput(PlayerId player, const std::string& input) override;
    std::string serializeState() const override;
    std::string serializeStateForPlayer(PlayerId id) const override;
    bool isFinished() const override { return m_finished; }
    int getWinner() const override { return m_winner; }
    void setPlayerCount(int count) override;
    bool isRealTime() const override { return true; }

    // --- State & Command Dispatch Seams ---
    void transitionTo(std::unique_ptr<IGamePhaseState> nextState);
    IGamePhaseState* getActiveState() const { return m_state.get(); }
    bool processCommand(PlayerId player, const ICommand& cmd);

    static bool isSafeSquare(int trackCell);

    // Testing helpers & state setters
    void setPawnStepForTesting(int playerIdx, int pawnIdx, int step);
    void setDiceForTesting(int dice);
    void rollDiceForTesting(int forcedRoll);
    void setCurrentTurnForTesting(int seatId);
    void setConsecutiveSixesForTesting(int c) { m_consecutiveSixes = c; }
    void setTurnTimerForTesting(float t) { m_turnTimer = t; }
    void setBotDelay(float d) { m_botDelay = d; }
    void setQuickMode(bool q);

    int getCurrentDice() const { return m_dice.getValue(); }
    bool isDiceRolled() const { return m_dice.isRolled(); }
    int getCurrentTurn() const { return m_currentTurn; }
    void setCurrentTurn(int turn) { m_currentTurn = turn; }
    int getConsecutiveSixes() const { return m_consecutiveSixes; }
    void setConsecutiveSixes(int c) { m_consecutiveSixes = c; }
    int getPawnCount() const { return m_pawnCount; }
    bool isQuick() const { return m_isQuick; }
    int getNumPlayers() const { return m_numPlayers; }

    const LudoPlayer& getPlayer(int idx) const { return m_players[idx]; }
    LudoPlayer& getPlayer(int idx) { return m_players[idx]; }

    Player* getPlayerEntity(int idx) {
        if (idx >= 0 && idx < static_cast<int>(m_playerEntities.size())) {
            return m_playerEntities[idx].get();
        }
        return nullptr;
    }

    const Player* getPlayerEntity(int idx) const {
        if (idx >= 0 && idx < static_cast<int>(m_playerEntities.size())) {
            return m_playerEntities[idx].get();
        }
        return nullptr;
    }

    float getTurnTimer() const { return m_turnTimer; }
    void tickTurnTimer(float dt) { m_turnTimer -= dt; }
    void resetTurnTimer() { m_turnTimer = 20.0f; }

    float getBotTimer() const { return m_botTimer; }
    void tickBotTimer(float dt) { m_botTimer -= dt; }
    void resetBotTimer() { m_botTimer = m_botDelay; }
    float getBotDelay() const { return m_botDelay; }

    void setLastAction(std::string action) { m_lastAction = std::move(action); }
    const std::string& getLastAction() const { return m_lastAction; }

    bool canPawnMove(int playerIdx, int pawnIdx, int dice) const;
    int getLegalMovesCount(int playerIdx, int dice) const;
    int pickBestBotPawn(int playerIdx, int dice) const;

    // Public game operations called by State/Command
    bool executeRollDice(int forcedRoll = 0);
    bool executeMovePawn(int pawnIdx);
    void nextTurn();

private:
    bool m_isSolo{false};
    int m_numPlayers{4};
    bool m_isQuick{true};
    int m_pawnCount{2}; // 2 for Quick, 4 for Classic

    int m_currentTurn{1}; // 1-based seat ID (1..4)
    Dice m_dice;
    int m_consecutiveSixes{0};

    float m_turnTimer{20.0f};
    float m_botTimer{0.5f};
    float m_botDelay{0.5f};

    bool m_finished{false};
    int m_winner{0};
    std::string m_lastAction{"Game started"};

    std::array<LudoPlayer, 4> m_players{};

    // Strategy Pattern: Players with PlayerControllers
    std::vector<std::unique_ptr<Player>> m_playerEntities;

    // State Pattern: Sovereign Phase State
    std::unique_ptr<IGamePhaseState> m_state;

    // Command Pattern: Deserialization Factory
    LudoCommandFactory m_commandFactory;
};

} // namespace core
} // namespace gamehub
