#include <gamehub/core/RoomState.hpp>
#include <gamehub/core/Room.hpp>

namespace gamehub {
namespace core {

bool WaitingRoomState::canAcceptPlayer(const Room& room) const {
    return room.getPlayerCount() < room.getMaxPlayers();
}

bool WaitingRoomState::canStartGame(const Room& room) const {
    // Solo games can start with 1 player; multiplayer requires at least 2
    if (room.getMaxPlayers() == 1) {
        return room.getPlayerCount() >= 1;
    }
    return room.getPlayerCount() >= 2;
}

} // namespace core
} // namespace gamehub
