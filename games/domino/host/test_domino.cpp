#include <cassert>
#include <iostream>
#include <string>
#include "DominoGame.hpp"

int main() {
    std::cout << "[TEST] Running Co-located Domino Unit Tests at Game Boundary..." << std::endl;

    using namespace gamehub::core;

    DominoGame game(true, 2, false);
    game.init();

    // 1. Initial State
    std::string stateJson = game.serializeState();
    assert(stateJson.find("\"game\":\"domino\"") != std::string::npos && "Game state must identify as domino");
    assert(stateJson.find("\"p1_count\":7") != std::string::npos && "Player 1 must start with 7 tiles");
    assert(stateJson.find("\"p2_count\":7") != std::string::npos && "Player 2 must start with 7 tiles");
    assert(stateJson.find("\"boneyard_count\":14") != std::string::npos && "Boneyard must have 14 tiles (28 - 14)");
    std::cout << "  [PASS] Initial deal and Boneyard count verified." << std::endl;

    // 2. Play first tile (index 0 in hand)
    assert(game.canPlayerPlay(0) && "Player 1 must be able to play on empty board");
    game.handleInput(1, "{\"cmd\":\"play\",\"index\":0,\"end\":\"right\"}");
    std::string afterPlay = game.serializeState();
    assert(afterPlay.find("\"p1_count\":6") != std::string::npos && "Player 1 should now have 6 tiles");
    std::cout << "  [PASS] Tile placement and hand reduction verified." << std::endl;

    // 3. Perspective filtering
    std::string p1Perspective = game.serializeStateForPlayer(1);
    assert(p1Perspective.find("\"1\":[") != std::string::npos && "Player 1 should see own tiles");
    assert(p1Perspective.find("\"2\":[") == std::string::npos && "Player 1 must NOT see Player 2 tiles");
    std::cout << "  [PASS] Hand privacy (Fog-of-War) verified." << std::endl;

    std::cout << "[TEST] All Co-located Domino Unit Tests Passed!" << std::endl;
    return 0;
}
