#pragma once

#include <memory>
#include <string>
#include <gamehub/core/PlayerController.hpp>
#include "KatroCommands.hpp"

namespace gamehub {
namespace core {

class KatroGame;

/**
 * @brief Autonomous strategy implementation for Malagasy Katro bots.
 * Simulates legal moves and selects the pit maximizing seed gain or board control.
 */
class KatroBotController : public BotController {
public:
    KatroBotController() = default;
    ~KatroBotController() override = default;

    std::unique_ptr<ICommand> decideAction(const Game& game, PlayerId id) override;
};

} // namespace core
} // namespace gamehub
