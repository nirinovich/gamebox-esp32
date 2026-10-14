#pragma once

#include <gamehub/core/Game.hpp>
#include <gamehub/core/Player.hpp>
#include <gamehub/core/PlayerController.hpp>
#include <gamehub/core/IGamePhaseState.hpp>
#include <vector>
#include <deque>
#include <string>
#include <memory>
#include <random>

#include "DominoEntities.hpp"
#include "DominoCommands.hpp"
#include "DominoStates.hpp"
#include "DominoBotController.hpp"

namespace gamehub {
namespace core {

class DominoGame : public Game {
public:
    explicit DominoGame(bool isSolo = false, int numPlayers = 2, int targetScore = 100);
    ~DominoGame() override = default;

    // --- Core Game Interface ---
    void init() override;
    void update(float dt) override;
    void handleInput(PlayerId player, const std::string& input) override;
    std::string serializeState() const override;
    std::string serializeStateForPlayer(PlayerId id) const override;
    bool isFinished() const override { return m_finished; }
    int getWinner() const override { return m_winner; }
    void setPlayerCount(int count) override { setNumPlayers(count); }

    // --- State & Command Dispatch Seams ---
    void transitionTo(std::unique_ptr<IDominoState> nextState);
    IDominoState* getActiveState() const { return m_state.get(); }
    bool processCommand(PlayerId player, const IDominoCommand& cmd);
    bool processCommand(PlayerId player, const ICommand& cmd);

    // --- Game Rules & Domain State Accessors ---
    void setNumPlayers(int n);
    int getNumPlayers() const { return m_numPlayers; }
    int getTurn() const { return m_turn; }
    void setTurn(int turn) { m_turn = turn; }

    int getLeftEnd() const { return m_board.getLeftEnd(); }
    int getRightEnd() const { return m_board.getRightEnd(); }
    bool canPlayerPlay(int pidx) const;
    size_t getDeadwoodCount() const { return m_tileSet.getDeadwoodCount(); }
    size_t getBoneyardCount() const { return m_tileSet.getDeadwoodCount(); }

    int getTargetScore() const { return m_targetScore; }
    void setTargetScore(int target) { m_targetScore = (target > 0) ? target : 100; }
    int getRebon() const { return m_rebonPool; }
    void setRebon(int rebon) { m_rebonPool = rebon; }
    int getCumulativeScore(int pidx) const { return (pidx >= 0 && pidx < 4) ? m_cumulativeScores[pidx] : 0; }
    int getRound() const { return m_round; }
    bool isRoundFinished() const { return m_roundFinished; }

    // --- State and Hand Modification for Testing & Play ---
    void setPendingChoice(int pidx, int tileIdx) {
        m_pendingChoicePlayer = pidx;
        m_pendingChoiceIdx = tileIdx;
    }
    void clearPendingChoice() {
        m_pendingChoicePlayer = 0;
        m_pendingChoiceIdx = -1;
    }
    int getPendingChoicePlayer() const { return m_pendingChoicePlayer; }
    int getPendingChoiceIndex() const { return m_pendingChoiceIdx; }

    void startNewRound();
    void setupCustomHand(int pidx, const std::vector<DominoTile>& tiles) {
        if (pidx >= 0 && pidx < m_numPlayers) {
            m_hands[pidx].setTiles(tiles);
        }
    }
    void setBoard(const std::deque<DominoTile>& b) {
        m_board.setTiles(b);
    }

    // --- Public Operations executed by States / Commands ---
    bool executePlayTile(int pidx, int tileIdx, const std::string& endChoice);
    void executePassTurn(int pidx);
    void checkBlockedWin();
    void determineOpeningLead();
    void advanceTurn();

    DominoBoard& getBoard() { return m_board; }
    const DominoBoard& getBoard() const { return m_board; }

    DominoHand& getHand(size_t pidx) { return m_hands[pidx]; }
    const DominoHand& getHand(size_t pidx) const { return m_hands[pidx]; }

    Player* getPlayer(size_t pidx) {
        if (pidx < m_players.size()) return m_players[pidx].get();
        return nullptr;
    }

    void setLastAction(std::string action) { m_lastAction = std::move(action); }
    const std::string& getLastAction() const { return m_lastAction; }

    void setFinished(bool fin, int winner) {
        m_finished = fin;
        m_winner = winner;
        if (m_finished && m_listener) {
            m_listener->onGameFinished(static_cast<PlayerId>(winner));
        }
    }

    void setRoundFinished(bool fin, int winner, int points) {
        m_roundFinished = fin;
        m_roundWinner = winner;
        m_roundPointsAwarded = points;
        if (m_roundFinished && m_listener) {
            m_listener->onRoundFinished(static_cast<PlayerId>(winner), points);
        }
    }

private:
    bool m_isSolo{false};
    int m_numPlayers{2};
    int m_targetScore{100};
    int m_turn{1};
    bool m_finished{false};
    int m_winner{0};
    int m_consecutivePasses{0};

    int m_round{1};
    bool m_roundFinished{false};
    int m_roundWinner{0};
    int m_roundPointsAwarded{0};
    int m_rebonPool{0};
    int m_cumulativeScores[4]{0, 0, 0, 0};

    int m_pendingChoicePlayer{0};
    int m_pendingChoiceIdx{-1};

    // Domain Entities
    DominoBoard m_board;
    std::vector<DominoHand> m_hands;
    DominoTileSet m_tileSet;
    std::mt19937 m_rng{1337};

    // Strategy Pattern: Players with PlayerControllers
    std::vector<std::unique_ptr<Player>> m_players;

    // State Pattern: Sovereign Phase State
    std::unique_ptr<IDominoState> m_state;

    // Command Pattern: Deserialization Factory
    DominoCommandFactory m_commandFactory;

    std::string m_lastAction;
};

} // namespace core
} // namespace gamehub
