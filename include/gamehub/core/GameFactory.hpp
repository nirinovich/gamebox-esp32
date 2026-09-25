#pragma once

#include <memory>
#include "GameTypes.hpp"
#include "SnakeGame.hpp"
#include "TicTacToeGame.hpp"

namespace gamehub {
namespace core {

class GameFactory {
public:
    static std::unique_ptr<Game> create(GameType type) {
        switch (type) {
            case GameType::SNAKE_SOLO:
                return std::make_unique<SnakeGame>(false);
            case GameType::SNAKE_DUEL:
                return std::make_unique<SnakeGame>(true);
            case GameType::TICTACTOE_SOLO:
                return std::make_unique<TicTacToeGame>(true);
            case GameType::TICTACTOE_PVP:
                return std::make_unique<TicTacToeGame>(false);
            default:
                return std::make_unique<SnakeGame>(false);
        }
    }
};

} // namespace core
} // namespace gamehub
