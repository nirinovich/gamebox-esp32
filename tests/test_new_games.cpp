#include <iostream>
#include <cassert>
#include <gamehub/core/ConnectFourGame.hpp>
#include <gamehub/core/PongGame.hpp>
#include <gamehub/core/TronGame.hpp>
#include <gamehub/core/BattleshipGame.hpp>

int main() {
    std::cout << "[TEST] Running New Games test suite..." << std::endl;

    // --- 1. Connect Four Tests ---
    {
        std::cout << "  - Testing ConnectFourGame..." << std::endl;
        gamehub::core::ConnectFourGame c4(false); // 2P PvP
        c4.init();
        assert(!c4.isFinished());
        assert(c4.getWinner() == 0);

        // Player 1 drops in col 0, Player 2 in col 1
        c4.handleInput(1, "{\"cmd\":\"drop\",\"col\":0}");
        c4.handleInput(2, "{\"cmd\":\"drop\",\"col\":1}");

        // Connect 4 horizontal for P1
        c4.handleInput(1, "{\"cmd\":\"drop\",\"col\":0}");
        c4.handleInput(2, "{\"cmd\":\"drop\",\"col\":1}");
        c4.handleInput(1, "{\"cmd\":\"drop\",\"col\":0}");
        c4.handleInput(2, "{\"cmd\":\"drop\",\"col\":1}");
        c4.handleInput(1, "{\"cmd\":\"drop\",\"col\":0}"); // P1 gets 4 vertically in col 0!

        assert(c4.isFinished());
        assert(c4.getWinner() == 1);
        std::cout << "    ConnectFour vertical win verified." << std::endl;

        // Test Minimax AI in solo mode
        gamehub::core::ConnectFourGame c4_solo(true);
        c4_solo.init();
        c4_solo.handleInput(1, "{\"cmd\":\"drop\",\"col\":3}");
        // Bot should have moved automatically
        std::string state = c4_solo.serializeState();
        assert(state.find("\"game\":\"connect4\"") != std::string::npos);
        std::cout << "    ConnectFour Solo Minimax response verified." << std::endl;
    }

    // --- 2. Pong Tests ---
    {
        std::cout << "  - Testing PongGame..." << std::endl;
        gamehub::core::PongGame pong;
        pong.init();
        assert(!pong.isFinished());
        assert(pong.getWinner() == 0);

        // Paddle movement
        pong.handleInput(1, "{\"cmd\":\"dir\",\"val\":\"DOWN\"}");
        pong.handleInput(2, "{\"cmd\":\"dir\",\"val\":\"UP\"}");
        pong.update(0.1f);

        std::string state = pong.serializeState();
        assert(state.find("\"game\":\"pong\"") != std::string::npos);
        assert(state.find("\"p1y\":") != std::string::npos);
        std::cout << "    Pong movement and physics verified." << std::endl;
    }

    // --- 3. Tron Tests ---
    {
        std::cout << "  - Testing TronGame..." << std::endl;
        gamehub::core::TronGame tron;
        tron.init();
        assert(!tron.isFinished());

        // Step simulation forward
        for (int i = 0; i < 5; ++i) {
            tron.update(0.1f);
        }
        std::string state = tron.serializeState();
        assert(state.find("\"game\":\"tron\"") != std::string::npos);
        std::cout << "    Tron light cycle stepping verified." << std::endl;
    }

    // --- 4. Battleship Tests ---
    {
        std::cout << "  - Testing BattleshipGame..." << std::endl;
        gamehub::core::BattleshipGame bship;
        bship.init();
        assert(!bship.isFinished());

        // Auto-place fleets for both players
        bship.handleInput(1, "{\"cmd\":\"auto_place\"}");
        bship.handleInput(2, "{\"cmd\":\"auto_place\"}");

        std::string state = bship.serializeState();
        assert(state.find("\"phase\":\"battle\"") != std::string::npos);
        assert(state.find("\"p1ready\":true") != std::string::npos);
        assert(state.find("\"p2ready\":true") != std::string::npos);

        // Turn-based firing
        bship.handleInput(1, "{\"cmd\":\"fire\",\"row\":0,\"col\":0}");
        std::string battleState = bship.serializeState();
        assert(battleState.find("\"turn\":2") != std::string::npos);
        std::cout << "    Battleship fleet deployment and battle firing verified." << std::endl;
    }

    std::cout << "[SUCCESS] All new game tests passed!" << std::endl;
    return 0;
}
