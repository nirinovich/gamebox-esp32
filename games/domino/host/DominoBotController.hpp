#pragma once

#include <memory>
#include <string>
#include <gamehub/core/PlayerController.hpp>
#include "DominoCommands.hpp"

namespace gamehub {
namespace core {

class DominoGame;

/**
 * @brief Autonomous strategy implementation for Malagasy Dominoes bots.
 * Analyzes open board ends and plays the highest pip tile or passes.
 */
class DominoBotController : public BotController {
public:
    DominoBotController() = default;
    ~DominoBotController() override = default;

    std::unique_ptr<ICommand> decideAction(const Game& game, PlayerId id) override;
};

} // namespace core
} // namespace gamehub
