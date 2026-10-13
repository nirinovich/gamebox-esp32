#include <cassert>
#include <iostream>
#include <string>
#include "ConnectFourGame.hpp"

int main() {
    std::cout << "[TEST] Running Co-located Connect Four Unit Tests at Game Boundary..." << std::endl;

    using namespace gamehub::core;

    ConnectFourGame game(true);
    game.init();

    // 1. Initial State
    std::string stateJson = game.serializeState();
    assert(stateJson.find("\"game\":\"connect4\"") != std::string::npos && "Game state must identify as connect4");
    assert(stateJson.find("\"turn\":1") != std::string::npos && "Turn must start with Player 1");
    assert(stateJson.find("\"finished\":false") != std::string::npos && "Game must start ongoing");
    std::cout << "  [PASS] Initial state verified." << std::endl;

    // 2. Drop piece in column 3 (center) - human moves, then bot responds
    game.handleInput(1, "{\"cmd\":\"drop\",\"col\":3}");
    std::string afterMove = game.serializeState();

    // Count occurrences of 1s and 2s in board
    size_t count1 = 0, count2 = 0;
    auto boardPos = afterMove.find("\"board\":[");
    assert(boardPos != std::string::npos);
    auto endBoard = afterMove.find("]", boardPos);
    std::string boardStr = afterMove.substr(boardPos, endBoard - boardPos);
    for (char c : boardStr) {
        if (c == '1') count1++;
        if (c == '2') count2++;
    }
    assert(count1 == 1 && "Player 1 piece should be on board");
    assert(count2 == 1 && "Bot should have responded with piece");
    std::cout << "  [PASS] Piece drop and bot response verified." << std::endl;

    std::cout << "[TEST] All Co-located Connect Four Unit Tests Passed!" << std::endl;
    return 0;
}
