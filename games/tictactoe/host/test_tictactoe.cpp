#include <cassert>
#include <iostream>
#include <string>
#include "TicTacToeGame.hpp"

int main() {
    std::cout << "[TEST] Running Co-located Tic-Tac-Toe Unit Tests at Game Boundary..." << std::endl;

    using namespace gamehub::core;

    TicTacToeGame game(true);
    game.init();

    // 1. Initial State
    std::string stateJson = game.serializeState();
    assert(stateJson.find("\"game\":\"tictactoe\"") != std::string::npos && "Game must identify as tictactoe");
    assert(stateJson.find("\"turn\":1") != std::string::npos && "Player 1 should start");
    assert(stateJson.find("\"finished\":false") != std::string::npos && "Game should not be finished");
    std::cout << "  [PASS] Initial state verified." << std::endl;

    // 2. Move placement and Bot counter-move
    game.handleInput(1, "{\"cmd\":\"place\",\"cell\":4}");
    std::string afterMove = game.serializeState();

    size_t count1 = 0, count2 = 0;
    auto boardPos = afterMove.find("\"board\":[");
    assert(boardPos != std::string::npos);
    auto endBoard = afterMove.find("]", boardPos);
    std::string boardStr = afterMove.substr(boardPos, endBoard - boardPos);
    for (char c : boardStr) {
        if (c == '1') count1++;
        if (c == '2') count2++;
    }
    assert(count1 == 1 && "Player 1 should have 1 marker on board");
    assert(count2 == 1 && "Bot should have made 1 counter-move");
    std::cout << "  [PASS] Move placement and Bot counter-move verified." << std::endl;

    // 3. Duplicate move rejected
    game.handleInput(1, "{\"cmd\":\"place\",\"cell\":4}");
    std::string dupState = game.serializeState();
    assert(dupState == afterMove && "Illegal duplicate move must be ignored");
    std::cout << "  [PASS] Duplicate move rejection verified." << std::endl;

    // 4. Minimax Bot Never Loses
    for (int firstMove = 0; firstMove < 9; ++firstMove) {
        TicTacToeGame simGame(true);
        simGame.init();
        simGame.handleInput(1, "{\"cmd\":\"place\",\"cell\":" + std::to_string(firstMove) + "}");

        while (!simGame.isFinished()) {
            const auto& board = simGame.getBoard();
            int move = -1;
            for (int i = 0; i < 9; ++i) {
                if (board[i] == 0) {
                    move = i;
                    break;
                }
            }
            if (move == -1) break;
            simGame.handleInput(1, "{\"cmd\":\"place\",\"cell\":" + std::to_string(move) + "}");
        }

        assert(simGame.getWinner() != 1 && "Bot must never lose against any strategy");
    }
    std::cout << "  [PASS] Minimax bot defense verified." << std::endl;

    std::cout << "[TEST] All Co-located Tic-Tac-Toe Unit Tests Passed!" << std::endl;
    return 0;
}
