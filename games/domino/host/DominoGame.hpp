#pragma once

#include <gamehub/core/Game.hpp>
#include <vector>
#include <deque>
#include <string>

namespace gamehub {
namespace core {

struct DominoTile {
    int left{0};
    int right{0};

    int pips() const { return left + right; }
    bool matches(int val) const { return left == val || right == val; }
    void flip() { std::swap(left, right); }
};

class DominoGame : public Game {
public:
    explicit DominoGame(bool isSolo = false, int numPlayers = 2, int targetScore = 100);
    ~DominoGame() override = default;

    void init() override;
    void update(float dt) override;
    void handleInput(PlayerId player, const std::string& input) override;
    std::string serializeState() const override;
    std::string serializeStateForPlayer(PlayerId id) const override;
    bool isFinished() const override { return m_finished; }
    int getWinner() const override { return m_winner; }
    void setPlayerCount(int count) override { setNumPlayers(count); }

    void setNumPlayers(int n);
    int getNumPlayers() const { return m_numPlayers; }
    int getLeftEnd() const;
    int getRightEnd() const;
    bool canPlayerPlay(int pidx) const;
    size_t getDeadwoodCount() const { return m_deadwood.size(); }
    size_t getBoneyardCount() const { return m_deadwood.size(); } // Backward compat alias

    int getTargetScore() const { return m_targetScore; }
    void setTargetScore(int target) { m_targetScore = (target > 0) ? target : 100; }
    int getRebon() const { return m_rebonPool; }
    int getCumulativeScore(int pidx) const { return (pidx >= 0 && pidx < 4) ? m_cumulativeScores[pidx] : 0; }
    int getRound() const { return m_round; }
    bool isRoundFinished() const { return m_roundFinished; }

    void setPendingChoice(int pidx, int tileIdx) {
        m_pendingChoicePlayer = pidx;
        m_pendingChoiceIdx = tileIdx;
    }
    void clearPendingChoice() {
        m_pendingChoicePlayer = 0;
        m_pendingChoiceIdx = -1;
    }

    void startNewRound();

    // Direct state setup helper for unit testing deterministic scenarios
    void setupCustomHand(int pidx, const std::vector<DominoTile>& tiles) {
        if (pidx >= 0 && pidx < m_numPlayers) {
            m_hands[pidx] = tiles;
        }
    }
    void setTurn(int turn) { m_turn = turn; }
    void setBoard(const std::deque<DominoTile>& b) { m_board = b; }

private:
    bool playTile(int pidx, int tileIdx, const std::string& endChoice);
    void passTurn(int pidx);
    void checkBlockedWin();
    void determineOpeningLead();
    void executeBotTurns();

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

    std::deque<DominoTile> m_board;
    std::vector<DominoTile> m_hands[4];
    std::vector<DominoTile> m_deadwood;
    std::string m_lastAction;
};

} // namespace core
} // namespace gamehub
