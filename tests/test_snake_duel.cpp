#include <cassert>
#include <iostream>
#include <string>
#include <gamehub/core/Room.hpp>
#include <gamehub/core/GameTypes.hpp>

int main() {
    std::cout << "[TEST] Running 2-Player Snake Duel Tests at Room Seam..." << std::endl;

    using namespace gamehub::core;

    // 1. Create a Room hosting 2-Player Snake Duel
    Room room("DUEL", GameType::SNAKE_DUEL);
    room.init();
    room.addPlayer(1);
    room.addPlayer(2);

    // 2. Initial state verification: both snakes present
    std::string stateJson = room.serializeState();
    assert(stateJson.find("\"game\":\"snake\"") != std::string::npos);
    assert(stateJson.find("\"duel\":true") != std::string::npos && "State must declare duel=true");
    assert(stateJson.find("\"body\":") != std::string::npos && "State must include Snake 1 body");
    assert(stateJson.find("\"body2\":") != std::string::npos && "State must include Snake 2 body");
    std::cout << "  [PASS] Initial 2-player Snake state serialization verified." << std::endl;

    // 3. Movement and input for both players simultaneously
    // Player 1 turns UP, Player 2 turns DOWN
    room.handleInput(1, "{\"cmd\":\"dir\",\"val\":\"UP\"}");
    room.handleInput(2, "{\"cmd\":\"dir\",\"val\":\"DOWN\"}");
    room.tick(0.066f);

    std::string movedState = room.serializeState();
    // Snake 1 was at (10,10), moved UP -> (10, 9)
    assert(movedState.find("\"head\":[10,9]") != std::string::npos);
    // Snake 2 was at (10,5), moved DOWN -> (10, 6)
    assert(movedState.find("\"head2\":[10,6]") != std::string::npos);
    std::cout << "  [PASS] Simultaneous dual-player input and movement verified." << std::endl;

    // 4. Opponent body collision death
    // Tick again:
    // Snake 1 moves to (10, 8), Snake 2 moves to (10, 7)
    room.tick(0.066f);
    // Tick again:
    // Snake 1 moves to (10, 7) and Snake 2 moves to (10, 8) -> Head-to-head or collision!
    room.tick(0.066f);
    std::string collisionState = room.serializeState();
    assert(room.isFinished() && "Mutual collision must end game");
    assert(room.getWinner() != 0 && "Game must declare a winner or draw");
    std::cout << "  [PASS] Mutual collision resolution verified." << std::endl;

    std::cout << "[TEST] All 2-Player Snake Duel Tests Passed!" << std::endl;
    return 0;
}
