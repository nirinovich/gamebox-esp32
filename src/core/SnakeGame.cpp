#include <gamehub/core/SnakeGame.hpp>
#include <sstream>
#include <cstdlib>

namespace gamehub {
namespace core {

SnakeGame::SnakeGame(bool isDuel) : m_isDuel(isDuel) {}

void SnakeGame::init() {
    m_finished = false;
    m_winner = 0;
    m_score1 = 0;
    m_score2 = 0;

    m_snake1.clear();
    m_snake1.push_back({10, 10});
    m_snake1.push_back({9, 10});
    m_snake1.push_back({8, 10});
    m_dir1 = Direction::RIGHT;
    m_nextDir1 = Direction::RIGHT;

    if (m_isDuel) {
        m_snake2.clear();
        m_snake2.push_back({10, 5});
        m_snake2.push_back({11, 5});
        m_snake2.push_back({12, 5});
        m_dir2 = Direction::LEFT;
        m_nextDir2 = Direction::LEFT;
    }

    m_food = {15, 10};
}

void SnakeGame::spawnFood() {
    for (int attempts = 0; attempts < 100; ++attempts) {
        int fx = rand() % m_gridWidth;
        int fy = rand() % m_gridHeight;
        bool onSnake = false;

        for (const auto& pt : m_snake1) {
            if (pt.x == fx && pt.y == fy) { onSnake = true; break; }
        }
        if (!onSnake && m_isDuel) {
            for (const auto& pt : m_snake2) {
                if (pt.x == fx && pt.y == fy) { onSnake = true; break; }
            }
        }

        if (!onSnake) {
            m_food = {fx, fy};
            return;
        }
    }
}

void SnakeGame::handleInput(PlayerId id, const std::string& commandJson) {
    if (m_finished) return;

    auto valPos = commandJson.find("\"val\":");
    if (valPos == std::string::npos) return;

    std::string dirStr;
    if (commandJson.find("\"UP\"") != std::string::npos) dirStr = "UP";
    else if (commandJson.find("\"DOWN\"") != std::string::npos) dirStr = "DOWN";
    else if (commandJson.find("\"LEFT\"") != std::string::npos) dirStr = "LEFT";
    else if (commandJson.find("\"RIGHT\"") != std::string::npos) dirStr = "RIGHT";

    if (dirStr.empty()) return;

    if (id == 1 || !m_isDuel) {
        if (dirStr == "UP" && m_dir1 != Direction::DOWN) m_nextDir1 = Direction::UP;
        else if (dirStr == "DOWN" && m_dir1 != Direction::UP) m_nextDir1 = Direction::DOWN;
        else if (dirStr == "LEFT" && m_dir1 != Direction::RIGHT) m_nextDir1 = Direction::LEFT;
        else if (dirStr == "RIGHT" && m_dir1 != Direction::LEFT) m_nextDir1 = Direction::RIGHT;
    } else if (id == 2 && m_isDuel) {
        if (dirStr == "UP" && m_dir2 != Direction::DOWN) m_nextDir2 = Direction::UP;
        else if (dirStr == "DOWN" && m_dir2 != Direction::UP) m_nextDir2 = Direction::DOWN;
        else if (dirStr == "LEFT" && m_dir2 != Direction::RIGHT) m_nextDir2 = Direction::LEFT;
        else if (dirStr == "RIGHT" && m_dir2 != Direction::LEFT) m_nextDir2 = Direction::RIGHT;
    }
}

void SnakeGame::update(float /*dt*/) {
    if (m_finished) return;

    m_dir1 = m_nextDir1;
    Point head1 = m_snake1.front();
    Point newHead1 = head1;

    switch (m_dir1) {
        case Direction::UP:    newHead1.y -= 1; break;
        case Direction::DOWN:  newHead1.y += 1; break;
        case Direction::LEFT:  newHead1.x -= 1; break;
        case Direction::RIGHT: newHead1.x += 1; break;
    }

    bool dead1 = false;
    bool dead2 = false;

    // Check wall collision for Snake 1
    if (newHead1.x < 0 || newHead1.x >= m_gridWidth || newHead1.y < 0 || newHead1.y >= m_gridHeight) {
        dead1 = true;
    }

    // Check self collision for Snake 1
    for (size_t i = 0; i < m_snake1.size() - 1; ++i) {
        if (m_snake1[i] == newHead1) {
            dead1 = true;
            break;
        }
    }

    Point newHead2 = {0, 0};
    if (m_isDuel) {
        m_dir2 = m_nextDir2;
        Point head2 = m_snake2.front();
        newHead2 = head2;

        switch (m_dir2) {
            case Direction::UP:    newHead2.y -= 1; break;
            case Direction::DOWN:  newHead2.y += 1; break;
            case Direction::LEFT:  newHead2.x -= 1; break;
            case Direction::RIGHT: newHead2.x += 1; break;
        }

        // Check wall collision for Snake 2
        if (newHead2.x < 0 || newHead2.x >= m_gridWidth || newHead2.y < 0 || newHead2.y >= m_gridHeight) {
            dead2 = true;
        }

        // Check self collision for Snake 2
        for (size_t i = 0; i < m_snake2.size() - 1; ++i) {
            if (m_snake2[i] == newHead2) {
                dead2 = true;
                break;
            }
        }

        // Head-to-head collision
        if (newHead1 == newHead2) {
            dead1 = true;
            dead2 = true;
        }

        // Snake 1 hits Snake 2 body
        for (const auto& pt : m_snake2) {
            if (pt == newHead1) {
                dead1 = true;
                break;
            }
        }

        // Snake 2 hits Snake 1 body
        for (const auto& pt : m_snake1) {
            if (pt == newHead2) {
                dead2 = true;
                break;
            }
        }
    }

    if (dead1 && dead2) {
        m_finished = true;
        m_winner = -1; // Draw
        return;
    } else if (dead1) {
        m_finished = true;
        m_winner = m_isDuel ? 2 : -1;
        return;
    } else if (dead2) {
        m_finished = true;
        m_winner = 1;
        return;
    }

    // Move Snake 1
    m_snake1.insert(m_snake1.begin(), newHead1);
    if (newHead1 == m_food) {
        m_score1 += 10;
        spawnFood();
    } else {
        m_snake1.pop_back();
    }

    // Move Snake 2
    if (m_isDuel) {
        m_snake2.insert(m_snake2.begin(), newHead2);
        if (newHead2 == m_food) {
            m_score2 += 10;
            spawnFood();
        } else {
            m_snake2.pop_back();
        }
    }
}

std::string SnakeGame::serializeState() const {
    std::ostringstream oss;
    oss << "{\"type\":\"state\",\"game\":\"snake\",\"duel\":" << (m_isDuel ? "true" : "false")
        << ",\"finished\":" << (m_finished ? "true" : "false")
        << ",\"winner\":" << m_winner
        << ",\"score\":" << m_score1
        << ",\"head\":[" << (m_snake1.empty() ? 0 : m_snake1.front().x) << "," << (m_snake1.empty() ? 0 : m_snake1.front().y) << "]"
        << ",\"food\":[" << m_food.x << "," << m_food.y << "]"
        << ",\"body\":[";

    for (size_t i = 0; i < m_snake1.size(); ++i) {
        if (i > 0) oss << ",";
        oss << "[" << m_snake1[i].x << "," << m_snake1[i].y << "]";
    }
    oss << "]";

    if (m_isDuel) {
        oss << ",\"score2\":" << m_score2
            << ",\"head2\":[" << (m_snake2.empty() ? 0 : m_snake2.front().x) << "," << (m_snake2.empty() ? 0 : m_snake2.front().y) << "]"
            << ",\"body2\":[";
        for (size_t i = 0; i < m_snake2.size(); ++i) {
            if (i > 0) oss << ",";
            oss << "[" << m_snake2[i].x << "," << m_snake2[i].y << "]";
        }
        oss << "]";
    }

    oss << "}";
    return oss.str();
}

} // namespace core
} // namespace gamehub
