#include <cassert>
#include <iostream>
#include <string>
#include <algorithm>
#include "FanoronaGame.hpp"

int main() {
    std::cout << "[TEST] Running Fanorona Malagasy Unit Tests..." << std::endl;
    using namespace gamehub::core;

    // ----------------------------------------------------
    // 1. Topology & Explicit Graph Verification (Rule 3.1)
    // ----------------------------------------------------
    {
        FanoronaGame game(false, 2);
        // Center node (row 2, col 4 -> node 22) must have 8 connections
        assert(FanoronaGame::isStrongNode(2, 4) && "Center (2,4) must be a strong node");
        assert(game.getEdges(22).size() == 8 && "Center must have 8 explicit connections (4 orthogonal, 4 diagonal)");

        // Corner (0,0) is strong (0+0=0 is even), has 3 neighbors: (0,1), (1,0), (1,1)
        assert(FanoronaGame::isStrongNode(0, 0) && "Corner (0,0) must be a strong node");
        assert(game.getEdges(0).size() == 3 && "Corner (0,0) must have 3 connections (2 ortho, 1 diag)");
        assert(game.isConnected(0, 1) && "Corner (0,0) connected to (0,1)");
        assert(game.isConnected(0, 9) && "Corner (0,0) connected to (1,0)");
        assert(game.isConnected(0, 10) && "Corner (0,0) connected diagonally to (1,1)");

        // Weak node (0,1): 0+1=1 is odd. Must only have orthogonal connections!
        assert(!FanoronaGame::isStrongNode(0, 1) && "Node (0,1) must be a weak node");
        assert(game.getEdges(1).size() == 3 && "Node (0,1) must have exactly 3 orthogonal connections");
        assert(game.isConnected(1, 0) && "Connected to (0,0)");
        assert(game.isConnected(1, 2) && "Connected to (0,2)");
        assert(game.isConnected(1, 10) && "Connected to (1,1)");
        assert(!game.isConnected(1, 9) && "Node (0,1) CANNOT connect diagonally to (1,0)");
        assert(!game.isConnected(1, 11) && "Node (0,1) CANNOT connect diagonally to (1,2)");

        std::cout << "  [PASS] 3.1 Board topology and explicit graph verified." << std::endl;
    }

    // ----------------------------------------------------
    // 2. Initial Setup Verification (Rule 3.2)
    // ----------------------------------------------------
    {
        FanoronaGame game(false, 1);
        const auto& board = game.getBoard();

        int p1Count = 0;
        int p2Count = 0;
        for (int p : board) {
            if (p == 1) p1Count++;
            else if (p == 2) p2Count++;
        }
        assert(p1Count == 22 && "Player 1 must have exactly 22 dark stones");
        assert(p2Count == 22 && "Player 2 must have exactly 22 light stones");
        assert(board[22] == 0 && "Center intersection (node 22) must be the single empty position");

        // Verify middle row pattern (indices 18..26)
        int expectedMid[9] = {2, 1, 2, 1, 0, 2, 1, 2, 1};
        for (int c = 0; c < 9; ++c) {
            assert(board[18 + c] == expectedMid[c] && "Row 2 pattern must match W, B, W, B, ., W, B, W, B");
        }

        // Verify starting player is initialized to 1 (pierres foncées / Host starts)
        assert(game.getTurn() == 1 && "Starting player must be Player 1 (pierres foncées / Host)");

        std::cout << "  [PASS] 3.2 Stones and deterministic initial position verified." << std::endl;
    }

    // ----------------------------------------------------
    // 3. Ordinary Moves Verification (Rule 3.3)
    // ----------------------------------------------------
    {
        FanoronaGame game(false, 1);
        // Setup a clear board scenario where no captures exist
        for (int i = 0; i < 45; ++i) game.setPieceAt(i, 0);
        game.setPieceAt(20, 1); // Row 2, Col 2 (Player 1)
        game.setTurn(1);

        // Ordinary move from 20 to 21 (empty neighbor)
        assert(game.isConnected(20, 21));
        bool ok = game.executeMove(1, 20, 21);
        assert(ok && "Ordinary move to adjacent empty node must succeed");
        assert(game.getPieceAt(20) == 0 && "Origin should now be empty");
        assert(game.getPieceAt(21) == 1 && "Destination should now contain stone");
        assert(game.getTurn() == 2 && "Turn must advance to Player 2 after ordinary move");

        // Try invalid ordinary moves:
        // Move to occupied node
        game.setPieceAt(22, 2);
        game.setTurn(2);
        assert(!game.executeMove(2, 22, 21) && "Cannot move to occupied intersection");

        // Move across non-existent connection (e.g. weak node (0,1) diagonally to (1,2))
        game.setPieceAt(1, 2);
        assert(!game.executeMove(2, 1, 11) && "Cannot move along non-existent diagonal");

        std::cout << "  [PASS] 3.3 Ordinary moves rules verified." << std::endl;
    }

    // ----------------------------------------------------
    // 4. Capture by Percussion (Approach) (Rule 3.4)
    // ----------------------------------------------------
    {
        FanoronaGame game(false, 1);
        for (int i = 0; i < 45; ++i) game.setPieceAt(i, 0);

        // Put Player 1 stone at (2, 2) = node 20
        // Move towards (2, 3) = node 21
        // Put Player 2 stones at (2, 4) = node 22 and (2, 5) = node 23
        game.setPieceAt(20, 1);
        game.setPieceAt(22, 2);
        game.setPieceAt(23, 2);
        game.setTurn(1);

        auto appCaptures = game.getApproachCaptures(20, 21);
        assert(appCaptures.size() == 2 && "Should capture both consecutive enemy stones at 22 and 23");
        assert(appCaptures[0] == 22 && appCaptures[1] == 23);

        auto witCaptures = game.getWithdrawalCaptures(20, 21);
        assert(witCaptures.empty() && "No withdrawal captures in this direction");

        bool ok = game.executeMove(1, 20, 21, "approach");
        assert(ok && "Percussion move must succeed");
        assert(game.getPieceAt(20) == 0);
        assert(game.getPieceAt(21) == 1);
        assert(game.getPieceAt(22) == 0 && "Captured stone at 22 must be removed");
        assert(game.getPieceAt(23) == 0 && "Captured stone at 23 must be removed");

        std::cout << "  [PASS] 3.4 Capture par percussion (approach) verified." << std::endl;
    }

    // ----------------------------------------------------
    // 5. Capture by Aspiration (Withdrawal) (Rule 3.5)
    // ----------------------------------------------------
    {
        FanoronaGame game(false, 1);
        for (int i = 0; i < 45; ++i) game.setPieceAt(i, 0);

        // Put Player 1 stone at (2, 2) = node 20
        // Put enemy stones behind at (2, 1) = node 19 and (2, 0) = node 18
        // Move Player 1 forward to (2, 3) = node 21
        game.setPieceAt(20, 1);
        game.setPieceAt(19, 2);
        game.setPieceAt(18, 2);
        game.setTurn(1);

        auto witCaptures = game.getWithdrawalCaptures(20, 21);
        assert(witCaptures.size() == 2 && "Should capture both consecutive enemy stones behind at 19 and 18");
        assert(witCaptures[0] == 19 && witCaptures[1] == 18);

        auto appCaptures = game.getApproachCaptures(20, 21);
        assert(appCaptures.empty() && "No approach captures");

        bool ok = game.executeMove(1, 20, 21, "withdrawal");
        assert(ok && "Aspiration move must succeed");
        assert(game.getPieceAt(19) == 0 && "Captured stone at 19 removed");
        assert(game.getPieceAt(18) == 0 && "Captured stone at 18 removed");

        std::cout << "  [PASS] 3.5 Capture par aspiration (withdrawal) verified." << std::endl;
    }

    // ----------------------------------------------------
    // 6. Capture Priority & Choice (Rule 3.6)
    // ----------------------------------------------------
    {
        FanoronaGame game(false, 1);
        for (int i = 0; i < 45; ++i) game.setPieceAt(i, 0);

        // Stone A has a capture, Stone B has no capture
        game.setPieceAt(20, 1); // can capture at 22
        game.setPieceAt(22, 2);

        game.setPieceAt(0, 1); // at corner, ordinary move to 1
        game.setTurn(1);

        assert(game.hasAnyLegalCaptures(1) && "Player 1 has available captures");
        // Ordinary move with stone 0 must be rejected!
        bool ordOk = game.executeMove(1, 0, 1);
        assert(!ordOk && "Ordinary move MUST be rejected when capture is available");

        // Dual choice test: both approach and withdrawal available for the same move
        // Stone at 20 moves to 21:
        // Enemy at 22 (ahead) -> approach
        // Enemy at 19 (behind) -> withdrawal
        game.setPieceAt(19, 2);
        auto app = game.getApproachCaptures(20, 21);
        auto wit = game.getWithdrawalCaptures(20, 21);
        assert(!app.empty() && !wit.empty() && "Both capture modes available for this move");

        // Choose withdrawal explicitly
        bool witOk = game.executeMove(1, 20, 21, "withdrawal");
        assert(witOk && "Explicit withdrawal selection must execute");
        assert(game.getPieceAt(19) == 0 && "Withdrawal stone removed");
        assert(game.getPieceAt(22) == 2 && "Approach stone remains intact when withdrawal chosen");

        std::cout << "  [PASS] 3.6 Priorité des captures and dual choice verified." << std::endl;
    }

    // ----------------------------------------------------
    // 7. Successive Captures (Rule 3.7)
    // ----------------------------------------------------
    {
        FanoronaGame game(false, 1);
        for (int i = 0; i < 45; ++i) game.setPieceAt(i, 0);

        // Setup a 2-step capture sequence for Player 1:
        // Start: Stone 1 at (2, 2) = node 20
        // Step 1: Move right to (2, 3) = node 21, percussing enemy at (2, 4) = node 22
        // Then from (2, 3) = node 21, can move down to (3, 3) = node 30 (direction down, different from right!),
        // percussing enemy at (4, 3) = node 39!
        game.setPieceAt(20, 1);
        game.setPieceAt(22, 2); // target of step 1
        game.setPieceAt(39, 2); // target of step 2 (ahead of (3,3) in down direction)
        game.setTurn(1);

        // Execute step 1
        bool step1 = game.executeMove(1, 20, 21, "approach");
        assert(step1 && "Step 1 must succeed");
        assert(game.isInMultiCapture() && "Must enter multi-capture state");
        assert(game.getActivePiece() == 21 && "Active piece must be at 21");
        assert(game.getTurn() == 1 && "Turn must NOT change during multi-capture");

        // Try to move a different piece or revisit 20
        assert(!game.canMakeMove(1, 21, 20) && "Cannot revisit node 20 in the same sequence");

        // Execute step 2 with the same active piece in a different direction
        bool step2 = game.executeMove(1, 21, 30, "approach");
        assert(step2 && "Step 2 must succeed");
        assert(game.getPieceAt(39) == 0 && "Enemy at 39 captured");
        assert(!game.isInMultiCapture() && "Multi-capture must terminate when no more captures exist");
        assert(game.getTurn() == 2 && "Turn must now pass to Player 2");

        std::cout << "  [PASS] 3.7 Captures successives strict chaining verified." << std::endl;
    }

    // ----------------------------------------------------
    // 8. Victory Conditions (Rule 3.9)
    // ----------------------------------------------------
    {
        // 8a. Victory by capturing all enemy stones
        FanoronaGame game(false, 1);
        for (int i = 0; i < 45; ++i) game.setPieceAt(i, 0);
        game.setPieceAt(20, 1);
        game.setPieceAt(22, 2); // only 1 enemy stone
        game.setTurn(1);

        game.executeMove(1, 20, 21, "approach");
        assert(game.isFinished() && "Game must finish when last enemy stone captured");
        assert(game.getWinner() == 1 && "Player 1 must be winner");
        assert(!game.getWinReason().empty() && "Win reason must be clearly stated");

        // 8b. Victory by immobilization
        FanoronaGame game2(false, 1);
        for (int i = 0; i < 45; ++i) game2.setPieceAt(i, 0);
        // Put player 2 at corner (0,0) surrounded by player 1 stones at (0,1), (1,0), (1,1)
        game2.setPieceAt(0, 2);
        game2.setPieceAt(1, 1);
        game2.setPieceAt(9, 1);
        game2.setPieceAt(10, 1);
        // Put another player 1 stone elsewhere
        game2.setPieceAt(44, 1);
        game2.setTurn(2);

        int winner = 0;
        std::string reason;
        bool imm = game2.checkImmobilization(2, winner, reason);
        assert(imm && "Player 2 must be detected as immobilized");
        assert(winner == 1 && "Player 1 must win upon Player 2 immobilization");

        std::cout << "  [PASS] 3.9 Conditions de victoire (elimination & immobilization) verified." << std::endl;
    }

    // ----------------------------------------------------
    // 9. Solo Mode vs Bot
    // ----------------------------------------------------
    {
        // Solo mode: Player 1 (human), Player 2 (Bot starts if startingPlayer = 2)
        FanoronaGame soloGame(true, 2);
        // On init(), bot (Player 2) has taken opening move and turn passed to Player 1
        assert(soloGame.getTurn() == 1 && "In solo mode with Player 2 starting, Bot moves and yields turn to Player 1");
        assert(soloGame.getPieceAt(22) != 0 && "Center position should now be occupied by the opening move");

        // Human responds with a legal move
        assert(soloGame.hasAnyLegalMoves(1) && "Human has legal moves");

        // Solo mode: Player 1 (Human) starts by default
        FanoronaGame humanSoloGame(true, 1);
        assert(humanSoloGame.getTurn() == 1 && "In solo mode with Player 1 starting, Human starts");
        assert(humanSoloGame.getPieceAt(22) == 0 && "Center node 22 is initially empty for Human opening");
        assert(humanSoloGame.hasAnyLegalCaptures(1) && "Human has legal opening approach captures");

        std::cout << "  [PASS] Solo mode bot execution verified." << std::endl;
    }

    // ----------------------------------------------------
    // 10. Move Metadata & Bot Step Execution for Animation
    // ----------------------------------------------------
    {
        FanoronaGame game(false, 1);
        for (int i = 0; i < 45; ++i) game.setPieceAt(i, 0);
        game.setPieceAt(20, 1);
        game.setPieceAt(22, 2);
        game.setTurn(1);

        bool ok = game.executeMove(1, 20, 21, "approach");
        assert(ok);
        const auto& lm = game.getLastMove();
        assert(lm.id > 0 && "Move counter must increment");
        assert(lm.player == 1 && "Last move player must be 1");
        assert(lm.from == 20 && lm.to == 21 && "Last move from/to must match");
        assert(lm.captureType == "approach" && "Capture type must match");
        assert(lm.captured.size() == 1 && lm.captured[0] == 22 && "Captured list must match");

        // Verify serialized JSON contains last_move structure
        std::string json = game.serializeState();
        assert(json.find("\"last_move\":") != std::string::npos && "State must serialize last_move");
        assert(json.find("\"from\":20") != std::string::npos && "State must include last_move from");
        assert(json.find("\"to\":21") != std::string::npos && "State must include last_move to");
        assert(json.find("\"captured\":[22]") != std::string::npos && "State must include last_move captured");

        // Test bot step execution
        FanoronaGame botGame(true, 1);
        for (int i = 0; i < 45; ++i) botGame.setPieceAt(i, 0);
        botGame.setPieceAt(20, 2);
        botGame.setTurn(2);
        bool stepOk = botGame.executeBotStep();
        // Test voluntary end_turn during multi-capture
        FanoronaGame multiGame(false, 1);
        for (int i = 0; i < 45; ++i) multiGame.setPieceAt(i, 0);
        multiGame.setPieceAt(20, 1);
        multiGame.setPieceAt(22, 2); // target of step 1
        multiGame.setPieceAt(39, 2); // target of step 2
        multiGame.setPieceAt(44, 2); // remaining stone so game doesn't end on step 1
        multiGame.setTurn(1);

        bool step1 = multiGame.executeMove(1, 20, 21, "approach");
        assert(step1 && "Step 1 must succeed");
        assert(multiGame.isInMultiCapture() && "Game should be in multi-capture state");

        multiGame.handleInput(1, "{\"cmd\":\"end_turn\"}");
        assert(!multiGame.isInMultiCapture() && "end_turn must end multi-capture state");
        assert(multiGame.getTurn() == 2 && "Turn must advance to player 2 after end_turn");

        std::cout << "  [PASS] Move metadata, last_move JSON serialization, executeBotStep, and voluntary end_turn verified." << std::endl;
    }

    std::cout << "[TEST] All Fanorona Malagasy Unit Tests Passed Successfully!" << std::endl;
    return 0;
}
