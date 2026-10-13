#include <cassert>
#include <iostream>
#include <string>
#include "PongGame.hpp"

int main() {
    std::cout << "[TEST] Running Co-located Pong Unit Tests at Game Boundary..." << std::endl;

    using namespace gamehub::core;

    PongGame game(false);
    game.init();

    // 1. Initial State
    std::string stateJson = game.serializeState();
    assert(stateJson.find("\"game\":\"pong\"") != std::string::npos && "Game state must identify as pong");
    assert(stateJson.find("\"score1\":0") != std::string::npos && "Score 1 must start at 0");
    assert(stateJson.find("\"score2\":0") != std::string::npos && "Score 2 must start at 0");
    assert(stateJson.find("\"finished\":false") != std::string::npos && "Game must start ongoing");
    std::cout << "  [PASS] Initial state verified." << std::endl;

    // 2. Paddle movement
    game.handleInput(1, "{\"cmd\":\"dir\",\"val\":\"UP\"}");
    game.update(0.1f);
    std::string moveUpState = game.serializeState();
    assert(moveUpState.find("\"p1y\":180.0") != std::string::npos && "Paddle 1 should move up by 20 units");
    std::cout << "  [PASS] Paddle movement verified." << std::endl;

    // 3. Stop movement
    game.handleInput(1, "{\"cmd\":\"dir\",\"val\":\"STOP\"}");
    game.update(0.1f);
    std::string stopState = game.serializeState();
    assert(stopState.find("\"p1y\":180.0") != std::string::npos && "Paddle 1 should stay stationary on STOP");
    std::cout << "  [PASS] Paddle stop verified." << std::endl;

    std::cout << "[TEST] All Co-located Pong Unit Tests Passed!" << std::endl;
    return 0;
}
