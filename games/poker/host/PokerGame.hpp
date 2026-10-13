#pragma once

#include <gamehub/core/Game.hpp>
#include <array>
#include <string>
#include <cstdint>

namespace gamehub {
namespace core {

struct Card {
    int rank{0}; // 2..14 (14 = Ace)
    int suit{0}; // 0..3 (0: c, 1: d, 2: h, 3: s)

    bool isValid() const { return rank >= 2 && rank <= 14 && suit >= 0 && suit <= 3; }
    std::string toString() const;
    std::string suitSymbol() const;
};

enum class PokerCategory {
    HIGH_CARD = 0,
    ONE_PAIR = 1,
    TWO_PAIR = 2,
    THREE_OF_A_KIND = 3,
    STRAIGHT = 4,
    FLUSH = 5,
    FULL_HOUSE = 6,
    FOUR_OF_A_KIND = 7,
    STRAIGHT_FLUSH = 8,
    ROYAL_FLUSH = 9
};

struct HandScore {
    int category{0}; // PokerCategory 0..9
    int tieBreakers[5]{0, 0, 0, 0, 0};

    bool operator<(const HandScore& o) const {
        if (category != o.category) return category < o.category;
        for (int i = 0; i < 5; ++i) {
            if (tieBreakers[i] != o.tieBreakers[i]) return tieBreakers[i] < o.tieBreakers[i];
        }
        return false;
    }
    bool operator>(const HandScore& o) const { return o < *this; }
    bool operator==(const HandScore& o) const {
        if (category != o.category) return false;
        for (int i = 0; i < 5; ++i) {
            if (tieBreakers[i] != o.tieBreakers[i]) return false;
        }
        return true;
    }
    bool operator!=(const HandScore& o) const { return !(*this == o); }
};

enum class PokerStreet {
    STARTING_HAND = 0,
    PRE_FLOP = 1,
    FLOP = 2,
    TURN = 3,
    RIVER = 4,
    SHOWDOWN = 5,
    HAND_OVER = 6
};

struct PokerPlayer {
    int id{1};
    std::string name{"Player"};
    int chips{1000};
    int currentBet{0};
    int totalHandBet{0};
    bool folded{false};
    bool allIn{false};
    bool isBot{false};
    bool hasActed{false};
    std::array<Card, 2> holeCards{};
    HandScore bestScore{};
    std::string handDescription{};
};

struct PotWinner {
    int playerId{0};
    int payout{0};
    std::string handDescription{};
};

class PokerGame : public Game {
public:
    explicit PokerGame(bool isSolo = false, int numPlayers = 4, int startingChips = 1000);
    ~PokerGame() override = default;

    void init() override;
    void update(float dt) override;
    void handleInput(PlayerId player, const std::string& input) override;
    std::string serializeState() const override;
    std::string serializeStateForPlayer(PlayerId id) const override;
    bool isFinished() const override { return m_finished; }
    int getWinner() const override { return m_winner; }
    void setPlayerCount(int count) override;
    bool isRealTime() const override { return true; }

    // Evaluator methods (zero-heap static functions)
    static HandScore evaluate5Cards(const std::array<Card, 5>& cards);
    static HandScore evaluate7Cards(const std::array<Card, 7>& cards);
    static std::string describeHand(const HandScore& score);

    // Testing helpers & deterministic state setters
    void setBotDelay(float d) { m_botDelay = d; }
    void setDeckForTesting(const std::array<Card, 52>& deck, int deckTop = 0);
    void setHoleCardsForTesting(int seatIdx, Card c1, Card c2);
    void setCommunityCardsForTesting(const std::array<Card, 5>& cards, int count);
    void setStreetForTesting(PokerStreet street) { m_street = street; }
    void setTurnTimerForTesting(float t) { m_turnTimer = t; }
    void setDealerSeatForTesting(int seat) { m_dealerSeat = seat; }
    void setCurrentTurnForTesting(int seatId) { m_currentTurn = seatId; }
    void setPlayerChipsForTesting(int seatIdx, int chips) { if (seatIdx >= 0 && seatIdx < 4) m_players[seatIdx].chips = chips; }

    int getCurrentTurn() const { return m_currentTurn; }
    PokerStreet getStreet() const { return m_street; }
    int getPot() const { return m_pot; }
    int getCommunityCount() const { return m_communityCount; }
    const std::array<Card, 5>& getCommunityCards() const { return m_communityCards; }
    const PokerPlayer& getPlayer(int seatIdx) const { return m_players[seatIdx]; }
    int getDealerSeat() const { return m_dealerSeat; }
    float getTurnTimer() const { return m_turnTimer; }
    int getNumPlayers() const { return m_numPlayers; }

    void startNewHand();

private:
    void shuffleDeck();
    void dealHoleCards();
    void postBlinds();
    void advanceStreet();
    void executeShowdown();
    void awardPotToRemainingPlayer(int winnerSeatIdx);
    void nextTurn();
    void executePlayerAction(int seatIdx, const std::string& action, int amount);
    void executeBotMove(int seatIdx);
    bool checkBettingRoundComplete() const;
    void collectBetsIntoPot();
    int getNextActiveSeat(int fromSeatIdx) const;
    int countActiveNonFolded() const;
    int countNonFoldedNonAllIn() const;
    void recordWinner(int playerId, int payout, const std::string& desc);

    bool m_isSolo{false};
    int m_numPlayers{4};
    int m_startingChips{1000};
    int m_smallBlind{10};
    int m_bigBlind{20};
    int m_currentHighestBet{20};
    int m_minRaise{20};

    PokerStreet m_street{PokerStreet::PRE_FLOP};
    int m_dealerSeat{0};
    int m_currentTurn{1}; // 1-based seat ID (1..4)
    float m_turnTimer{20.0f};
    float m_botTimer{0.5f};
    float m_botDelay{0.5f};
    float m_handOverTimer{0.0f};

    int m_pot{0};
    bool m_finished{false};
    int m_winner{0};
    int m_handNumber{0};

    std::array<Card, 52> m_deck{};
    int m_deckTop{0};

    std::array<Card, 5> m_communityCards{};
    int m_communityCount{0};

    std::array<PokerPlayer, 4> m_players{};

    std::array<PotWinner, 8> m_lastWinners{};
    int m_numLastWinners{0};
    std::string m_lastAction{"Hand dealt"};
};

} // namespace core
} // namespace gamehub
