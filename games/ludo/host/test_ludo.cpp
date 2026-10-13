#include <iostream>
#undef NDEBUG
#include <cassert>
#include <string>
#include "LudoGame.hpp"

#ifdef _WIN32
#include <crtdbg.h>
#include <stdlib.h>
#endif

int main() {
#ifdef _WIN32
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
#endif

    std::cout << "[TEST] Running Ludo Authoritative Board Engine Tests..." << std::endl;

    using namespace gamehub::core;

    // 1. Initial State: Quick (2 pawns) vs Classic (4 pawns)
    {
        LudoGame quickGame(false, 4, true);
        quickGame.init();
        assert(quickGame.getPawnCount() == 2);
        assert(quickGame.isQuick() == true);
        assert(quickGame.getCurrentTurn() == 1);
        assert(quickGame.getPlayer(0).pawns[0].state == PawnState::IN_YARD);
        assert(quickGame.getPlayer(0).pawns[1].state == PawnState::IN_YARD);

        std::string qState = quickGame.serializeState();
        assert(qState.find("\"game\":\"ludo\"") != std::string::npos);
        assert(qState.find("\"is_quick\":true") != std::string::npos);
        assert(qState.find("\"pawn_count\":2") != std::string::npos);

        LudoGame classicGame(false, 4, false);
        classicGame.init();
        assert(classicGame.getPawnCount() == 4);
        assert(classicGame.isQuick() == false);
        assert(classicGame.getPlayer(0).pawns[3].state == PawnState::IN_YARD);

        std::cout << "  [PASS] Initial state (Quick 2 pawns vs Classic 4 pawns) verified." << std::endl;
    }

    // 2. Yard Rollout on 6 & Non-6 Rejection
    {
        LudoGame game(false, 2, true);
        game.init();

        // Roll 5: cannot exit yard!
        game.setDiceForTesting(5);
        assert(game.canPawnMove(0, 0, 5) == false);
        assert(game.getLegalMovesCount(0, 5) == 0);

        // Roll 6: can exit yard!
        game.setDiceForTesting(6);
        assert(game.canPawnMove(0, 0, 6) == true);
        assert(game.getLegalMovesCount(0, 6) == 2);

        // Move pawn 0 out
        game.handleInput(1, "{\"cmd\":\"move\",\"pawn\":0}");
        assert(game.getPlayer(0).pawns[0].state == PawnState::ON_TRACK);
        assert(game.getPlayer(0).pawns[0].step == 0);
        assert(game.getPlayer(0).pawns[0].getTrackCell(0) == 0); // Red start cell

        // Rolling 6 grants bonus roll (still Player 1's turn, diceRolled reset)
        assert(game.getCurrentTurn() == 1);
        assert(game.isDiceRolled() == false);

        std::cout << "  [PASS] Yard rollout on 6 with bonus roll verified." << std::endl;
    }

    // 3. Three Consecutive 6s Forfeiture
    {
        LudoGame game(false, 2, true);
        game.init();

        game.setConsecutiveSixesForTesting(2); // Already rolled two 6s
        game.rollDiceForTesting(6); // Third 6 rolled!

        // Turn must pass to Player 2
        assert(game.getCurrentTurn() == 2);
        assert(game.getConsecutiveSixes() == 0);

        std::cout << "  [PASS] Three consecutive 6s forfeiture verified." << std::endl;
    }

    // 4. SafeSquare Immunity (No Capture on Star Cells)
    {
        LudoGame game(false, 2, true);
        game.init();

        // Star squares: {0, 8, 13, 21, 26, 34, 39, 47}
        assert(LudoGame::isSafeSquare(8) == true);
        assert(LudoGame::isSafeSquare(13) == true);
        assert(LudoGame::isSafeSquare(7) == false);

        // P1 has pawn on step 8 (track cell 8, which is a Star SafeSquare)
        game.setPawnStepForTesting(0, 0, 8);
        assert(game.getPlayer(0).pawns[0].getTrackCell(0) == 8);

        // P2 (Green, starts at 13): puts pawn on track cell 8 (which is step (8 - 13 + 52) % 52 = 47)
        game.setPawnStepForTesting(1, 0, 47);
        assert(game.getPlayer(1).pawns[0].getTrackCell(13) == 8);

        // P1 advances to cell 8 where P2 is already sitting: NO capture because cell 8 is a SafeSquare!
        game.setPawnStepForTesting(0, 1, 6);
        game.setCurrentTurnForTesting(1);
        game.setDiceForTesting(2); // 6 + 2 = step 8
        game.handleInput(1, "{\"cmd\":\"move\",\"pawn\":1}");

        // P2's pawn must still be ON_TRACK, not returned to yard!
        assert(game.getPlayer(1).pawns[0].state == PawnState::ON_TRACK);
        assert(game.getPlayer(1).pawns[0].step == 47);

        std::cout << "  [PASS] Star SafeSquare immunity verified." << std::endl;
    }

    // 5. Normal Cell Collision Capture & Bonus Roll
    {
        LudoGame game(false, 2, true);
        game.init();

        // Track cell 10 is NOT a safe square
        assert(LudoGame::isSafeSquare(10) == false);

        // P2 (Green, start 13) has pawn on cell 10 (step (10 - 13 + 52) % 52 = 49)
        game.setPawnStepForTesting(1, 0, 49);
        assert(game.getPlayer(1).pawns[0].getTrackCell(13) == 10);

        // P1 (Red, start 0) has pawn on cell 7 (step 7)
        game.setPawnStepForTesting(0, 0, 7);
        game.setCurrentTurnForTesting(1);
        game.setDiceForTesting(3); // 7 + 3 = 10 -> Lands on P2!

        game.handleInput(1, "{\"cmd\":\"move\",\"pawn\":0}");

        // P2's pawn must be CAPTURED back to yard!
        assert(game.getPlayer(1).pawns[0].state == PawnState::IN_YARD);
        assert(game.getPlayer(1).pawns[0].step == -1);

        // P1 earns a bonus roll for capture! (Still P1's turn)
        assert(game.getCurrentTurn() == 1);
        assert(game.isDiceRolled() == false);

        std::cout << "  [PASS] Non-safe collision capture and bonus roll verified." << std::endl;
    }

    // 6. Home Stretch & Exact Roll Victory
    {
        LudoGame game(false, 2, true); // Quick match (2 pawns)
        game.init();

        // P1 pawn 0 is already FINISHED
        game.setPawnStepForTesting(0, 0, 56);
        assert(game.getPlayer(0).finishedCount == 1);

        // P1 pawn 1 is at step 54 (Home stretch)
        game.setPawnStepForTesting(0, 1, 54);
        game.setCurrentTurnForTesting(1);

        // Roll 3: 54 + 3 = 57 > 56 -> Overshoot! Cannot move!
        assert(game.canPawnMove(0, 1, 3) == false);

        // Roll 2: 54 + 2 = 56 -> Exact roll to Goal!
        assert(game.canPawnMove(0, 1, 2) == true);

        game.setDiceForTesting(2);
        game.handleInput(1, "{\"cmd\":\"move\",\"pawn\":1}");

        // Both pawns are finished: MATCH WON!
        assert(game.getPlayer(0).pawns[1].state == PawnState::FINISHED);
        assert(game.isFinished() == true);
        assert(game.getWinner() == 1);

        std::cout << "  [PASS] Exact roll Home finish and Quick match victory verified." << std::endl;
    }

    // 7. Tactical Bot Solo Play
    {
        LudoGame soloGame(true, 4, true);
        soloGame.init();
        soloGame.setBotDelay(0.01f);

        assert(soloGame.getPlayer(0).isBot == false);
        assert(soloGame.getPlayer(1).isBot == true);
        assert(soloGame.getPlayer(2).isBot == true);
        assert(soloGame.getPlayer(3).isBot == true);

        // Advance ticks: bots roll and move autonomously without hanging
        for (int i = 0; i < 40; ++i) {
            soloGame.update(0.05f);
        }

        std::cout << "  [PASS] Solo mode vs 3 tactical bots verified." << std::endl;
    }

    std::cout << "[SUCCESS] All Ludo Host Engine tests passed!" << std::endl;
    return 0;
}
