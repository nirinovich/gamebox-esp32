#include "PokerGame.hpp"
#include <gamehub/core/GameRegistry.hpp>
#include <algorithm>
#include <random>
#include <sstream>

namespace {
std::unique_ptr<gamehub::core::Game> createPokerSolo() {
    return std::make_unique<gamehub::core::PokerGame>(true, 4);
}
std::unique_ptr<gamehub::core::Game> createPokerTable() {
    return std::make_unique<gamehub::core::PokerGame>(false, 4);
}
}

#if 0 // POKER DISABLED FOR NOW (per user request and ADR-0022)
REGISTER_GAME("poker_solo", PokerSolo, &createPokerSolo, {
    gamehub::core::GameId("poker_solo"),
    "Texas Hold'em Solo",
    "SOLO",
    "No-Limit Texas Hold'em vs 3 Heuristic Bots",
    1, 1,
    gamehub::core::InputFlag::ACTION_BUTTONS,
    "poker_solo",
    false
})

REGISTER_GAME("poker_table", PokerTable, &createPokerTable, {
    gamehub::core::GameId("poker_table"),
    "Texas Hold'em Table",
    "PARTY",
    "2-4P No-Limit Texas Hold'em with Live Showdown & Side-Pots",
    2, 4,
    gamehub::core::InputFlag::ACTION_BUTTONS,
    "poker_table",
    false
})
#endif // POKER DISABLED FOR NOW

namespace gamehub {
namespace core {

std::string Card::toString() const {
    if (!isValid()) return "";
    static const char ranks[] = "??23456789TJQKA";
    static const char suits[] = "cdhs";
    std::string s;
    s += ranks[rank];
    s += suits[suit];
    return s;
}

std::string Card::suitSymbol() const {
    if (!isValid()) return "";
    static const char* symbols[] = {"♣", "♦", "♥", "♠"};
    return symbols[suit];
}

HandScore PokerGame::evaluate5Cards(const std::array<Card, 5>& cards) {
    std::array<Card, 5> s = cards;
    std::sort(s.begin(), s.end(), [](const Card& a, const Card& b) {
        return a.rank > b.rank;
    });

    bool isFlush = (s[0].suit == s[1].suit &&
                    s[1].suit == s[2].suit &&
                    s[2].suit == s[3].suit &&
                    s[3].suit == s[4].suit);

    bool isDistinct = (s[0].rank != s[1].rank &&
                       s[1].rank != s[2].rank &&
                       s[2].rank != s[3].rank &&
                       s[3].rank != s[4].rank);

    bool isNormalStraight = isDistinct && (s[0].rank - s[4].rank == 4);
    bool isWheel = (s[0].rank == 14 && s[1].rank == 5 && s[2].rank == 4 && s[3].rank == 3 && s[4].rank == 2);
    bool isStraight = isNormalStraight || isWheel;
    int straightHigh = isWheel ? 5 : (isNormalStraight ? s[0].rank : 0);

    int counts[15] = {0};
    for (int i = 0; i < 5; ++i) {
        counts[s[i].rank]++;
    }

    int fourRank = 0;
    int threeRank = 0;
    int pair1 = 0;
    int pair2 = 0;

    for (int r = 14; r >= 2; --r) {
        if (counts[r] == 4) {
            fourRank = r;
        } else if (counts[r] == 3) {
            threeRank = r;
        } else if (counts[r] == 2) {
            if (pair1 == 0) pair1 = r;
            else pair2 = r;
        }
    }

    HandScore score{};
    if (isFlush && isStraight) {
        if (straightHigh == 14) {
            score.category = static_cast<int>(PokerCategory::ROYAL_FLUSH);
            score.tieBreakers[0] = 14;
        } else {
            score.category = static_cast<int>(PokerCategory::STRAIGHT_FLUSH);
            score.tieBreakers[0] = straightHigh;
        }
    } else if (fourRank > 0) {
        score.category = static_cast<int>(PokerCategory::FOUR_OF_A_KIND);
        score.tieBreakers[0] = fourRank;
        for (int i = 0; i < 5; ++i) {
            if (s[i].rank != fourRank) {
                score.tieBreakers[1] = s[i].rank;
                break;
            }
        }
    } else if (threeRank > 0 && pair1 > 0) {
        score.category = static_cast<int>(PokerCategory::FULL_HOUSE);
        score.tieBreakers[0] = threeRank;
        score.tieBreakers[1] = pair1;
    } else if (isFlush) {
        score.category = static_cast<int>(PokerCategory::FLUSH);
        for (int i = 0; i < 5; ++i) score.tieBreakers[i] = s[i].rank;
    } else if (isStraight) {
        score.category = static_cast<int>(PokerCategory::STRAIGHT);
        score.tieBreakers[0] = straightHigh;
    } else if (threeRank > 0) {
        score.category = static_cast<int>(PokerCategory::THREE_OF_A_KIND);
        score.tieBreakers[0] = threeRank;
        int idx = 1;
        for (int i = 0; i < 5; ++i) {
            if (s[i].rank != threeRank) score.tieBreakers[idx++] = s[i].rank;
        }
    } else if (pair1 > 0 && pair2 > 0) {
        score.category = static_cast<int>(PokerCategory::TWO_PAIR);
        score.tieBreakers[0] = pair1;
        score.tieBreakers[1] = pair2;
        for (int i = 0; i < 5; ++i) {
            if (s[i].rank != pair1 && s[i].rank != pair2) {
                score.tieBreakers[2] = s[i].rank;
                break;
            }
        }
    } else if (pair1 > 0) {
        score.category = static_cast<int>(PokerCategory::ONE_PAIR);
        score.tieBreakers[0] = pair1;
        int idx = 1;
        for (int i = 0; i < 5; ++i) {
            if (s[i].rank != pair1) score.tieBreakers[idx++] = s[i].rank;
        }
    } else {
        score.category = static_cast<int>(PokerCategory::HIGH_CARD);
        for (int i = 0; i < 5; ++i) score.tieBreakers[i] = s[i].rank;
    }

    return score;
}

HandScore PokerGame::evaluate7Cards(const std::array<Card, 7>& cards) {
    HandScore bestScore{};
    bool first = true;
    for (int a = 0; a < 6; ++a) {
        for (int b = a + 1; b < 7; ++b) {
            std::array<Card, 5> five{};
            int idx = 0;
            for (int k = 0; k < 7; ++k) {
                if (k != a && k != b) {
                    five[idx++] = cards[k];
                }
            }
            HandScore s = evaluate5Cards(five);
            if (first || s > bestScore) {
                bestScore = s;
                first = false;
            }
        }
    }
    return bestScore;
}

std::string PokerGame::describeHand(const HandScore& score) {
    static const char* rNames[] = {
        "", "", "2", "3", "4", "5", "6", "7", "8", "9", "Ten", "Jack", "Queen", "King", "Ace"
    };
    static const char* rPlurals[] = {
        "", "", "Deuces", "3s", "4s", "5s", "6s", "7s", "8s", "9s", "Tens", "Jacks", "Queens", "Kings", "Aces"
    };

    auto safeName = [](int r) -> std::string {
        if (r >= 2 && r <= 14) return rNames[r];
        return "";
    };
    auto safePlural = [](int r) -> std::string {
        if (r >= 2 && r <= 14) return rPlurals[r];
        return "";
    };

    switch (static_cast<PokerCategory>(score.category)) {
        case PokerCategory::ROYAL_FLUSH:
            return "Royal Flush";
        case PokerCategory::STRAIGHT_FLUSH:
            return "Straight Flush (" + safeName(score.tieBreakers[0]) + " High)";
        case PokerCategory::FOUR_OF_A_KIND:
            return "Four of a Kind, " + safePlural(score.tieBreakers[0]);
        case PokerCategory::FULL_HOUSE:
            return "Full House, " + safePlural(score.tieBreakers[0]) + " full of " + safePlural(score.tieBreakers[1]);
        case PokerCategory::FLUSH:
            return "Flush (" + safeName(score.tieBreakers[0]) + " High)";
        case PokerCategory::STRAIGHT:
            return "Straight (" + safeName(score.tieBreakers[0]) + " High)";
        case PokerCategory::THREE_OF_A_KIND:
            return "Three of a Kind, " + safePlural(score.tieBreakers[0]);
        case PokerCategory::TWO_PAIR:
            return "Two Pair, " + safePlural(score.tieBreakers[0]) + " and " + safePlural(score.tieBreakers[1]);
        case PokerCategory::ONE_PAIR:
            return "Pair of " + safePlural(score.tieBreakers[0]);
        case PokerCategory::HIGH_CARD:
        default:
            return "High Card (" + safeName(score.tieBreakers[0]) + ")";
    }
}

PokerGame::PokerGame(bool isSolo, int numPlayers, int startingChips)
    : m_isSolo(isSolo),
      m_numPlayers(std::clamp(numPlayers, 2, 4)),
      m_startingChips(startingChips > 0 ? startingChips : 1000) {
    init();
}

void PokerGame::setPlayerCount(int count) {
    int clamped = std::clamp(count, 2, 4);
    if (clamped == m_numPlayers) return;
    m_numPlayers = clamped;
    init();
}

void PokerGame::init() {
    m_finished = false;
    m_winner = 0;
    m_handNumber = 0;
    m_dealerSeat = m_numPlayers - 1;
    m_pot = 0;
    m_numLastWinners = 0;

    for (int i = 0; i < 4; ++i) {
        m_players[i].id = i + 1;
        m_players[i].chips = (i < m_numPlayers) ? m_startingChips : 0;
        m_players[i].currentBet = 0;
        m_players[i].totalHandBet = 0;
        m_players[i].folded = (i >= m_numPlayers);
        m_players[i].allIn = false;
        m_players[i].hasActed = false;
        m_players[i].handDescription = "";

        if (m_isSolo) {
            m_players[i].isBot = (i > 0);
            m_players[i].name = (i == 0) ? "Player 1" : ("Bot " + std::to_string(i));
        } else {
            m_players[i].isBot = false;
            m_players[i].name = "Player " + std::to_string(i + 1);
        }
    }

    startNewHand();
}

void PokerGame::shuffleDeck() {
    int idx = 0;
    for (int s = 0; s < 4; ++s) {
        for (int r = 2; r <= 14; ++r) {
            m_deck[idx++] = Card{r, s};
        }
    }
    m_deckTop = 0;

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(m_deck.begin(), m_deck.end(), g);
}

void PokerGame::setDeckForTesting(const std::array<Card, 52>& deck, int deckTop) {
    m_deck = deck;
    m_deckTop = deckTop;
}

void PokerGame::setHoleCardsForTesting(int seatIdx, Card c1, Card c2) {
    if (seatIdx >= 0 && seatIdx < 4) {
        m_players[seatIdx].holeCards[0] = c1;
        m_players[seatIdx].holeCards[1] = c2;
    }
}

void PokerGame::setCommunityCardsForTesting(const std::array<Card, 5>& cards, int count) {
    m_communityCards = cards;
    m_communityCount = count;
}

int PokerGame::getNextActiveSeat(int fromSeatIdx) const {
    for (int step = 1; step <= m_numPlayers; ++step) {
        int seat = (fromSeatIdx + step) % m_numPlayers;
        if (!m_players[seat].folded && !m_players[seat].allIn && m_players[seat].chips > 0) {
            return seat;
        }
    }
    return -1;
}

int PokerGame::countActiveNonFolded() const {
    int cnt = 0;
    for (int i = 0; i < m_numPlayers; ++i) {
        if (!m_players[i].folded && (m_players[i].chips > 0 || m_players[i].totalHandBet > 0 || m_players[i].allIn)) {
            cnt++;
        }
    }
    return cnt;
}

int PokerGame::countNonFoldedNonAllIn() const {
    int cnt = 0;
    for (int i = 0; i < m_numPlayers; ++i) {
        if (!m_players[i].folded && !m_players[i].allIn && m_players[i].chips > 0) {
            cnt++;
        }
    }
    return cnt;
}

void PokerGame::recordWinner(int playerId, int payout, const std::string& desc) {
    if (m_numLastWinners < 8) {
        m_lastWinners[m_numLastWinners++] = PotWinner{playerId, payout, desc};
    }
}

void PokerGame::startNewHand() {
    int playersWithChips = 0;
    int solePlayerId = 0;
    for (int i = 0; i < m_numPlayers; ++i) {
        if (m_players[i].chips > 0) {
            playersWithChips++;
            solePlayerId = m_players[i].id;
        }
    }

    if (playersWithChips <= 1) {
        m_finished = true;
        m_winner = solePlayerId;
        m_street = PokerStreet::HAND_OVER;
        return;
    }

    m_handNumber++;
    m_pot = 0;
    m_numLastWinners = 0;
    m_communityCount = 0;
    for (auto& c : m_communityCards) c = Card{0, 0};

    // Rotate dealer button to next active player
    for (int step = 1; step <= m_numPlayers; ++step) {
        int nextD = (m_dealerSeat + step) % m_numPlayers;
        if (m_players[nextD].chips > 0) {
            m_dealerSeat = nextD;
            break;
        }
    }

    // Reset player round variables
    for (int i = 0; i < m_numPlayers; ++i) {
        m_players[i].currentBet = 0;
        m_players[i].totalHandBet = 0;
        m_players[i].folded = (m_players[i].chips == 0);
        m_players[i].allIn = false;
        m_players[i].hasActed = false;
        m_players[i].handDescription = "";
        m_players[i].holeCards[0] = Card{0, 0};
        m_players[i].holeCards[1] = Card{0, 0};
    }

    shuffleDeck();
    dealHoleCards();
    postBlinds();

    m_street = PokerStreet::PRE_FLOP;
    m_turnTimer = 20.0f;
    m_botTimer = m_botDelay;
    m_lastAction = "Hand #" + std::to_string(m_handNumber) + " started";
}

void PokerGame::dealHoleCards() {
    for (int cardNum = 0; cardNum < 2; ++cardNum) {
        for (int i = 0; i < m_numPlayers; ++i) {
            int seat = (m_dealerSeat + 1 + i) % m_numPlayers;
            if (!m_players[seat].folded && m_deckTop < 52) {
                m_players[seat].holeCards[cardNum] = m_deck[m_deckTop++];
            }
        }
    }
}

void PokerGame::postBlinds() {
    int activeCount = 0;
    for (int i = 0; i < m_numPlayers; ++i) {
        if (!m_players[i].folded) activeCount++;
    }

    int sbSeat = -1;
    int bbSeat = -1;

    if (activeCount == 2) {
        // Heads up: Dealer is SB, other is BB
        sbSeat = m_dealerSeat;
        for (int i = 0; i < m_numPlayers; ++i) {
            if (i != m_dealerSeat && !m_players[i].folded) {
                bbSeat = i;
                break;
            }
        }
    } else {
        // 3-4 players: SB is clockwise from Dealer, BB is clockwise from SB
        for (int step = 1; step <= m_numPlayers; ++step) {
            int s = (m_dealerSeat + step) % m_numPlayers;
            if (!m_players[s].folded) {
                if (sbSeat == -1) sbSeat = s;
                else if (bbSeat == -1) { bbSeat = s; break; }
            }
        }
    }

    // Post SB
    if (sbSeat != -1) {
        int sbAmt = std::min(m_players[sbSeat].chips, m_smallBlind);
        m_players[sbSeat].chips -= sbAmt;
        m_players[sbSeat].currentBet = sbAmt;
        m_players[sbSeat].totalHandBet = sbAmt;
        if (m_players[sbSeat].chips == 0) m_players[sbSeat].allIn = true;
    }

    // Post BB
    if (bbSeat != -1) {
        int bbAmt = std::min(m_players[bbSeat].chips, m_bigBlind);
        m_players[bbSeat].chips -= bbAmt;
        m_players[bbSeat].currentBet = bbAmt;
        m_players[bbSeat].totalHandBet = bbAmt;
        if (m_players[bbSeat].chips == 0) m_players[bbSeat].allIn = true;
    }

    m_currentHighestBet = m_bigBlind;
    m_minRaise = m_bigBlind;

    // Action starts clockwise after BB (or in heads up, SB acts first pre-flop)
    if (activeCount == 2) {
        m_currentTurn = sbSeat + 1;
    } else {
        int utg = bbSeat;
        for (int step = 1; step <= m_numPlayers; ++step) {
            int s = (bbSeat + step) % m_numPlayers;
            if (!m_players[s].folded && !m_players[s].allIn) {
                utg = s;
                break;
            }
        }
        m_currentTurn = utg + 1;
    }
}

bool PokerGame::checkBettingRoundComplete() const {
    int activeUnfolded = 0;
    int playersCanAct = 0;

    for (int i = 0; i < m_numPlayers; ++i) {
        if (!m_players[i].folded) {
            activeUnfolded++;
            if (!m_players[i].allIn && m_players[i].chips > 0) {
                playersCanAct++;
                if (!m_players[i].hasActed) return false;
                if (m_players[i].currentBet != m_currentHighestBet) return false;
            }
        }
    }

    if (activeUnfolded <= 1) return true;
    if (playersCanAct <= 1) {
        // If at most 1 player can act, check if their bet matches highest
        for (int i = 0; i < m_numPlayers; ++i) {
            if (!m_players[i].folded && !m_players[i].allIn && m_players[i].chips > 0) {
                if (m_players[i].currentBet < m_currentHighestBet) return false;
            }
        }
        return true;
    }
    return true;
}

void PokerGame::collectBetsIntoPot() {
    for (int i = 0; i < m_numPlayers; ++i) {
        m_pot += m_players[i].currentBet;
        m_players[i].currentBet = 0;
        m_players[i].hasActed = false;
    }
}

void PokerGame::advanceStreet() {
    collectBetsIntoPot();
    m_currentHighestBet = 0;
    m_minRaise = m_bigBlind;

    if (m_street == PokerStreet::PRE_FLOP) {
        m_street = PokerStreet::FLOP;
        // Flop: 3 cards
        if (m_deckTop < 52) m_deckTop++; // burn
        for (int i = 0; i < 3 && m_deckTop < 52; ++i) {
            m_communityCards[i] = m_deck[m_deckTop++];
        }
        m_communityCount = 3;
        m_lastAction = "Flop dealt";
    } else if (m_street == PokerStreet::FLOP) {
        m_street = PokerStreet::TURN;
        // Turn: 1 card
        if (m_deckTop < 52) m_deckTop++; // burn
        if (m_deckTop < 52) {
            m_communityCards[3] = m_deck[m_deckTop++];
        }
        m_communityCount = 4;
        m_lastAction = "Turn dealt";
    } else if (m_street == PokerStreet::TURN) {
        m_street = PokerStreet::RIVER;
        // River: 1 card
        if (m_deckTop < 52) m_deckTop++; // burn
        if (m_deckTop < 52) {
            m_communityCards[4] = m_deck[m_deckTop++];
        }
        m_communityCount = 5;
        m_lastAction = "River dealt";
    } else if (m_street == PokerStreet::RIVER) {
        m_street = PokerStreet::SHOWDOWN;
        executeShowdown();
        return;
    }

    // If at most 1 active player can act (all others are all-in or folded), run out the board!
    if (countNonFoldedNonAllIn() <= 1 && countActiveNonFolded() > 1) {
        while (m_communityCount < 5 && m_deckTop < 52) {
            m_communityCards[m_communityCount++] = m_deck[m_deckTop++];
        }
        executeShowdown();
        return;
    }

    // Next round action starts with first active player clockwise from dealer
    int nextSeat = getNextActiveSeat(m_dealerSeat);
    if (nextSeat != -1) {
        m_currentTurn = nextSeat + 1;
        m_turnTimer = 20.0f;
        m_botTimer = m_botDelay;
    } else {
        executeShowdown();
    }
}

void PokerGame::executeShowdown() {
    collectBetsIntoPot();

    // Evaluate 7 cards for each active player
    for (int i = 0; i < m_numPlayers; ++i) {
        if (!m_players[i].folded && (m_players[i].chips > 0 || m_players[i].totalHandBet > 0 || m_players[i].allIn)) {
            std::array<Card, 7> seven = {
                m_players[i].holeCards[0],
                m_players[i].holeCards[1],
                m_communityCards[0],
                m_communityCards[1],
                m_communityCards[2],
                m_communityCards[3],
                m_communityCards[4]
            };
            m_players[i].bestScore = evaluate7Cards(seven);
            m_players[i].handDescription = describeHand(m_players[i].bestScore);
        }
    }

    // Side-pot calculation:
    // Gather distinct positive totalHandBet contribution thresholds
    std::vector<int> levels;
    for (int i = 0; i < m_numPlayers; ++i) {
        int bet = m_players[i].totalHandBet;
        if (bet > 0 && std::find(levels.begin(), levels.end(), bet) == levels.end()) {
            levels.push_back(bet);
        }
    }
    std::sort(levels.begin(), levels.end());

    int prevLevel = 0;
    for (int level : levels) {
        int step = level - prevLevel;
        if (step <= 0) continue;

        int sliceAmount = 0;
        std::vector<int> eligible;

        for (int i = 0; i < m_numPlayers; ++i) {
            int bet = m_players[i].totalHandBet;
            if (bet >= level) {
                sliceAmount += step;
                if (!m_players[i].folded) eligible.push_back(i);
            } else if (bet > prevLevel) {
                sliceAmount += (bet - prevLevel);
                if (!m_players[i].folded) eligible.push_back(i);
            }
        }

        prevLevel = level;
        if (sliceAmount <= 0) continue;

        if (eligible.empty()) {
            // Give to last non-folded player if any
            for (int i = 0; i < m_numPlayers; ++i) {
                if (!m_players[i].folded) {
                    m_players[i].chips += sliceAmount;
                    recordWinner(m_players[i].id, sliceAmount, m_players[i].handDescription);
                    break;
                }
            }
            continue;
        }

        if (eligible.size() == 1) {
            int winSeat = eligible[0];
            m_players[winSeat].chips += sliceAmount;
            recordWinner(m_players[winSeat].id, sliceAmount, m_players[winSeat].handDescription);
            continue;
        }

        // Find best HandScore among eligible
        HandScore best{};
        bool first = true;
        for (int seat : eligible) {
            if (first || m_players[seat].bestScore > best) {
                best = m_players[seat].bestScore;
                first = false;
            }
        }

        std::vector<int> winners;
        for (int seat : eligible) {
            if (m_players[seat].bestScore == best) {
                winners.push_back(seat);
            }
        }

        int share = sliceAmount / static_cast<int>(winners.size());
        int remainder = sliceAmount % static_cast<int>(winners.size());

        for (size_t w = 0; w < winners.size(); ++w) {
            int winSeat = winners[w];
            int payout = share + (w < static_cast<size_t>(remainder) ? 1 : 0);
            m_players[winSeat].chips += payout;
            recordWinner(m_players[winSeat].id, payout, m_players[winSeat].handDescription);
        }
    }

    m_pot = 0;
    m_street = PokerStreet::HAND_OVER;
    m_handOverTimer = 4.0f;
    m_lastAction = "Showdown finished";

    // Check match completion
    int active = 0;
    int winnerId = 0;
    for (int i = 0; i < m_numPlayers; ++i) {
        if (m_players[i].chips > 0) {
            active++;
            winnerId = m_players[i].id;
        }
    }
    if (active <= 1) {
        m_finished = true;
        m_winner = winnerId;
    }
}

void PokerGame::awardPotToRemainingPlayer(int winnerSeatIdx) {
    collectBetsIntoPot();
    m_players[winnerSeatIdx].chips += m_pot;
    recordWinner(m_players[winnerSeatIdx].id, m_pot, "All opponents folded");
    m_lastAction = m_players[winnerSeatIdx].name + " won " + std::to_string(m_pot) + " chips";
    m_pot = 0;
    m_street = PokerStreet::HAND_OVER;
    m_handOverTimer = 4.0f;

    int active = 0;
    int winnerId = 0;
    for (int i = 0; i < m_numPlayers; ++i) {
        if (m_players[i].chips > 0) {
            active++;
            winnerId = m_players[i].id;
        }
    }
    if (active <= 1) {
        m_finished = true;
        m_winner = winnerId;
    }
}

void PokerGame::nextTurn() {
    int curSeat = m_currentTurn - 1;
    for (int step = 1; step <= m_numPlayers; ++step) {
        int nextSeat = (curSeat + step) % m_numPlayers;
        if (!m_players[nextSeat].folded && !m_players[nextSeat].allIn && m_players[nextSeat].chips > 0) {
            m_currentTurn = nextSeat + 1;
            m_turnTimer = 20.0f;
            m_botTimer = m_botDelay;
            return;
        }
    }

    // No one left who can act -> advance street
    advanceStreet();
}

void PokerGame::executePlayerAction(int seatIdx, const std::string& action, int amount) {
    if (seatIdx < 0 || seatIdx >= m_numPlayers) return;
    auto& p = m_players[seatIdx];
    if (p.folded || p.allIn) return;

    if (action == "fold") {
        p.folded = true;
        p.hasActed = true;
        m_lastAction = p.name + " folded";

        if (countActiveNonFolded() <= 1) {
            for (int i = 0; i < m_numPlayers; ++i) {
                if (!m_players[i].folded) {
                    awardPotToRemainingPlayer(i);
                    return;
                }
            }
            return;
        }
    } else if (action == "check") {
        if (p.currentBet < m_currentHighestBet) {
            // Cannot check when facing a bet: auto-fold
            executePlayerAction(seatIdx, "fold", 0);
            return;
        }
        p.hasActed = true;
        m_lastAction = p.name + " checked";
    } else if (action == "call") {
        int toCall = m_currentHighestBet - p.currentBet;
        if (toCall <= 0) {
            p.hasActed = true;
            m_lastAction = p.name + " checked";
        } else {
            int commit = std::min(toCall, p.chips);
            p.chips -= commit;
            p.currentBet += commit;
            p.totalHandBet += commit;
            if (p.chips == 0) p.allIn = true;
            p.hasActed = true;
            m_lastAction = p.name + (p.allIn ? " called (All-in)" : " called");
        }
    } else if (action == "raise" || action == "bet") {
        int targetBet = (amount > m_currentHighestBet) ? amount : (m_currentHighestBet + amount);
        if (targetBet - m_currentHighestBet < m_minRaise && (targetBet - p.currentBet) < p.chips) {
            targetBet = m_currentHighestBet + m_minRaise;
        }
        int needed = targetBet - p.currentBet;
        if (needed >= p.chips) {
            // All-in raise
            int commit = p.chips;
            p.chips = 0;
            p.currentBet += commit;
            p.totalHandBet += commit;
            p.allIn = true;
            if (p.currentBet > m_currentHighestBet) {
                int r = p.currentBet - m_currentHighestBet;
                if (r >= m_minRaise) m_minRaise = r;
                m_currentHighestBet = p.currentBet;
                for (int i = 0; i < m_numPlayers; ++i) {
                    if (i != seatIdx && !m_players[i].folded && !m_players[i].allIn) {
                        m_players[i].hasActed = false;
                    }
                }
            }
            m_lastAction = p.name + " raised All-In to " + std::to_string(p.currentBet);
        } else {
            p.chips -= needed;
            p.currentBet += needed;
            p.totalHandBet += needed;
            int r = p.currentBet - m_currentHighestBet;
            if (r > m_minRaise) m_minRaise = r;
            m_currentHighestBet = p.currentBet;
            for (int i = 0; i < m_numPlayers; ++i) {
                if (i != seatIdx && !m_players[i].folded && !m_players[i].allIn) {
                    m_players[i].hasActed = false;
                }
            }
            m_lastAction = p.name + " raised to " + std::to_string(p.currentBet);
        }
        p.hasActed = true;
    } else if (action == "all_in") {
        int commit = p.chips;
        p.chips = 0;
        p.currentBet += commit;
        p.totalHandBet += commit;
        p.allIn = true;
        if (p.currentBet > m_currentHighestBet) {
            int r = p.currentBet - m_currentHighestBet;
            if (r >= m_minRaise) m_minRaise = r;
            m_currentHighestBet = p.currentBet;
            for (int i = 0; i < m_numPlayers; ++i) {
                if (i != seatIdx && !m_players[i].folded && !m_players[i].allIn) {
                    m_players[i].hasActed = false;
                }
            }
        }
        p.hasActed = true;
        m_lastAction = p.name + " pushed All-In (" + std::to_string(p.currentBet) + ")";
    }

    if (checkBettingRoundComplete()) {
        advanceStreet();
    } else {
        nextTurn();
    }
}

void PokerGame::executeBotMove(int seatIdx) {
    if (seatIdx < 0 || seatIdx >= m_numPlayers) return;
    const auto& bot = m_players[seatIdx];
    int toCall = m_currentHighestBet - bot.currentBet;

    if (m_street == PokerStreet::PRE_FLOP) {
        int r1 = bot.holeCards[0].rank;
        int r2 = bot.holeCards[1].rank;
        bool pair = (r1 == r2);
        bool suited = (bot.holeCards[0].suit == bot.holeCards[1].suit);
        int high = std::max(r1, r2);

        if (pair && high >= 10) {
            // Strong pair (TT, JJ, QQ, KK, AA)
            if (toCall == 0 || toCall <= m_bigBlind * 2) {
                executePlayerAction(seatIdx, "raise", m_currentHighestBet + m_bigBlind * 2);
            } else {
                executePlayerAction(seatIdx, "call", 0);
            }
        } else if (pair || (high >= 12) || (suited && high >= 10)) {
            // Medium hand
            if (toCall <= m_bigBlind * 2) {
                executePlayerAction(seatIdx, "call", 0);
            } else if (toCall == 0) {
                executePlayerAction(seatIdx, "check", 0);
            } else {
                executePlayerAction(seatIdx, "fold", 0);
            }
        } else {
            // Weak hand
            if (toCall == 0) {
                executePlayerAction(seatIdx, "check", 0);
            } else {
                executePlayerAction(seatIdx, "fold", 0);
            }
        }
    } else {
        // Post-flop evaluation
        std::array<Card, 7> seven = {
            bot.holeCards[0], bot.holeCards[1],
            m_communityCards[0], m_communityCards[1], m_communityCards[2],
            m_communityCount >= 4 ? m_communityCards[3] : Card{0, 0},
            m_communityCount >= 5 ? m_communityCards[4] : Card{0, 0}
        };
        HandScore currentScore = evaluate7Cards(seven);

        if (currentScore.category >= static_cast<int>(PokerCategory::THREE_OF_A_KIND)) {
            // Monster
            if (toCall == 0) {
                executePlayerAction(seatIdx, "raise", m_currentHighestBet + m_bigBlind);
            } else {
                executePlayerAction(seatIdx, "call", 0);
            }
        } else if (currentScore.category >= static_cast<int>(PokerCategory::ONE_PAIR)) {
            // Pair or Two Pair
            if (toCall == 0) {
                executePlayerAction(seatIdx, "check", 0);
            } else if (toCall <= m_bigBlind * 3) {
                executePlayerAction(seatIdx, "call", 0);
            } else {
                executePlayerAction(seatIdx, "fold", 0);
            }
        } else {
            // High card
            if (toCall == 0) {
                executePlayerAction(seatIdx, "check", 0);
            } else {
                executePlayerAction(seatIdx, "fold", 0);
            }
        }
    }
}

void PokerGame::update(float dt) {
    if (m_finished) return;

    if (m_street == PokerStreet::HAND_OVER) {
        if (m_isSolo) {
            m_handOverTimer -= dt;
            if (m_handOverTimer <= 0.0f) {
                startNewHand();
            }
        }
        return;
    }

    // Active betting street
    m_turnTimer -= dt;
    if (m_turnTimer <= 0.0f) {
        int curSeat = m_currentTurn - 1;
        if (curSeat >= 0 && curSeat < m_numPlayers) {
            if (m_players[curSeat].currentBet == m_currentHighestBet) {
                executePlayerAction(curSeat, "check", 0);
            } else {
                executePlayerAction(curSeat, "fold", 0);
            }
        }
        return;
    }

    // Bot move
    int curSeat = m_currentTurn - 1;
    if (curSeat >= 0 && curSeat < m_numPlayers && m_players[curSeat].isBot && !m_players[curSeat].folded && !m_players[curSeat].allIn) {
        m_botTimer -= dt;
        if (m_botTimer <= 0.0f) {
            executeBotMove(curSeat);
        }
    }
}

void PokerGame::handleInput(PlayerId player, const std::string& input) {
    if (input.find("\"cmd\":\"next_hand\"") != std::string::npos ||
        input.find("\"action\":\"next_hand\"") != std::string::npos) {
        if (m_street == PokerStreet::HAND_OVER && !m_finished) {
            startNewHand();
        }
        return;
    }

    int seatIdx = static_cast<int>(player) - 1;
    if (seatIdx < 0 || seatIdx >= m_numPlayers) return;
    if (m_currentTurn != static_cast<int>(player)) return;

    std::string act = "";
    int amt = 0;

    auto getVal = [&](const std::string& key) -> std::string {
        auto pos = input.find("\"" + key + "\":");
        if (pos == std::string::npos) return "";
        auto start = pos + key.length() + 3;
        while (start < input.length() && (input[start] == ' ' || input[start] == '\"')) start++;
        auto end = input.find_first_of("\",}", start);
        if (end == std::string::npos) return input.substr(start);
        return input.substr(start, end - start);
    };

    act = getVal("cmd");
    if (act.empty()) act = getVal("action");

    std::string amtStr = getVal("amount");
    if (!amtStr.empty()) {
        try { amt = std::stoi(amtStr); } catch (...) { amt = 0; }
    }

    if (!act.empty()) {
        executePlayerAction(seatIdx, act, amt);
    }
}

std::string PokerGame::serializeState() const {
    return serializeStateForPlayer(1);
}

std::string PokerGame::serializeStateForPlayer(PlayerId id) const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"type\":\"state\",";
    oss << "\"game\":\"poker\",";
    
    static const char* streetNames[] = {
        "STARTING_HAND", "PRE_FLOP", "FLOP", "TURN", "RIVER", "SHOWDOWN", "HAND_OVER"
    };
    oss << "\"street\":\"" << streetNames[static_cast<int>(m_street)] << "\",";
    oss << "\"hand_number\":" << m_handNumber << ",";
    oss << "\"pot\":" << m_pot << ",";
    oss << "\"current_bet\":" << m_currentHighestBet << ",";
    oss << "\"min_raise\":" << m_minRaise << ",";
    oss << "\"current_turn\":" << m_currentTurn << ",";
    oss << "\"dealer_seat\":" << (m_dealerSeat + 1) << ",";
    oss << "\"turn_time_left\":" << std::max(0.0f, m_turnTimer) << ",";
    oss << "\"is_solo\":" << (m_isSolo ? "true" : "false") << ",";
    oss << "\"finished\":" << (m_finished ? "true" : "false") << ",";
    oss << "\"winner\":" << m_winner << ",";
    oss << "\"last_action\":\"" << m_lastAction << "\",";

    // Community cards
    oss << "\"community_cards\":[";
    for (int i = 0; i < m_communityCount; ++i) {
        if (i > 0) oss << ",";
        oss << "{\"rank\":" << m_communityCards[i].rank
            << ",\"suit\":" << m_communityCards[i].suit
            << ",\"code\":\"" << m_communityCards[i].toString() << "\"}";
    }
    oss << "],";

    // Players
    oss << "\"players\":[";
    bool revealAll = (m_street == PokerStreet::SHOWDOWN || m_street == PokerStreet::HAND_OVER);

    for (int i = 0; i < m_numPlayers; ++i) {
        if (i > 0) oss << ",";
        const auto& p = m_players[i];
        oss << "{\"id\":" << p.id
            << ",\"name\":\"" << p.name << "\""
            << ",\"chips\":" << p.chips
            << ",\"bet\":" << p.currentBet
            << ",\"total_bet\":" << p.totalHandBet
            << ",\"folded\":" << (p.folded ? "true" : "false")
            << ",\"all_in\":" << (p.allIn ? "true" : "false")
            << ",\"is_bot\":" << (p.isBot ? "true" : "false")
            << ",\"cards\":[";

        bool showHole = (p.id == static_cast<int>(id)) || revealAll;
        if (!p.folded && p.holeCards[0].isValid()) {
            if (showHole) {
                oss << "{\"rank\":" << p.holeCards[0].rank
                    << ",\"suit\":" << p.holeCards[0].suit
                    << ",\"code\":\"" << p.holeCards[0].toString() << "\"},"
                    << "{\"rank\":" << p.holeCards[1].rank
                    << ",\"suit\":" << p.holeCards[1].suit
                    << ",\"code\":\"" << p.holeCards[1].toString() << "\"}";
            } else {
                oss << "{\"hidden\":true},{\"hidden\":true}";
            }
        }
        oss << "]";

        if (!p.handDescription.empty() && showHole) {
            oss << ",\"hand_rank\":\"" << p.handDescription << "\"";
        } else if (p.id == static_cast<int>(id) && m_communityCount >= 3 && p.holeCards[0].isValid()) {
            std::array<Card, 7> curSeven = {
                p.holeCards[0], p.holeCards[1],
                m_communityCards[0], m_communityCards[1], m_communityCards[2],
                m_communityCount >= 4 ? m_communityCards[3] : Card{0, 0},
                m_communityCount >= 5 ? m_communityCards[4] : Card{0, 0}
            };
            HandScore hs = evaluate7Cards(curSeven);
            oss << ",\"hand_rank\":\"" << describeHand(hs) << "\"";
        }
        oss << "}";
    }
    oss << "],";

    // Winners
    oss << "\"winners\":[";
    for (int i = 0; i < m_numLastWinners; ++i) {
        if (i > 0) oss << ",";
        oss << "{\"id\":" << m_lastWinners[i].playerId
            << ",\"payout\":" << m_lastWinners[i].payout
            << ",\"hand_rank\":\"" << m_lastWinners[i].handDescription << "\"}";
    }
    oss << "]";

    oss << "}";
    return oss.str();
}

} // namespace core
} // namespace gamehub
