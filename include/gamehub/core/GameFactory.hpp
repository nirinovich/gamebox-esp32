#pragma once

#include <memory>
#include "GameTypes.hpp"
#include "Game.hpp"
#include "SnakeGame.hpp"
#include "TicTacToeGame.hpp"
#include "ConnectFourGame.hpp"
#include "PongGame.hpp"
#include "TronGame.hpp"
#include "BattleshipGame.hpp"
#include "DominoGame.hpp"

namespace gamehub {
namespace core {

class GameFactory {
public:
    static std::unique_ptr<Game> create(GameType type) {
        switch (type) {
            case GameType::SNAKE_SOLO:         return std::make_unique<SnakeGame>(false);
            case GameType::SNAKE_DUEL:         return std::make_unique<SnakeGame>(true);
            case GameType::TICTACTOE_SOLO:     return std::make_unique<TicTacToeGame>(true);
            case GameType::TICTACTOE_PVP:      return std::make_unique<TicTacToeGame>(false);
            case GameType::CONNECT_FOUR_SOLO:  return std::make_unique<ConnectFourGame>(true);
            case GameType::CONNECT_FOUR_PVP:   return std::make_unique<ConnectFourGame>(false);
            case GameType::PONG_DUEL:          return std::make_unique<PongGame>();
            case GameType::TRON_DUEL:          return std::make_unique<TronGame>();
            case GameType::BATTLESHIP_PVP:     return std::make_unique<BattleshipGame>();
            case GameType::DOMINO_SOLO:        return std::make_unique<DominoGame>(true, 2);
            case GameType::DOMINO_PVP:         return std::make_unique<DominoGame>(false, 3);
            default:                           return std::make_unique<SnakeGame>(false);
        }
    }
};

} // namespace core
} // namespace gamehub
