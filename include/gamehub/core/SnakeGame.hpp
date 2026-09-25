#pragma once

#include "Game.hpp"
#include <vector>

namespace gamehub {
namespace core {

class SnakeGame : public Game {
public:
    explicit SnakeGame(bool isDuel = false);
    ~SnakeGame() override = default;

    void init() override;
    void update(float dt) override;
    void handleInput(PlayerId id, const std::string& commandJson) override;
    std::string serializeState() const override;
    bool isFinished() const override { return m_finished; }
    int getWinner() const override { return m_winner; }

    void setFood(int x, int y) { m_food = {x, y}; }

private:
    void spawnFood();

    bool m_isDuel{false};
    int m_gridWidth{20};
    int m_gridHeight{20};

    std::vector<Point> m_snake1;
    Direction m_dir1{Direction::RIGHT};
    Direction m_nextDir1{Direction::RIGHT};
    int m_score1{0};

    std::vector<Point> m_snake2;
    Direction m_dir2{Direction::LEFT};
    Direction m_nextDir2{Direction::LEFT};
    int m_score2{0};

    Point m_food{15, 10};
    bool m_finished{false};
    int m_winner{0}; // 0 = ongoing, 1 = p1 win, 2 = p2 win, -1 = p1 died in solo
};

} // namespace core
} // namespace gamehub
