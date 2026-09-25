#include <cassert>
#include <iostream>
#include <string>
#include <gamehub/core/RoomManager.hpp>

int main() {
    std::cout << "[TEST] Running RoomManager Lifecycle Tests..." << std::endl;

    using namespace gamehub::core;

    RoomManager manager(4); // Max 4 concurrent rooms

    // 1. Create Room & Verify 4-character RoomCode
    Room* room1 = manager.createRoom(GameType::TICTACTOE_PVP, 101);
    assert(room1 != nullptr && "Room creation must succeed");
    std::string code1 = room1->getCode();
    assert(code1.length() == 4 && "RoomCode must be exactly 4 characters");
    for (char c : code1) {
        assert(((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) && "RoomCode must be uppercase alphanumeric");
    }
    assert(room1->getPlayerCount() == 1 && "Initial player must be registered");
    std::cout << "  [PASS] Room creation and 4-letter RoomCode generation verified." << std::endl;

    // 2. Join Room with RoomCode
    Room* joinSuccess = manager.joinRoom(code1, 102);
    assert(joinSuccess == room1 && "Joining with valid code must return the room");
    assert(room1->getPlayerCount() == 2 && "Room must now have 2 players");
    std::cout << "  [PASS] Joining room via valid RoomCode verified." << std::endl;

    // 3. Reject invalid RoomCode
    Room* joinFail = manager.joinRoom("ZZZZ", 103);
    assert(joinFail == nullptr && "Joining non-existent room must fail");
    std::cout << "  [PASS] Invalid RoomCode rejection verified." << std::endl;

    // 4. Enforce 4-Room Capacity Cap
    Room* room2 = manager.createRoom(GameType::TICTACTOE_PVP, 201);
    Room* room3 = manager.createRoom(GameType::SNAKE_DUEL, 301);
    Room* room4 = manager.createRoom(GameType::SNAKE_SOLO, 401);
    assert(room2 && room3 && room4 && "Creating up to 4 rooms must succeed");

    Room* room5 = manager.createRoom(GameType::TICTACTOE_PVP, 501);
    assert(room5 == nullptr && "5th room creation must be rejected by 4-room cap");
    std::cout << "  [PASS] 4-Room capacity limit enforcement verified." << std::endl;

    // 5. Disconnect Cleanup
    // When player 101 disconnects, room1 is cleaned up
    manager.handleDisconnect(101);
    assert(manager.findRoomByCode(code1) == nullptr && "Room must be destroyed on player disconnect");

    // After cleanup, a new room CAN be created
    Room* roomNew = manager.createRoom(GameType::TICTACTOE_PVP, 601);
    assert(roomNew != nullptr && "Capacity must be freed after room destruction");
    std::cout << "  [PASS] Room cleanup on player disconnect verified." << std::endl;

    std::cout << "[TEST] All RoomManager Lifecycle Tests Passed!" << std::endl;
    return 0;
}
