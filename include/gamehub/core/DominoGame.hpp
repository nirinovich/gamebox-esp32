#pragma once

#include "Game.hpp"
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
    explicit DominoGame(bool isSolo = false, int numPlayers = 2, bool isBlockMode = true);
    ~DominoGame() override = default;

    void init() override;
    void update(float dt) override;
    void handleInput(PlayerId player, const std::string& input) override;
    std::string serializeState() const override;
    bool isFinished() const override { return m_finished; }
    int getWinner() const override { return m_winner; }

    void setNumPlayers(int n);
    int getNumPlayers() const { return m_numPlayers; }
    int getLeftEnd() const;
    int getRightEnd() const;
    bool canPlayerPlay(int pidx) const;
    size_t getBoneyardCount() const { return m_boneyard.size(); }
    bool isBlockMode() const { return m_isBlockMode; }
    void setBlockMode(bool block) { m_isBlockMode = block; }

private:
    bool playTile(int pidx, int tileIdx, const std::string& end);
    bool drawTile(int pidx);
    void passTurn(int pidx);
    void checkBlockedWin();
    void executeBotTurns();

    bool m_isSolo{false};
    int m_numPlayers{2};
    bool m_isBlockMode{true};
    int m_turn{1};
    bool m_finished{false};
    int m_winner{0};
    int m_consecutivePasses{0};

    std::deque<DominoTile> m_board;
    std::vector<DominoTile> m_hands[3];
    std::vector<DominoTile> m_boneyard;
    std::string m_lastAction;
};

} // namespace core
} // namespace gamehub
