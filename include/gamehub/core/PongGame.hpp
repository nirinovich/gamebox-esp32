#pragma once
#include "Game.hpp"
#include "GameTypes.hpp"
#include <string>

namespace gamehub {
namespace core {

class PongGame : public Game {
public:
    static constexpr float FIELD_W  = 400.f;
    static constexpr float FIELD_H  = 400.f;
    static constexpr float PAD_W    = 10.f;
    static constexpr float PAD_H    = 60.f;
    static constexpr float BALL_R   = 8.f;
    static constexpr float PAD_SPEED = 200.f;
    static constexpr float BALL_SPEED_INIT = 220.f;
    static constexpr int   WIN_SCORE = 5;

    PongGame();

    void init() override;
    void update(float dt) override;
    void handleInput(PlayerId player, const std::string& input) override;
    std::string serializeState() const override;
    bool isFinished() const override { return m_finished; }
    int  getWinner() const override  { return m_winner; }

private:
    float m_bx{}, m_by{};
    float m_vx{}, m_vy{};
    float m_p1y{}, m_p2y{};
    float m_p1vy{}, m_p2vy{};
    int   m_score1{0}, m_score2{0};
    bool  m_finished{false};
    int   m_winner{0};

    void resetBall(int serveDir);
};

} // namespace core
} // namespace gamehub
