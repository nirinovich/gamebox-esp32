#pragma once

#include "Game.hpp"
#include "PlayerController.hpp"
#include <array>
#include <memory>

namespace gamehub {
namespace core {

class TicTacToeGame : public Game {
public:
    explicit TicTacToeGame(bool isSolo = true);
    ~TicTacToeGame() override = default;

    void init() override;
    void update(float dt) override;
    void handleInput(PlayerId id, const std::string& commandJson) override;
    std::string serializeState() const override;
    bool isFinished() const override { return m_finished; }
    int getWinner() const override { return m_winner; }

    const std::array<int, 9>& getBoard() const { return m_board; }

private:
    void checkGameState();
    bool checkWinFor(int playerMark) const;
    bool isBoardFull() const;

    bool m_isSolo{true};
    std::array<int, 9> m_board{0};
    int m_turn{1};
    bool m_finished{false};
    int m_winner{0}; // 0 = ongoing, 1 = p1 win, 2 = p2 win, -1 = draw

    std::unique_ptr<PlayerController> m_bot;
};

} // namespace core
} // namespace gamehub
