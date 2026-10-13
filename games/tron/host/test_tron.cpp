#include <cassert>
#include <iostream>
#include <string>
#include "TronGame.hpp"

int main() {
    std::cout << "[TEST] Running Co-located Tron Unit Tests at Game Boundary..." << std::endl;

    using namespace gamehub::core;

    TronGame game(false);
    game.init();

    // 1. Initial State
    std::string stateJson = game.serializeState();
    assert(stateJson.find("\"game\":\"tron\"") != std::string::npos && "Game state must identify as tron");
    assert(stateJson.find("\"finished\":false") != std::string::npos && "Game must start ongoing");
    std::cout << "  [PASS] Initial state verified." << std::endl;

    // 2. Tick movement (P1 starts heading right)
    game.update(0.10f);
    std::string movedState = game.serializeState();
    assert(movedState.find("\"p1\":{\"x\":3,\"y\":15") != std::string::npos && "P1 must have advanced right to (3, 15)");
    std::cout << "  [PASS] Tick movement verified." << std::endl;

    // 3. Direction change
    game.handleInput(1, "{\"cmd\":\"dir\",\"val\":\"DOWN\"}");
    game.update(0.10f);
    std::string downState = game.serializeState();
    assert(downState.find("\"p1\":{\"x\":3,\"y\":16") != std::string::npos && "P1 must have turned down to (3, 16)");
    std::cout << "  [PASS] Direction input handling verified." << std::endl;

    // 4. Reversal rejection (cannot turn UP when moving DOWN)
    game.handleInput(1, "{\"cmd\":\"dir\",\"val\":\"UP\"}");
    game.update(0.10f);
    std::string blockedState = game.serializeState();
    assert(blockedState.find("\"p1\":{\"x\":3,\"y\":17") != std::string::npos && "P1 must ignore 180-degree reversal");
    std::cout << "  [PASS] Reversal rejection verified." << std::endl;

    std::cout << "[TEST] All Co-located Tron Unit Tests Passed!" << std::endl;
    return 0;
}
