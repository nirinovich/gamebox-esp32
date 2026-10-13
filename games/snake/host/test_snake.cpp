#include <cassert>
#include <iostream>
#include <string>
#include "SnakeGame.hpp"

int main() {
    std::cout << "[TEST] Running Co-located Snake Unit Tests at Game Boundary..." << std::endl;

    using namespace gamehub::core;

    SnakeGame game(false);
    game.init();

    // 1. Initial State
    std::string stateJson = game.serializeState();
    assert(stateJson.find("\"game\":\"snake\"") != std::string::npos && "Game state must identify as snake");
    assert(stateJson.find("\"score\":0") != std::string::npos && "Initial score must be 0");
    assert(stateJson.find("\"finished\":false") != std::string::npos && "Game must not be finished initially");
    std::cout << "  [PASS] Initial state verified." << std::endl;

    // 2. Tick movement (starts at 10,10 moving RIGHT)
    game.update(0.12f);
    std::string movedState = game.serializeState();
    assert(movedState.find("\"head\":[11,10]") != std::string::npos && "Snake must have moved right to (11, 10)");
    std::cout << "  [PASS] Tick movement verified." << std::endl;

    // 3. Direction turn (DOWN)
    game.handleInput(1, "{\"cmd\":\"dir\",\"val\":\"DOWN\"}");
    game.update(0.12f);
    std::string downState = game.serializeState();
    assert(downState.find("\"head\":[11,11]") != std::string::npos && "Snake must have moved down to (11, 11)");
    std::cout << "  [PASS] Direction input handling verified." << std::endl;

    // 4. 180-degree reversal prevention (cannot turn UP when moving DOWN)
    game.handleInput(1, "{\"cmd\":\"dir\",\"val\":\"UP\"}");
    game.update(0.12f);
    std::string blockedState = game.serializeState();
    assert(blockedState.find("\"head\":[11,12]") != std::string::npos && "Snake must ignore 180-degree reversal");
    std::cout << "  [PASS] Reversal rejection verified." << std::endl;

    // 5. Wall collision
    game.handleInput(1, "{\"cmd\":\"dir\",\"val\":\"RIGHT\"}");
    for (int i = 0; i < 20; ++i) {
        game.update(0.12f);
    }
    std::string deadState = game.serializeState();
    assert(deadState.find("\"finished\":true") != std::string::npos && "Wall collision must finish game");
    std::cout << "  [PASS] Wall collision verified." << std::endl;

    std::cout << "[TEST] All Co-located Snake Unit Tests Passed!" << std::endl;
    return 0;
}
