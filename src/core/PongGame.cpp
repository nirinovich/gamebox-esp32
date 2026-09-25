#include <gamehub/core/PongGame.hpp>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <iomanip>

namespace gamehub {
namespace core {

PongGame::PongGame() {
    init();
}

void PongGame::init() {
    m_p1y = FIELD_H / 2.f;
    m_p2y = FIELD_H / 2.f;
    m_p1vy = 0.f;
    m_p2vy = 0.f;
    m_score1 = 0;
    m_score2 = 0;
    m_finished = false;
    m_winner = 0;
    resetBall(1);
}

void PongGame::resetBall(int serveDir) {
    m_bx = FIELD_W / 2.f;
    m_by = FIELD_H / 2.f;
    float angle = (float)(std::rand() % 60 - 30) * 3.14159f / 180.f;
    m_vx = BALL_SPEED_INIT * serveDir;
    m_vy = BALL_SPEED_INIT * std::sin(angle);
}

void PongGame::update(float dt) {
    if (m_finished) return;

    // Move paddles
    m_p1y += m_p1vy * dt;
    m_p2y += m_p2vy * dt;
    float halfPad = PAD_H / 2.f;
    if (m_p1y < halfPad) m_p1y = halfPad;
    if (m_p1y > FIELD_H - halfPad) m_p1y = FIELD_H - halfPad;
    if (m_p2y < halfPad) m_p2y = halfPad;
    if (m_p2y > FIELD_H - halfPad) m_p2y = FIELD_H - halfPad;

    // Move ball
    m_bx += m_vx * dt;
    m_by += m_vy * dt;

    // Top/bottom wall bounce
    if (m_by - BALL_R < 0.f) { m_by = BALL_R; m_vy = std::abs(m_vy); }
    if (m_by + BALL_R > FIELD_H) { m_by = FIELD_H - BALL_R; m_vy = -std::abs(m_vy); }

    // Paddle 1 (left, x = PAD_W)
    float p1x = PAD_W;
    if (m_vx < 0 && m_bx - BALL_R < p1x + PAD_W && m_bx + BALL_R > p1x - PAD_W) {
        if (m_by >= m_p1y - halfPad && m_by <= m_p1y + halfPad) {
            float offset = (m_by - m_p1y) / halfPad;
            m_vx = std::abs(m_vx) * 1.05f;
            m_vy = offset * BALL_SPEED_INIT;
            m_bx = p1x + PAD_W + BALL_R;
        }
    }
    // Paddle 2 (right, x = FIELD_W - PAD_W)
    float p2x = FIELD_W - PAD_W;
    if (m_vx > 0 && m_bx + BALL_R > p2x - PAD_W && m_bx - BALL_R < p2x + PAD_W) {
        if (m_by >= m_p2y - halfPad && m_by <= m_p2y + halfPad) {
            float offset = (m_by - m_p2y) / halfPad;
            m_vx = -std::abs(m_vx) * 1.05f;
            m_vy = offset * BALL_SPEED_INIT;
            m_bx = p2x - PAD_W - BALL_R;
        }
    }
    // Speed cap
    float speed = std::sqrt(m_vx*m_vx + m_vy*m_vy);
    float maxSpeed = BALL_SPEED_INIT * 3.f;
    if (speed > maxSpeed) { m_vx = m_vx / speed * maxSpeed; m_vy = m_vy / speed * maxSpeed; }

    // Score
    if (m_bx < 0.f) {
        ++m_score2;
        if (m_score2 >= WIN_SCORE) { m_finished = true; m_winner = 2; return; }
        resetBall(1);
    }
    if (m_bx > FIELD_W) {
        ++m_score1;
        if (m_score1 >= WIN_SCORE) { m_finished = true; m_winner = 1; return; }
        resetBall(-1);
    }
}

void PongGame::handleInput(PlayerId player, const std::string& input) {
    if (m_finished) return;
    bool up   = input.find("\"UP\"")   != std::string::npos || input.find("\"val\":\"UP\"")   != std::string::npos;
    bool down = input.find("\"DOWN\"") != std::string::npos || input.find("\"val\":\"DOWN\"") != std::string::npos;
    bool stop = input.find("\"STOP\"") != std::string::npos || input.find("\"val\":\"STOP\"") != std::string::npos;
    float vel = stop ? 0.f : (up ? -PAD_SPEED : (down ? PAD_SPEED : 0.f));
    if (player == 1) m_p1vy = vel;
    else if (player == 2) m_p2vy = vel;
}

std::string PongGame::serializeState() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1);
    oss << "{\"type\":\"state\",\"game\":\"pong\""
        << ",\"bx\":"  << m_bx
        << ",\"by\":"  << m_by
        << ",\"p1y\":" << m_p1y
        << ",\"p2y\":" << m_p2y
        << ",\"score1\":" << m_score1
        << ",\"score2\":" << m_score2
        << ",\"finished\":" << (m_finished ? "true" : "false")
        << ",\"winner\":"  << m_winner
        << "}";
    return oss.str();
}

} // namespace core
} // namespace gamehub
