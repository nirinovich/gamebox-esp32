#include <gamehub/core/TronGame.hpp>
#include <sstream>

namespace gamehub {
namespace core {

TronGame::TronGame() {
    init();
}

void TronGame::init() {
    m_grid.assign(GRID, std::vector<int>(GRID, 0));
    // P1 starts left-centre heading right
    m_p1 = {2, GRID/2, 1, 0, 1, 0, true};
    // P2 starts right-centre heading left
    m_p2 = {GRID-3, GRID/2, -1, 0, -1, 0, true};
    // Stamp initial positions
    m_grid[m_p1.x][m_p1.y] = 1;
    m_grid[m_p2.x][m_p2.y] = 2;
    m_finished = false;
    m_winner = 0;
    m_tickAccum = 0.f;
}

bool TronGame::outOfBounds(int x, int y) const {
    return x < 0 || x >= GRID || y < 0 || y >= GRID;
}

void TronGame::update(float dt) {
    if (m_finished) return;
    m_tickAccum += dt;
    if (m_tickAccum < TICK_INTERVAL) return;
    m_tickAccum -= TICK_INTERVAL;

    // Commit queued direction
    m_p1.dx = m_p1.nextDx; m_p1.dy = m_p1.nextDy;
    m_p2.dx = m_p2.nextDx; m_p2.dy = m_p2.nextDy;

    // Advance
    int nx1 = m_p1.x + m_p1.dx, ny1 = m_p1.y + m_p1.dy;
    int nx2 = m_p2.x + m_p2.dx, ny2 = m_p2.y + m_p2.dy;

    bool dead1 = outOfBounds(nx1, ny1) || m_grid[nx1][ny1] != 0;
    bool dead2 = outOfBounds(nx2, ny2) || m_grid[nx2][ny2] != 0;
    // Head-on collision
    if (nx1 == nx2 && ny1 == ny2) { dead1 = true; dead2 = true; }

    if (!dead1) { m_p1.x = nx1; m_p1.y = ny1; m_grid[nx1][ny1] = 1; }
    if (!dead2) { m_p2.x = nx2; m_p2.y = ny2; m_grid[nx2][ny2] = 2; }

    m_p1.alive = !dead1;
    m_p2.alive = !dead2;

    if (dead1 || dead2) {
        m_finished = true;
        if (dead1 && dead2) m_winner = -1;
        else if (dead1)     m_winner = 2;
        else                m_winner = 1;
    }
}

void TronGame::handleInput(PlayerId player, const std::string& input) {
    if (m_finished) return;
    int dx = 0, dy = 0;
    if (input.find("UP")    != std::string::npos) { dx =  0; dy = -1; }
    if (input.find("DOWN")  != std::string::npos) { dx =  0; dy =  1; }
    if (input.find("LEFT")  != std::string::npos) { dx = -1; dy =  0; }
    if (input.find("RIGHT") != std::string::npos) { dx =  1; dy =  0; }
    if (dx == 0 && dy == 0) return;

    if (player == 1) {
        // Disallow 180-degree reversal
        if (dx == -m_p1.dx && dy == -m_p1.dy) return;
        m_p1.nextDx = dx; m_p1.nextDy = dy;
    } else if (player == 2) {
        if (dx == -m_p2.dx && dy == -m_p2.dy) return;
        m_p2.nextDx = dx; m_p2.nextDy = dy;
    }
}

std::string TronGame::serializeState() const {
    std::ostringstream oss;
    oss << "{\"type\":\"state\",\"game\":\"tron\""
        << ",\"p1\":{\"x\":" << m_p1.x << ",\"y\":" << m_p1.y << ",\"alive\":" << (m_p1.alive?"true":"false") << "}"
        << ",\"p2\":{\"x\":" << m_p2.x << ",\"y\":" << m_p2.y << ",\"alive\":" << (m_p2.alive?"true":"false") << "}"
        << ",\"finished\":" << (m_finished ? "true" : "false")
        << ",\"winner\":" << m_winner
        << ",\"grid\":[";
    // Flat row-major: grid[x][y], emit row by row (y outer)
    for (int y = 0; y < GRID; ++y) {
        for (int x = 0; x < GRID; ++x) {
            if (y > 0 || x > 0) oss << ",";
            oss << m_grid[x][y];
        }
    }
    oss << "]}";
    return oss.str();
}

} // namespace core
} // namespace gamehub
