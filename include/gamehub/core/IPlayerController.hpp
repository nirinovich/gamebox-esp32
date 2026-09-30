#pragma once

#include <cstdint>
#include <string>

namespace gamehub {
namespace core {

/**
 * @brief Strategy Pattern interface for player decision-making.
 * Decouples game rules from the source of inputs (human network streams vs autonomous algorithmic bots).
 */
template <typename StateType, typename ActionType>
class IPlayerController {
public:
    virtual ~IPlayerController() = default;

    /**
     * @brief Compute the next action given the current game state and player identity.
     */
    virtual ActionType computeAction(const StateType& state, int playerId) = 0;

    /**
     * @brief Whether this controller operates autonomously (true for bots, false for humans).
     */
    virtual bool isAutonomous() const { return false; }
};

} // namespace core
} // namespace gamehub
