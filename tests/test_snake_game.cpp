#include <cassert>
#include <iostream>
#include <string>
#include <gamehub/core/Room.hpp>
#include <gamehub/core/GameTypes.hpp>

int main() {
    std::cout << "[TEST] Running Solo Snake Game Tests at Room Seam..." << std::endl;

    using namespace gamehub::core;

    // 1. Create a Room hosting Solo Snake
    Room room("SOLO", GameType::SNAKE_SOLO);
    room.init();

    // 2. Verify initial serialized state
    std::string stateJson = room.serializeState();
    assert(stateJson.find("\"game\":\"snake\"") != std::string::npos && "Game state must identify as snake");
    assert(stateJson.find("\"score\":0") != std::string::npos && "Initial score must be 0");
    assert(stateJson.find("\"finished\":false") != std::string::npos && "Game must not be finished initially");
    std::cout << "  [PASS] Initial Snake state serialization verified." << std::endl;

    // 3. Test movement on Tick
    // Snake starts at (10, 10) moving RIGHT by default
    room.tick(0.1f);
    std::string movedState = room.serializeState();
    assert(movedState.find("\"head\":[11,10]") != std::string::npos && "Snake must have moved right to (11, 10)");
    std::cout << "  [PASS] Snake tick movement verified." << std::endl;

    // 4. Test direction input handling
    // Turn DOWN
    room.handleInput(1, "{\"cmd\":\"dir\",\"val\":\"DOWN\"}");
    room.tick(0.1f);
    std::string downState = room.serializeState();
    assert(downState.find("\"head\":[11,11]") != std::string::npos && "Snake must have moved down to (11, 11)");
    std::cout << "  [PASS] Snake directional input handling verified." << std::endl;

    // 5. Test 180-degree reversal prevention (cannot turn UP when moving DOWN)
    room.handleInput(1, "{\"cmd\":\"dir\",\"val\":\"UP\"}");
    room.tick(0.1f);
    std::string blockedState = room.serializeState();
    assert(blockedState.find("\"head\":[11,12]") != std::string::npos && "Snake must ignore 180-degree reverse and continue down");
    std::cout << "  [PASS] 180-degree reversal rejection verified." << std::endl;

    // 6. Test wall collision game over
    // Turn RIGHT and run into right wall (grid width is 20, x goes 0..19)
    room.handleInput(1, "{\"cmd\":\"dir\",\"val\":\"RIGHT\"}");
    for (int i = 0; i < 20; ++i) {
        room.tick(0.1f);
    }
    std::string deadState = room.serializeState();
    assert(deadState.find("\"finished\":true") != std::string::npos && "Snake hitting wall must trigger finished=true");
    std::cout << "  [PASS] Wall collision death verified." << std::endl;

    std::cout << "[TEST] All Solo Snake Tests Passed!" << std::endl;
    return 0;
}
