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
    PONG_SOLO,
    PONG_DUEL,
    TRON_SOLO,
    TRON_DUEL,
    BATTLESHIP_SOLO,
    BATTLESHIP_PVP,
    DOMINO_SOLO,
    DOMINO_PVP
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

inline const char* gameTypeToString(GameType gt) {
    switch (gt) {
        case GameType::SNAKE_SOLO: return "snake_solo";
        case GameType::SNAKE_DUEL: return "snake_duel";
        case GameType::TICTACTOE_SOLO: return "ttt_solo";
        case GameType::TICTACTOE_PVP: return "ttt_pvp";
        case GameType::CONNECT_FOUR_SOLO: return "connect4_solo";
        case GameType::CONNECT_FOUR_PVP: return "connect4_pvp";
        case GameType::PONG_SOLO: return "pong_solo";
        case GameType::PONG_DUEL: return "pong_duel";
        case GameType::TRON_SOLO: return "tron_solo";
        case GameType::TRON_DUEL: return "tron_duel";
        case GameType::BATTLESHIP_SOLO: return "battleship_solo";
        case GameType::BATTLESHIP_PVP: return "battleship_pvp";
        case GameType::DOMINO_SOLO: return "domino_solo";
        case GameType::DOMINO_PVP: return "domino_pvp";
        default: return "ttt_pvp";
    }
}

} // namespace core
} // namespace gamehub
