#pragma once

#include <memory>
#include <string>
#include <gamehub/core/PlayerController.hpp>
#include "LudoCommands.hpp"

namespace gamehub {
namespace core {

class LudoGame;

/**
 * @brief Autonomous strategy implementation for Ludo bots using tactical heuristics.
 * Decides whether to roll the dice or select the tactically optimal pawn to move.
 */
class LudoBotController : public BotController {
public:
    LudoBotController() = default;
    ~LudoBotController() override = default;

    std::unique_ptr<ICommand> decideAction(const Game& game, PlayerId id) override;
};

} // namespace core
} // namespace gamehub
