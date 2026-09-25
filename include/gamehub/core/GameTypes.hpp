#pragma once

#include <cstdint>
#include <string>

namespace gamehub {
namespace core {

using PlayerId = uint32_t;

enum class GameType {
    SNAKE_SOLO,
    SNAKE_DUEL,
    TICTACTOE_SOLO,
    TICTACTOE_PVP,
    CONNECT_FOUR_SOLO,
    CONNECT_FOUR_PVP,
    PONG_DUEL,
    TRON_DUEL,
    BATTLESHIP_PVP
};

struct Point {
    int x{0};
    int y{0};

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

enum class Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

} // namespace core
} // namespace gamehub
