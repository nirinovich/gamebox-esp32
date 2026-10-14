#pragma once

#include <string>
#include "GameTypes.hpp"

namespace gamehub {
namespace core {

class Game;
class ICommand;

/**
 * @brief GoF State Pattern interface for discrete game phases and turn lifecycles.
 * The active state holds sovereignty over command validation, transition logic, and phase execution.
 */
class IGamePhaseState {
public:
    virtual ~IGamePhaseState() = default;

    /**
     * @brief Hook called when transitioning into this state.
     */
    virtual void onEnter(Game& /*game*/) {}

    /**
     * @brief Handle and validate a polymorphic command submitted by a player.
     * @return true if the command was accepted and processed, false if illegal in this phase.
     */
    virtual bool handleCommand(Game& game, PlayerId playerId, const ICommand& command) = 0;

    /**
     * @brief Periodic tick update hook (e.g. for timers, animations, automatic transitions).
     */
    virtual void update(Game& /*game*/, float /*dt*/) {}

    /**
     * @brief Hook called when transitioning out of this state.
     */
    virtual void onExit(Game& /*game*/) {}

    /**
     * @brief Canonical name of this state (e.g. "WAITING_ROLL", "ROUND_ACTIVE", "BLOCKED").
     */
    virtual std::string getPhaseName() const = 0;
};

} // namespace core
} // namespace gamehub
