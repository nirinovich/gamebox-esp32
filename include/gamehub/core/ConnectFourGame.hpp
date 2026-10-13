#pragma once
#include "Game.hpp"
#include "GameTypes.hpp"
#include "IBotStrategy.hpp"
#include <array>
#include <string>
#include <memory>

namespace gamehub {
namespace core {

// 7 columns x 6 rows. m_board[col][row], row 0 = bottom.
class ConnectFourGame : public Game {
public:
    static constexpr int COLS = 7;
    static constexpr int ROWS = 6;

    explicit ConnectFourGame(bool vsBot, std::unique_ptr<IConnectFourStrategy> botStrategy = nullptr);

    void init() override;
    void update(float dt) override;
    void handleInput(PlayerId player, const std::string& input) override;
    std::string serializeState() const override;
    bool isFinished() const override { return m_finished; }
    int  getWinner() const override  { return m_winner; }

    void setBotStrategy(std::unique_ptr<IConnectFourStrategy> strategy) { m_botStrategy = std::move(strategy); }

private:
    bool m_vsBot;
    std::array<std::array<int,ROWS>, COLS> m_board{};
    int  m_turn{1};
    bool m_finished{false};
    int  m_winner{0};
    std::unique_ptr<IConnectFourStrategy> m_botStrategy;

    bool dropPiece(int col, int player);
    bool checkWin(int player) const;
    bool isBoardFull() const;
    int  bestBotCol();
};

} // namespace core
} // namespace gamehub
