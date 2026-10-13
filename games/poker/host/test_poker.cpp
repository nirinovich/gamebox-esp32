#include <iostream>
#undef NDEBUG
#include <cassert>
#include <string>
#include <array>
#include "PokerGame.hpp"

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

    std::cout << "[TEST] Running Texas Hold'em Poker Authoritative Engine Tests..." << std::endl;

    using namespace gamehub::core;

    // 1. 5-Card Hand Evaluator Verification
    {
        // Royal Flush
        std::array<Card, 5> royal = {Card{14, 3}, Card{13, 3}, Card{12, 3}, Card{11, 3}, Card{10, 3}};
        HandScore sRoyal = PokerGame::evaluate5Cards(royal);
        assert(sRoyal.category == static_cast<int>(PokerCategory::ROYAL_FLUSH));
        assert(PokerGame::describeHand(sRoyal) == "Royal Flush");

        // Straight Flush
        std::array<Card, 5> strFlush = {Card{9, 2}, Card{8, 2}, Card{7, 2}, Card{6, 2}, Card{5, 2}};
        HandScore sStrFlush = PokerGame::evaluate5Cards(strFlush);
        assert(sStrFlush.category == static_cast<int>(PokerCategory::STRAIGHT_FLUSH));
        assert(sStrFlush.tieBreakers[0] == 9);

        // Steel Wheel (A-2-3-4-5 Straight Flush)
        std::array<Card, 5> wheelFlush = {Card{14, 0}, Card{2, 0}, Card{3, 0}, Card{4, 0}, Card{5, 0}};
        HandScore sWheelFlush = PokerGame::evaluate5Cards(wheelFlush);
        assert(sWheelFlush.category == static_cast<int>(PokerCategory::STRAIGHT_FLUSH));
        assert(sWheelFlush.tieBreakers[0] == 5);

        // Four of a Kind
        std::array<Card, 5> quads = {Card{10, 0}, Card{10, 1}, Card{10, 2}, Card{10, 3}, Card{14, 1}};
        HandScore sQuads = PokerGame::evaluate5Cards(quads);
        assert(sQuads.category == static_cast<int>(PokerCategory::FOUR_OF_A_KIND));
        assert(sQuads.tieBreakers[0] == 10);
        assert(sQuads.tieBreakers[1] == 14);

        // Full House
        std::array<Card, 5> boat = {Card{14, 0}, Card{14, 1}, Card{14, 2}, Card{13, 0}, Card{13, 1}};
        HandScore sBoat = PokerGame::evaluate5Cards(boat);
        assert(sBoat.category == static_cast<int>(PokerCategory::FULL_HOUSE));
        assert(sBoat.tieBreakers[0] == 14);
        assert(sBoat.tieBreakers[1] == 13);

        // Flush
        std::array<Card, 5> flush = {Card{14, 3}, Card{10, 3}, Card{7, 3}, Card{6, 3}, Card{2, 3}};
        HandScore sFlush = PokerGame::evaluate5Cards(flush);
        assert(sFlush.category == static_cast<int>(PokerCategory::FLUSH));
        assert(sFlush.tieBreakers[0] == 14);

        // Straight (Normal)
        std::array<Card, 5> straight = {Card{10, 0}, Card{9, 1}, Card{8, 2}, Card{7, 3}, Card{6, 0}};
        HandScore sStraight = PokerGame::evaluate5Cards(straight);
        assert(sStraight.category == static_cast<int>(PokerCategory::STRAIGHT));
        assert(sStraight.tieBreakers[0] == 10);

        // Straight (Wheel A-2-3-4-5)
        std::array<Card, 5> wheel = {Card{14, 0}, Card{5, 1}, Card{4, 2}, Card{3, 3}, Card{2, 0}};
        HandScore sWheel = PokerGame::evaluate5Cards(wheel);
        assert(sWheel.category == static_cast<int>(PokerCategory::STRAIGHT));
        assert(sWheel.tieBreakers[0] == 5);

        // Three of a Kind
        std::array<Card, 5> trips = {Card{7, 0}, Card{7, 1}, Card{7, 2}, Card{14, 3}, Card{2, 0}};
        HandScore sTrips = PokerGame::evaluate5Cards(trips);
        assert(sTrips.category == static_cast<int>(PokerCategory::THREE_OF_A_KIND));
        assert(sTrips.tieBreakers[0] == 7);
        assert(sTrips.tieBreakers[1] == 14);

        // Two Pair
        std::array<Card, 5> twoPair = {Card{14, 0}, Card{14, 1}, Card{10, 2}, Card{10, 3}, Card{5, 0}};
        HandScore sTwoPair = PokerGame::evaluate5Cards(twoPair);
        assert(sTwoPair.category == static_cast<int>(PokerCategory::TWO_PAIR));
        assert(sTwoPair.tieBreakers[0] == 14);
        assert(sTwoPair.tieBreakers[1] == 10);
        assert(sTwoPair.tieBreakers[2] == 5);

        // One Pair
        std::array<Card, 5> pair = {Card{14, 0}, Card{14, 1}, Card{9, 2}, Card{5, 3}, Card{2, 0}};
        HandScore sPair = PokerGame::evaluate5Cards(pair);
        assert(sPair.category == static_cast<int>(PokerCategory::ONE_PAIR));
        assert(sPair.tieBreakers[0] == 14);

        // High Card
        std::array<Card, 5> highCard = {Card{14, 0}, Card{12, 1}, Card{9, 2}, Card{5, 3}, Card{2, 0}};
        HandScore sHighCard = PokerGame::evaluate5Cards(highCard);
        assert(sHighCard.category == static_cast<int>(PokerCategory::HIGH_CARD));

        // Comparison Checks
        assert(sRoyal > sStrFlush);
        assert(sStrFlush > sQuads);
        assert(sQuads > sBoat);
        assert(sBoat > sFlush);
        assert(sFlush > sStraight);
        assert(sStraight > sTrips);
        assert(sTrips > sTwoPair);
        assert(sTwoPair > sPair);
        assert(sPair > sHighCard);

        // Kicker tie breaker: Pair of Aces with King kicker beats Pair of Aces with Queen kicker
        std::array<Card, 5> pairK = {Card{14, 0}, Card{14, 1}, Card{13, 2}, Card{5, 3}, Card{2, 0}};
        std::array<Card, 5> pairQ = {Card{14, 2}, Card{14, 3}, Card{12, 0}, Card{5, 1}, Card{2, 1}};
        assert(PokerGame::evaluate5Cards(pairK) > PokerGame::evaluate5Cards(pairQ));

        std::cout << "  [PASS] 5-Card Hand Evaluator (High Card to Royal Flush + Kickers) verified." << std::endl;
    }

    // 2. 7-Card Evaluator Combination Selection
    {
        // 7 cards: 2 hole cards + 5 board cards
        // Board: [10s, Js, Qs, 2h, 3c]
        // Hand A: [Ks, As] -> Royal Flush!
        // Hand B: [9s, 8s] -> Straight Flush 9-High!
        std::array<Card, 7> handA = {
            Card{13, 3}, Card{14, 3}, // Ks, As
            Card{10, 3}, Card{11, 3}, Card{12, 3}, Card{2, 2}, Card{3, 0}
        };
        HandScore scoreA = PokerGame::evaluate7Cards(handA);
        assert(scoreA.category == static_cast<int>(PokerCategory::ROYAL_FLUSH));

        std::array<Card, 7> handB = {
            Card{9, 3}, Card{8, 3}, // 9s, 8s
            Card{10, 3}, Card{11, 3}, Card{12, 3}, Card{2, 2}, Card{3, 0}
        };
        HandScore scoreB = PokerGame::evaluate7Cards(handB);
        assert(scoreB.category == static_cast<int>(PokerCategory::STRAIGHT_FLUSH));
        assert(scoreB.tieBreakers[0] == 12); // Q-high straight flush

        assert(scoreA > scoreB);
        std::cout << "  [PASS] 7-Card Evaluator combo selection verified." << std::endl;
    }

    // 3. Game Dealing & Strict Card Masking
    {
        PokerGame game(false, 2, 1000);
        game.init();

        assert(game.getStreet() == PokerStreet::PRE_FLOP);
        // Verify state for Player 1: P1 cards exposed, P2 cards masked
        std::string p1State = game.serializeStateForPlayer(1);
        assert(p1State.find("\"game\":\"poker\"") != std::string::npos);
        assert(p1State.find("\"hidden\":true") != std::string::npos); // Opponent cards masked
        assert(p1State.find("\"pot\":") != std::string::npos);

        // Verify Player 2 state: P2 cards exposed, P1 masked
        std::string p2State = game.serializeStateForPlayer(2);
        assert(p2State.find("\"hidden\":true") != std::string::npos);

        std::cout << "  [PASS] Dealing & Authoritative Card Masking verified." << std::endl;
    }

    // 4. Multi-Street Betting Loop: Pre-Flop -> Flop -> Turn -> River -> Showdown
    {
        PokerGame game(false, 2, 1000);
        game.init();

        // Deterministic deck for repeatable test
        std::array<Card, 52> testDeck{};
        for (int i = 0; i < 52; ++i) testDeck[i] = Card{(i % 13) + 2, i / 13};
        game.setDeckForTesting(testDeck, 0);

        // Pre-flop: Small blind acts first in heads-up
        int turn = game.getCurrentTurn();
        game.handleInput(turn, "{\"cmd\":\"call\"}");
        // Big blind checks
        turn = game.getCurrentTurn();
        game.handleInput(turn, "{\"cmd\":\"check\"}");

        // Now we should be on the FLOP
        assert(game.getStreet() == PokerStreet::FLOP);
        assert(game.getCommunityCount() == 3);

        // Flop betting: Check, Check
        turn = game.getCurrentTurn();
        game.handleInput(turn, "{\"cmd\":\"check\"}");
        turn = game.getCurrentTurn();
        game.handleInput(turn, "{\"cmd\":\"check\"}");

        // Now on the TURN
        assert(game.getStreet() == PokerStreet::TURN);
        assert(game.getCommunityCount() == 4);

        // Turn betting: Check, Check
        turn = game.getCurrentTurn();
        game.handleInput(turn, "{\"cmd\":\"check\"}");
        turn = game.getCurrentTurn();
        game.handleInput(turn, "{\"cmd\":\"check\"}");

        // Now on the RIVER
        assert(game.getStreet() == PokerStreet::RIVER);
        assert(game.getCommunityCount() == 5);

        // River betting: Check, Check
        turn = game.getCurrentTurn();
        game.handleInput(turn, "{\"cmd\":\"check\"}");
        turn = game.getCurrentTurn();
        game.handleInput(turn, "{\"cmd\":\"check\"}");

        // Now at SHOWDOWN / HAND_OVER
        assert(game.getStreet() == PokerStreet::HAND_OVER);
        std::string showdownState = game.serializeStateForPlayer(1);
        // At showdown, opponent's cards are revealed!
        assert(showdownState.find("\"winners\":[") != std::string::npos);

        std::cout << "  [PASS] Multi-street betting loop (Pre-flop -> Flop -> Turn -> River -> Showdown) verified." << std::endl;
    }

    // 5. 20-Second Server Action Timeout (Auto-Check / Auto-Fold)
    {
        PokerGame game(false, 2, 1000);
        game.init();

        // Facing BB of 20, SB has put in 10. Current turn is SB (P1).
        // If timer expires, SB is facing a bet of 20 (they only put in 10), so they should AUTO-FOLD!
        game.setTurnTimerForTesting(0.01f);
        game.update(0.05f); // Timer expires

        assert(game.getStreet() == PokerStreet::HAND_OVER);
        // Player 1 folded due to timeout, Player 2 wins pot
        const auto& p1 = game.getPlayer(0);
        assert(p1.folded == true);
        const auto& p2 = game.getPlayer(1);
        assert(p2.chips > 1000);

        std::cout << "  [PASS] 20-second timeout auto-fold verified." << std::endl;
    }

    // 6. Side-Pot Distribution with Multiple All-Ins
    {
        PokerGame game(false, 3, 1000);
        game.init();
        game.setBotDelay(999.0f); // Disable bot automatic actions during manual test

        // Setup stacks: P1 has 100, P2 has 300, P3 has 1000
        game.setPlayerChipsForTesting(0, 100);
        game.setPlayerChipsForTesting(1, 300);
        game.setPlayerChipsForTesting(2, 1000);

        // Set hole cards
        // P1: Pair of Aces (strongest)
        game.setHoleCardsForTesting(0, Card{14, 0}, Card{14, 1});
        // P2: Pair of Kings (second strongest)
        game.setHoleCardsForTesting(1, Card{13, 0}, Card{13, 1});
        // P3: High Card Queen (weakest)
        game.setHoleCardsForTesting(2, Card{12, 0}, Card{2, 1});

        // Set deterministic remaining deck for flop, turn, river: 2, 4, 6, 8, 10 (all even, rainbow)
        std::array<Card, 52> detDeck{};
        for (int i = 0; i < 52; ++i) detDeck[i] = Card{2 + (i % 8), i / 8};
        detDeck[6] = Card{3, 0}; // burn 1
        detDeck[7] = Card{2, 0}; // 2c
        detDeck[8] = Card{4, 1}; // 4d
        detDeck[9] = Card{6, 2}; // 6h
        detDeck[10] = Card{8, 3}; // 8s
        detDeck[11] = Card{10, 0}; // 10c
        game.setDeckForTesting(detDeck, 6);

        // Pre-flop action sequence:
        game.handleInput(game.getCurrentTurn(), "{\"cmd\":\"all_in\"}");
        game.handleInput(game.getCurrentTurn(), "{\"cmd\":\"all_in\"}");
        game.handleInput(game.getCurrentTurn(), "{\"cmd\":\"call\"}");

        // After all-ins, game runs to showdown
        assert(game.getStreet() == PokerStreet::HAND_OVER);

        // Main pot: 100 * 3 = 300 chips. Won by P1 (Aces).
        // Side pot: (310 - 100) * 2 = 420 chips. Won by P2 (Kings).
        // P1 ends with 300 chips.
        // P2 ends with 420 chips.
        // P3 ends with 690 chips.
        assert(game.getPlayer(0).chips == 300);
        assert(game.getPlayer(1).chips == 420);
        assert(game.getPlayer(2).chips == 710);

        std::cout << "  [PASS] Side-pot distribution with all-in tiers verified." << std::endl;
    }

    // 7. Fold Victory without Showdown
    {
        PokerGame game(false, 2, 1000);
        game.init();

        int turn = game.getCurrentTurn();
        game.handleInput(turn, "{\"cmd\":\"fold\"}");

        assert(game.getStreet() == PokerStreet::HAND_OVER);
        std::string state = game.serializeStateForPlayer(1);
        assert(state.find("folded") != std::string::npos);

        std::cout << "  [PASS] Fold victory without showdown verified." << std::endl;
    }

    // 8. Solo Mode vs Bots
    {
        PokerGame soloGame(true, 4, 1000);
        soloGame.init();
        soloGame.setBotDelay(0.01f); // Quick bot delay for test

        assert(soloGame.getPlayer(0).isBot == false);
        assert(soloGame.getPlayer(1).isBot == true);
        assert(soloGame.getPlayer(2).isBot == true);
        assert(soloGame.getPlayer(3).isBot == true);

        // Advance a few ticks; bots should execute their moves without hanging
        for (int i = 0; i < 50; ++i) {
            soloGame.update(0.05f);
        }

        std::cout << "  [PASS] Solo mode vs 3 heuristic bots verified." << std::endl;
    }

    std::cout << "[SUCCESS] All Texas Hold'em Poker Host Engine tests passed!" << std::endl;
    return 0;
}
