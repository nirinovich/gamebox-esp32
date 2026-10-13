#include <cassert>
#include <iostream>
#include <string>
#include "BattleshipGame.hpp"

int main() {
    std::cout << "[TEST] Running Co-located Battleship Unit Tests at Game Boundary..." << std::endl;

    using namespace gamehub::core;

    BattleshipGame game(false);
    game.init();

    // 1. Initial State in placement phase
    std::string stateJson = game.serializeState();
    assert(stateJson.find("\"game\":\"battleship\"") != std::string::npos && "Game state must identify as battleship");
    assert(stateJson.find("\"phase\":\"placement\"") != std::string::npos && "Phase must start in placement");
    std::cout << "  [PASS] Initial placement phase verified." << std::endl;

    // 2. Auto-place for Player 1 and Player 2
    game.handleInput(1, "{\"cmd\":\"auto_place\"}");
    game.handleInput(2, "{\"cmd\":\"auto_place\"}");
    std::string battleState = game.serializeState();
    assert(battleState.find("\"phase\":\"battle\"") != std::string::npos && "Both players ready must transition to battle phase");
    std::cout << "  [PASS] Auto-placement and phase transition verified." << std::endl;

    // 3. Perspective filtering (Fog-of-War)
    std::string p1Perspective = game.serializeStateForPlayer(1);
    // Player 1 sees own ships unmasked in p1.own, but in p2 (enemy) unhit ships are masked (value 0, not 1)
    auto p2Pos = p1Perspective.find("\"p2\":{");
    assert(p2Pos != std::string::npos);
    std::string p2Sub = p1Perspective.substr(p2Pos);
    // Before firing, p2.own should not contain value '1'
    assert(p2Sub.find("1") == std::string::npos && "Player 1 must NOT see Player 2 unhit ships (Fog-of-War)");
    std::cout << "  [PASS] Player-specific perspective filtering (Fog-of-War) verified." << std::endl;

    std::cout << "[TEST] All Co-located Battleship Unit Tests Passed!" << std::endl;
    return 0;
}
