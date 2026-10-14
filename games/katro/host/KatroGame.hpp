#pragma once

#include <gamehub/core/Game.hpp>
#include <gamehub/core/Player.hpp>
#include <gamehub/core/PlayerController.hpp>
#include <gamehub/core/IGamePhaseState.hpp>
#include <vector>
#include <string>
#include <memory>
#include <cstdint>

#include "KatroEntities.hpp"
#include "KatroCommands.hpp"
#include "KatroStates.hpp"
#include "KatroBotController.hpp"

namespace gamehub {
namespace core {

class KatroGame : public Game {
public:
    explicit KatroGame(bool isSolo = true, int cols = KATRO_DEFAULT_COLS, int seedsPerCell = KATRO_DEFAULT_SEEDS);
    ~KatroGame() override = default;

    // --- Core Game Interface ---
    void init() override;
    void update(float dt) override;
    void handleInput(PlayerId id, const std::string& commandJson) override;
    std::string serializeState() const override;
    bool isFinished() const override { return m_finished; }
    int  getWinner()  const override { return m_winner;   }
    bool isRealTime() const override { return true; }

    // --- State & Command Dispatch Seams ---
    void transitionTo(std::unique_ptr<IGamePhaseState> nextState);
    IGamePhaseState* getActiveState() const { return m_state.get(); }
    bool processCommand(PlayerId player, const ICommand& cmd);

    // --- Moteur de jeu Katro ---
    bool playTurn(int cell, int player, KatroDirection dir);
    bool playTurn(int cell, int player);

    // --- Géométrie et plateau (déléguées à KatroBoard) ---
    int getRows() const { return m_board.getRows(); }
    int getCols() const { return m_board.getCols(); }
    int getTotalCells() const { return m_board.getTotalCells(); }
    int getInitialSeeds() const { return m_board.getInitialSeeds(); }

    int getRow(int cellIndex) const { return m_board.getRow(cellIndex); }
    int getCol(int cellIndex) const { return m_board.getCol(cellIndex); }
    int getCellIndex(int row, int col) const { return m_board.getCellIndex(row, col); }

    bool isPlayerCell(int cellIndex, KatroPlayer player) const { return m_board.isPlayerCell(cellIndex, player); }
    bool isPlayerCell(int cellIndex, int playerNum) const {
        return m_board.isPlayerCell(cellIndex, (playerNum == 1) ? KatroPlayer::SOUTH : KatroPlayer::NORTH);
    }

    bool isInnerRow(int cellIndex, KatroPlayer player) const { return m_board.isInnerRow(cellIndex, player); }
    bool isInnerRow(int cellIndex, int playerNum) const {
        return m_board.isInnerRow(cellIndex, (playerNum == 1) ? KatroPlayer::SOUTH : KatroPlayer::NORTH);
    }

    bool isOuterRow(int cellIndex, KatroPlayer player) const { return m_board.isOuterRow(cellIndex, player); }

    int nextCell(int cellIndex, KatroPlayer player, KatroDirection dir) const {
        return m_board.nextCell(cellIndex, player, dir);
    }

    int getOppositeInner(int cellIndex, KatroPlayer player) const {
        return m_board.getOppositeInner(cellIndex, player);
    }

    int getOppositeOuter(int cellIndex, KatroPlayer player) const {
        return m_board.getOppositeOuter(cellIndex, player);
    }

    bool isOpponentInnerRowEmpty(KatroPlayer player) const {
        return m_board.isOpponentInnerRowEmpty(player);
    }

    // --- Accesseurs d'état et méthodes pour les tests ---
    int getSeedCount(int cellIndex) const { return m_board.getSeedCount(cellIndex); }
    int countPlayerSeeds(KatroPlayer player) const { return m_board.countPlayerSeeds(player); }
    int countPlayerSeeds(int playerNum) const {
        return m_board.countPlayerSeeds((playerNum == 1) ? KatroPlayer::SOUTH : KatroPlayer::NORTH);
    }
    int getTotalSeedsOnBoard() const { return m_board.getTotalSeeds(); }

    int getScore(int playerNum) const { return countPlayerSeeds(playerNum); }
    int getTurn() const { return m_turn; }

    void setBoard(const std::vector<int>& board) { m_board.setBoard(board); }
    void setTurn(int turn) { m_turn = turn; }
    void setDefaultDirection(KatroDirection dir) { m_defaultDirection = dir; }
    KatroDirection getDefaultDirection() const { return m_defaultDirection; }

    void setEarlyStopThreshold(int threshold) { m_earlyStopThreshold = threshold; }
    int  getEarlyStopThreshold() const { return m_earlyStopThreshold; }

    bool canPlayerMove(KatroPlayer player) const { return m_board.canPlayerMove(player); }

    const std::vector<KatroAnimEvent>& getLastEvents() const { return m_lastEvents; }
    int getMoveSeq() const override { return m_moveSeq; }

    /**
     * Choisit un coup aléatoire parmi toutes les cases disponibles
     * (contenant des graines) dans le camp du bot (Nord).
     * @return Index de la case choisie, ou -1 si aucune case n'est disponible.
     */
    int computeBotMove() const;

    KatroBoard& getBoard() { return m_board; }
    const KatroBoard& getBoard() const { return m_board; }

    Player* getPlayer(int playerNum) {
        int idx = playerNum - 1;
        if (idx >= 0 && idx < static_cast<int>(m_players.size())) {
            return m_players[idx].get();
        }
        return nullptr;
    }

    float getBotTimer() const { return m_botTimer; }
    void tickBotTimer(float dt) { m_botTimer += dt; }
    void resetBotTimer() { m_botTimer = 0.0f; }

    void checkEndCondition();

    static constexpr float BOT_DELAY_SEC = 0.6f;

private:
    bool m_isSolo{true};

    // Domain entity
    KatroBoard m_board;

    int m_turn{1};           ///< 1 = Sud, 2 = Nord
    bool m_finished{false};
    int m_winner{0};         ///< 0 = en cours, 1 = Sud, 2 = Nord, -1 = égalité
    KatroDirection m_defaultDirection{KatroDirection::CLOCKWISE};
    int m_earlyStopThreshold{0};

    std::vector<KatroAnimEvent> m_lastEvents;
    int m_moveSeq{0};
    float m_botTimer{0.0f};

    // Strategy Pattern: Players with PlayerControllers
    std::vector<std::unique_ptr<Player>> m_players;

    // State Pattern: Sovereign Phase State
    std::unique_ptr<IGamePhaseState> m_state;

    // Command Pattern: Deserialization Factory
    KatroCommandFactory m_commandFactory;
};

} // namespace core
} // namespace gamehub
