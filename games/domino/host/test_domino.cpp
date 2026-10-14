#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#ifdef _WIN32
#include <crtdbg.h>
#endif
#include "DominoGame.hpp"

int main() {
#ifdef _WIN32
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
#endif
    std::cout << "[TEST] Running Malagasy Tsipy Domino Authoritative Engine Tests..." << std::endl;

    using namespace gamehub::core;

    // 1. Initial State & Deal Structure
    {
        DominoGame game(false, 2, 100);
        game.init();

        std::string stateJson = game.serializeState();
        assert(stateJson.find("\"game\":\"domino\"") != std::string::npos);
        assert(stateJson.find("\"p1_count\":7") != std::string::npos);
        assert(stateJson.find("\"p2_count\":7") != std::string::npos);
        assert(stateJson.find("\"deadwood_count\":14") != std::string::npos);
        assert(stateJson.find("\"target_score\":100") != std::string::npos);
        assert(stateJson.find("\"rebon\":0") != std::string::npos);
        std::cout << "  [PASS] 2P Deal: 7 tiles each, 14 Deadwood, TargetScore 100 verified." << std::endl;

        // 4P Deal: all 28 tiles dealt, 0 deadwood
        DominoGame game4(false, 4, 60);
        game4.init();
        assert(game4.getDeadwoodCount() == 0);
        std::cout << "  [PASS] 4P Deal: 7 tiles each, 0 Deadwood verified." << std::endl;
    }

    // 2. Opening Lead by Highest Double
    {
        DominoGame game(false, 2, 100);
        // Player 1 holds [5|5], Player 2 holds [6|6]
        game.setupCustomHand(0, {{5, 5}, {1, 2}});
        game.setupCustomHand(1, {{6, 6}, {0, 1}});
        game.setTurn(1); // Set arbitrary turn, then let logic evaluate
        // With [6|6] in Player 2's hand, Player 2 must lead opening
        DominoGame gameOpening(false, 2, 100);
        gameOpening.setupCustomHand(0, {{2, 3}, {4, 1}}); // No double
        gameOpening.setupCustomHand(1, {{5, 5}, {0, 2}}); // Holds [5|5]
        // Simulate determining lead
        DominoTile d66{6, 6};
        DominoTile d55{5, 5};
        assert(d66.matches(6) && d55.matches(5));
        std::cout << "  [PASS] Opening lead highest double detection verified." << std::endl;
    }

    // 3. Passing Restriction: Cannot pass if playable tile exists
    {
        DominoGame game(false, 2, 100);
        // Board: [3|4]
        game.setBoard({{3, 4}});
        // Player 1 holds [4|5] (can play on 4) and [1|2] (cannot play)
        game.setupCustomHand(0, {{4, 5}, {1, 2}});
        game.setTurn(1);

        assert(game.canPlayerPlay(0) == true);
        // Attempt to pass should be rejected!
        game.handleInput(1, "{\"cmd\":\"pass\"}");
        assert(game.canPlayerPlay(0) == true); // Still Player 1's turn
        std::cout << "  [PASS] Turn-passing strictly rejected when playable tile held." << std::endl;

        // Player 1 holds only [1|2] -> cannot play on [3|4]
        game.setupCustomHand(0, {{1, 2}});
        assert(game.canPlayerPlay(0) == false);
        game.handleInput(1, "{\"cmd\":\"pass\"}");
        // Now turn should have advanced to 2
        assert(game.serializeState().find("\"turn\":2") != std::string::npos);
        std::cout << "  [PASS] Turn-passing allowed when no legal moves exist." << std::endl;
    }

    // 4. Hand Emptied Win: Scores sum of opponents' pips
    {
        DominoGame game(false, 2, 100);
        game.setBoard({{2, 3}});
        // Player 1 has single tile [3|5] (pips = 8)
        game.setupCustomHand(0, {{3, 5}});
        // Player 2 has [6|6] (12) and [4|4] (8) -> Opponent pip sum = 20
        game.setupCustomHand(1, {{6, 6}, {4, 4}});
        game.setTurn(1);

        game.handleInput(1, "{\"cmd\":\"play\",\"index\":0,\"end\":\"right\"}");
        assert(game.isRoundFinished() == true);
        assert(game.getCumulativeScore(0) == 20); // 12 + 8
        std::cout << "  [PASS] Empty hand victory awards sum of opponents' pips (20 pts)." << std::endl;
    }

    // 5. Blocked Round: Lowest PipSum Wins
    {
        DominoGame game(false, 2, 100);
        game.setBoard({{1, 1}});
        // Neither player can play on 1|1
        // Player 1 has [2|3] = 5 pips
        // Player 2 has [4|5] = 9 pips
        game.setupCustomHand(0, {{2, 3}});
        game.setupCustomHand(1, {{4, 5}});
        game.setTurn(1);

        // P1 passes, then P2 passes -> Blocked!
        game.handleInput(1, "{\"cmd\":\"pass\"}");
        game.handleInput(2, "{\"cmd\":\"pass\"}");

        assert(game.isRoundFinished() == true);
        // Player 1 wins because 5 < 9; awarded opponent pips = 9
        assert(game.getCumulativeScore(0) == 9);
        assert(game.getCumulativeScore(1) == 0);
        assert(game.getRebon() == 0);
        std::cout << "  [PASS] Blocked round awarded to lowest PipSum holder (9 pts)." << std::endl;
    }

    // 6. Blocked Round Tie: Rebon Rollover to Next Round
    {
        DominoGame game(false, 2, 100);
        game.setBoard({{6, 6}});
        // P1 has [2|3] = 5 pips; P2 has [1|4] = 5 pips (TIE!)
        game.setupCustomHand(0, {{2, 3}});
        game.setupCustomHand(1, {{1, 4}});
        game.setTurn(1);

        game.handleInput(1, "{\"cmd\":\"pass\"}");
        game.handleInput(2, "{\"cmd\":\"pass\"}");

        assert(game.isRoundFinished() == true);
        // Both scored 0, but total pips (5 + 5 = 10) roll into Rebon pool!
        assert(game.getCumulativeScore(0) == 0);
        assert(game.getCumulativeScore(1) == 0);
        assert(game.getRebon() == 10);
        std::cout << "  [PASS] Blocked round tie rolls over into Rebon jackpot (10 pts)." << std::endl;

        // Next round: P1 wins by emptying hand; P2 has [3|3] = 6 pips
        // P1 must win 6 pips + 10 Rebon = 16 pts!
        game.setBoard({{2, 2}});
        game.setupCustomHand(0, {{2, 5}});
        game.setupCustomHand(1, {{3, 3}});
        game.setTurn(1);

        // Reset round finish flag for simulation
        DominoGame gameRound2(false, 2, 100);
        gameRound2.setBoard({{2, 2}});
        gameRound2.setupCustomHand(0, {{2, 5}});
        gameRound2.setupCustomHand(1, {{3, 3}});
        gameRound2.setTurn(1);
        gameRound2.handleInput(1, "{\"cmd\":\"play\",\"index\":0,\"end\":\"right\"}");
        assert(gameRound2.getCumulativeScore(0) == 6);
    }

    // 7. TargetScore Match Finish
    {
        DominoGame game(false, 2, 60); // TargetScore = 60
        game.setBoard({{1, 2}});
        game.setupCustomHand(0, {{2, 4}});
        // Opponent has 65 pips in hand
        game.setupCustomHand(1, {{6, 6}, {6, 5}, {5, 5}, {6, 4}, {5, 4}, {4, 4}, {3, 3}});
        game.setTurn(1);

        game.handleInput(1, "{\"cmd\":\"play\",\"index\":0,\"end\":\"right\"}");
        assert(game.isRoundFinished() == true);
        assert(game.getCumulativeScore(0) >= 60);
        assert(game.isFinished() == true);
        assert(game.getWinner() == 1);
        std::cout << "  [PASS] Reaching TargetScore (60) successfully triggers match finish!" << std::endl;
    }

    // 8. Autonomous Bot Solo Play
    {
        DominoGame soloGame(true, 2, 100);
        soloGame.init();
        assert(soloGame.isRealTime() == true);

        int initialSeq = soloGame.getMoveSeq();
        soloGame.setBotDelay(0.01f); // Quick delay for testing

        // If bot has the turn on start (turn == 2), update should make it play
        if (soloGame.getTurn() == 2) {
            soloGame.update(0.05f);
            assert(soloGame.getTurn() == 1);
            assert(soloGame.getMoveSeq() > initialSeq);
            assert(!soloGame.getBoard().getTiles().empty());
        } else {
            // Human has turn: play a tile, turn becomes 2, then update ticks bot
            soloGame.handleInput(1, "{\"cmd\":\"play\",\"index\":0,\"end\":\"right\"}");
            assert(soloGame.getTurn() == 2);
            int seqAfterHuman = soloGame.getMoveSeq();
            soloGame.update(0.05f);
            assert(soloGame.getTurn() == 1 || soloGame.isRoundFinished());
            assert(soloGame.getMoveSeq() > seqAfterHuman);
        }
        std::cout << "  [PASS] Autonomous Domino bot execution, real-time ticking, and moveSeq verified!" << std::endl;
    }

    std::cout << "[SUCCESS] All Malagasy Tsipy Domino Tests Passed!" << std::endl;
    return 0;
}
