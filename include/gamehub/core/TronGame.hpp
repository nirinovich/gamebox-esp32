#pragma once
#include "Game.hpp"
#include "GameTypes.hpp"
#include <vector>
#include <string>

namespace gamehub {
namespace core {

class TronGame : public Game {
public:
    static constexpr int GRID = 30;
    static constexpr float TICK_INTERVAL = 0.10f;

    TronGame();

    void init() override;
    void update(float dt) override;
    void handleInput(PlayerId player, const std::string& input) override;
    std::string serializeState() const override;
    bool isFinished() const override { return m_finished; }
    int  getWinner() const override  { return m_winner; }

private:
    struct Bike {
        int x{0}, y{0};
        int dx{1}, dy{0};
        int nextDx{1}, nextDy{0};
        bool alive{true};
    };

    std::vector<std::vector<int>> m_grid; // 0=free, 1=P1 trail, 2=P2 trail
    Bike  m_p1, m_p2;
    bool  m_finished{false};
    int   m_winner{0};
    float m_tickAccum{0.f};

    bool outOfBounds(int x, int y) const;
};

} // namespace core
} // namespace gamehub
