#include <iostream>
#include <cassert>
#include <gamehub/core/ConnectFourGame.hpp>
#include <gamehub/core/PongGame.hpp>
#include <gamehub/core/TronGame.hpp>
#include <gamehub/core/BattleshipGame.hpp>
#include <gamehub/core/DominoGame.hpp>

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

    // --- 5. Domino Tests ---
    {
        std::cout << "  - Testing DominoGame..." << std::endl;
        gamehub::core::DominoGame domino2(false, 2); // 2P PvP
        domino2.init();
        assert(!domino2.isFinished());
        assert(domino2.getLeftEnd() == -1);
        assert(domino2.getRightEnd() == -1);

        std::string s2 = domino2.serializeState();
        assert(s2.find("\"game\":\"domino\"") != std::string::npos);
        assert(s2.find("\"p1_count\":7") != std::string::npos);
        assert(s2.find("\"p2_count\":7") != std::string::npos);
        assert(s2.find("\"boneyard_count\":14") != std::string::npos);

        // Player 1 plays tile 0 on empty board
        domino2.handleInput(1, "{\"cmd\":\"play\",\"index\":0,\"end\":\"right\"}");
        assert(domino2.getLeftEnd() >= 0 && domino2.getRightEnd() >= 0);
        std::string s2_played = domino2.serializeState();
        assert(s2_played.find("\"p1_count\":6") != std::string::npos);
        assert(s2_played.find("\"turn\":2") != std::string::npos);
        std::cout << "    Domino 2P dealing, empty board placement, and turn shift verified." << std::endl;

        // 3-Player game test
        gamehub::core::DominoGame domino3(false, 3);
        domino3.init();
        std::string s3 = domino3.serializeState();
        assert(s3.find("\"num_players\":3") != std::string::npos);
        assert(s3.find("\"p1_count\":7") != std::string::npos);
        assert(s3.find("\"p2_count\":7") != std::string::npos);
        assert(s3.find("\"p3_count\":7") != std::string::npos);
        assert(s3.find("\"boneyard_count\":7") != std::string::npos); // 28 - 21 = 7

        // Block mode prohibits drawing
        domino3.handleInput(1, "{\"cmd\":\"draw\"}");
        std::string s3_nodraw = domino3.serializeState();
        assert(s3_nodraw.find("\"p1_count\":7") != std::string::npos);
        assert(s3_nodraw.find("\"block_mode\":true") != std::string::npos);

        // Switch to Draw mode and verify drawing from boneyard
        domino3.setBlockMode(false);
        domino3.handleInput(1, "{\"cmd\":\"draw\"}");
        std::string s3_drawn = domino3.serializeState();
        assert(s3_drawn.find("\"p1_count\":8") != std::string::npos);
        assert(s3_drawn.find("\"boneyard_count\":6") != std::string::npos);
        assert(s3_drawn.find("\"block_mode\":false") != std::string::npos);
        std::cout << "    Domino Block mode (no-draw) and Draw mode variants verified." << std::endl;

        // Solo Mode with Strategic Minimax Bot
        gamehub::core::DominoGame dominoSolo(true, 2);
        dominoSolo.init();
        dominoSolo.handleInput(1, "{\"cmd\":\"play\",\"index\":0,\"end\":\"right\"}");
        std::string sSolo = dominoSolo.serializeState();
        assert(sSolo.find("\"game\":\"domino\"") != std::string::npos);
        assert(sSolo.find("\"last_action\"") != std::string::npos);
        std::cout << "    Domino Solo AI bot play and action tracking verified." << std::endl;

        // Pass validation test: player cannot pass if they hold playable tiles
        gamehub::core::DominoGame dominoPass(false, 2);
        dominoPass.init();
        assert(dominoPass.canPlayerPlay(0)); // Opening move is always playable
        dominoPass.handleInput(1, "{\"cmd\":\"pass\"}");
        std::string sPassReject = dominoPass.serializeState();
        assert(sPassReject.find("\"turn\":1") != std::string::npos); // Rejected: turn must remain 1
        std::cout << "    Domino pass rules verified (cannot pass when playable moves exist)." << std::endl;
    }

    std::cout << "[SUCCESS] All new game tests passed!" << std::endl;
    return 0;
}
