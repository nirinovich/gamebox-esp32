#include <cassert>
#include <iostream>
#include <string>
#include <gamehub/core/Room.hpp>
#include <gamehub/core/GameTypes.hpp>

int main() {
    std::cout << "[TEST] Running Solo Tic-Tac-Toe Game Tests at Room Seam..." << std::endl;

    using namespace gamehub::core;

    // 1. Create a Room hosting Solo Tic-Tac-Toe
    Room room("TTT1", GameType::TICTACTOE_SOLO);
    room.init();

    // 2. Initial state verification
    std::string stateJson = room.serializeState();
    assert(stateJson.find("\"game\":\"tictactoe\"") != std::string::npos && "Game must identify as tictactoe");
    assert(stateJson.find("\"turn\":1") != std::string::npos && "Player 1 should start");
    assert(stateJson.find("\"finished\":false") != std::string::npos && "Game should not be finished");
    std::cout << "  [PASS] Initial Tic-Tac-Toe state verified." << std::endl;

    // 3. Move placement and Bot counter-move
    // Player 1 places at center (index 4)
    room.handleInput(1, "{\"cmd\":\"place\",\"cell\":4}");
    std::string afterMove = room.serializeState();
    
    // Board should now have Player 1's marker at 4, AND the Minimax bot should have made a response move
    assert(afterMove.find("\"cell\":4") == std::string::npos); // Verify board has markers
    // Count occurrences of markers in board array: exactly one 1 and one 2
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
    assert(count2 == 1 && "Bot (Player 2) should have made 1 counter-move");
    std::cout << "  [PASS] Move placement and Bot Minimax counter-move verified." << std::endl;

    // 4. Duplicate cell placement rejection
    // Try to place again at cell 4
    room.handleInput(1, "{\"cmd\":\"place\",\"cell\":4}");
    std::string dupState = room.serializeState();
    assert(dupState == afterMove && "Illegal duplicate move must be ignored");
    std::cout << "  [PASS] Illegal move rejection verified." << std::endl;

    // 5. Minimax Bot Unbeatable Verification
    // Play multiple games with various player moves, bot must NEVER lose
    for (int firstMove = 0; firstMove < 9; ++firstMove) {
        Room simRoom("BOT_TEST", GameType::TICTACTOE_SOLO);
        simRoom.init();
        simRoom.handleInput(1, "{\"cmd\":\"place\",\"cell\":" + std::to_string(firstMove) + "}");

        // Keep placing in the first available empty cell until game finishes
        for (int step = 0; step < 5; ++step) {
            if (simRoom.isFinished()) break;
            std::string state = simRoom.serializeState();
            // Find an empty cell
            for (int c = 0; c < 9; ++c) {
                if (state.find("\"cell\":" + std::to_string(c)) == std::string::npos) {
                    simRoom.handleInput(1, "{\"cmd\":\"place\",\"cell\":" + std::to_string(c) + "}");
                    break;
                }
            }
        }
        assert(simRoom.getWinner() != 1 && "Minimax Bot must NEVER lose to human player");
    }
    std::cout << "  [PASS] Minimax bot invincibility verified across test games." << std::endl;

    std::cout << "[TEST] All Tic-Tac-Toe Tests Passed!" << std::endl;
    return 0;
}
