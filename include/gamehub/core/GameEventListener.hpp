#pragma once

#include <cstdint>
#include "GameTypes.hpp"

namespace gamehub {
namespace core {

/**
 * @brief Observer Pattern interface for game events.
 * Allows authoritative games to notify Room and HostEngine of outcomes and lifecycle shifts
 * without procedural polling ("Tell, Don't Ask").
 */
class GameEventListener {
public:
    virtual ~GameEventListener() = default;

    /**
     * @brief Dispatched when a game concludes.
     */
    virtual void onGameFinished(PlayerId winner) = 0;

    /**
     * @brief Dispatched when a multi-round game (e.g. Dominoes) completes an individual round.
     */
    virtual void onRoundFinished(PlayerId /*roundWinner*/, int /*pointsAwarded*/) {}

    /**
     * @brief Dispatched when an authoritative mutation invalidates previous client state,
     * signaling that an updated snapshot should be broadcast.
     */
    virtual void onStateInvalidated() {}
};

} // namespace core
} // namespace gamehub
