#pragma once

#include <memory>
#include "GameTypes.hpp"
#include "Game.hpp"
#include <snake/host/SnakeGame.hpp>
#include <tictactoe/host/TicTacToeGame.hpp>
#include <connectfour/host/ConnectFourGame.hpp>
#include <pong/host/PongGame.hpp>
#include <tron/host/TronGame.hpp>
#include <battleship/host/BattleshipGame.hpp>
#include <domino/host/DominoGame.hpp>

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
            case GameType::PONG_SOLO:          return std::make_unique<PongGame>(true);
            case GameType::PONG_DUEL:          return std::make_unique<PongGame>(false);
            case GameType::TRON_SOLO:          return std::make_unique<TronGame>(true);
            case GameType::TRON_DUEL:          return std::make_unique<TronGame>(false);
            case GameType::BATTLESHIP_SOLO:    return std::make_unique<BattleshipGame>(true);
            case GameType::BATTLESHIP_PVP:     return std::make_unique<BattleshipGame>(false);
            case GameType::DOMINO_SOLO:        return std::make_unique<DominoGame>(true, 2);
            case GameType::DOMINO_PVP:         return std::make_unique<DominoGame>(false, 3);
            default:                           return std::make_unique<SnakeGame>(false);
        }
    }
};

} // namespace core
} // namespace gamehub
