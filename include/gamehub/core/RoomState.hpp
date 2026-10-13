#pragma once

#include <string>

namespace gamehub {
namespace core {

class Room;

/**
 * @brief Canonical lifecycle states for a Room, per CONTEXT.md domain model.
 */
enum class RoomLifecycle {
    WAITING,   ///< Open in RoomDirectory, awaiting challenger(s)
    ACTIVE,    ///< Match in progress, running authoritative ticks and inputs
    FINISHED,  ///< Outcome resolved, preserving end-game stats & replay
    DISSOLVED  ///< Match terminated, sessions unbound, ready for cleanup
};

/**
 * @brief GoF State Pattern interface for Room lifecycle management.
 */
class RoomState {
public:
    virtual ~RoomState() = default;

    virtual RoomLifecycle getLifecycle() const = 0;
    virtual const char* getName() const = 0;
    virtual bool canAcceptPlayer(const Room& room) const = 0;
    virtual bool canStartGame(const Room& room) const = 0;
    virtual bool canProcessInput(const Room& room) const = 0;
    virtual bool shouldTick(const Room& room) const = 0;
};

class WaitingRoomState : public RoomState {
public:
    RoomLifecycle getLifecycle() const override { return RoomLifecycle::WAITING; }
    const char* getName() const override { return "WAITING"; }
    bool canAcceptPlayer(const Room& room) const override;
    bool canStartGame(const Room& room) const override;
    bool canProcessInput(const Room& /*room*/) const override { return false; }
    bool shouldTick(const Room& /*room*/) const override { return false; }
};

class ActiveRoomState : public RoomState {
public:
    RoomLifecycle getLifecycle() const override { return RoomLifecycle::ACTIVE; }
    const char* getName() const override { return "ACTIVE"; }
    bool canAcceptPlayer(const Room& /*room*/) const override { return false; }
    bool canStartGame(const Room& /*room*/) const override { return false; }
    bool canProcessInput(const Room& /*room*/) const override { return true; }
    bool shouldTick(const Room& /*room*/) const override { return true; }
};

class FinishedRoomState : public RoomState {
public:
    RoomLifecycle getLifecycle() const override { return RoomLifecycle::FINISHED; }
    const char* getName() const override { return "FINISHED"; }
    bool canAcceptPlayer(const Room& /*room*/) const override { return false; }
    bool canStartGame(const Room& /*room*/) const override { return false; }
    bool canProcessInput(const Room& /*room*/) const override { return false; }
    bool shouldTick(const Room& /*room*/) const override { return false; }
};

class DissolvedRoomState : public RoomState {
public:
    RoomLifecycle getLifecycle() const override { return RoomLifecycle::DISSOLVED; }
    const char* getName() const override { return "DISSOLVED"; }
    bool canAcceptPlayer(const Room& /*room*/) const override { return false; }
    bool canStartGame(const Room& /*room*/) const override { return false; }
    bool canProcessInput(const Room& /*room*/) const override { return false; }
    bool shouldTick(const Room& /*room*/) const override { return false; }
};

} // namespace core
} // namespace gamehub
