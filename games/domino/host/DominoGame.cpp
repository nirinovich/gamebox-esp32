#include "DominoGame.hpp"
#include <gamehub/core/GameRegistry.hpp>
#include <sstream>
#include <algorithm>
#include <random>
#include <charconv>

namespace {
std::unique_ptr<gamehub::core::Game> createDominoSolo() {
    return std::make_unique<gamehub::core::DominoGame>(true, 2);
}
std::unique_ptr<gamehub::core::Game> createDominoDuel() {
    return std::make_unique<gamehub::core::DominoGame>(false, 2);
}
std::unique_ptr<gamehub::core::Game> createDominoPvp() {
    return std::make_unique<gamehub::core::DominoGame>(false, 3);
}
}

REGISTER_GAME("domino_solo", DominoSolo, &createDominoSolo, {
    gamehub::core::GameId("domino_solo"),
    "Dominoes Solo",
    "SOLO",
    "Double-six Malagasy Tsipy Dominoes vs Bot",
    1, 1,
    gamehub::core::InputFlag::ACTION_BUTTONS,
    "domino_solo",
    false
})

REGISTER_GAME("domino_duel", DominoDuel, &createDominoDuel, {
    gamehub::core::GameId("domino_duel"),
    "Domino Duel",
    "DUEL",
    "2P Malagasy Tsipy Dominoes duel with Deadwood & Rebon",
    2, 2,
    gamehub::core::InputFlag::ACTION_BUTTONS,
    "domino_solo",
    true
})

REGISTER_GAME("domino_pvp", DominoPvp, &createDominoPvp, {
    gamehub::core::GameId("domino_pvp"),
    "Malagasy Domino Table",
    "PARTY",
    "2-4P Malagasy Tsipy Dominoes with Rebon & TargetScore",
    2, 4,
    gamehub::core::InputFlag::ACTION_BUTTONS,
    "domino_pvp",
    false
})

namespace gamehub {
namespace core {

DominoGame::DominoGame(bool isSolo, int numPlayers, int targetScore)
    : m_isSolo(isSolo), m_numPlayers(std::clamp(numPlayers, 2, 4)), m_targetScore(targetScore > 0 ? targetScore : 100) {
    init();
}

void DominoGame::setNumPlayers(int n) {
    int clamped = std::clamp(n, 2, 4);
    if (clamped == m_numPlayers) return;
    m_numPlayers = clamped;
    init();
}

void DominoGame::init() {
    for (int i = 0; i < 4; ++i) {
        m_cumulativeScores[i] = 0;
    }
    m_rebonPool = 0;
    m_round = 1;
    m_finished = false;
    m_winner = 0;
    startNewRound();
}

void DominoGame::startNewRound() {
    m_board.clear();
    m_deadwood.clear();
    for (int i = 0; i < 4; ++i) m_hands[i].clear();
    m_roundFinished = false;
    m_roundWinner = 0;
    m_roundPointsAwarded = 0;
    m_consecutivePasses = 0;
    m_lastAction.clear();

    // 1. Generate full Double-Six 28-tile set
    std::vector<DominoTile> allTiles;
    allTiles.reserve(28);
    for (int i = 0; i <= 6; ++i) {
        for (int j = i; j <= 6; ++j) {
            allTiles.push_back({i, j});
        }
    }

    // 2. Deterministic shuffle
    static std::mt19937 rng(42);
    std::shuffle(allTiles.begin(), allTiles.end(), rng);

    // 3. Deal exactly 7 tiles per player
    int tileIdx = 0;
    for (int p = 0; p < m_numPlayers; ++p) {
        for (int t = 0; t < 7 && tileIdx < (int)allTiles.size(); ++t) {
            m_hands[p].push_back(allTiles[tileIdx++]);
        }
    }

    // 4. Remaining tiles form inaccessible Deadwood (14 in 2P, 7 in 3P, 0 in 4P)
    while (tileIdx < (int)allTiles.size()) {
        m_deadwood.push_back(allTiles[tileIdx++]);
    }

    // 5. Determine opening lead (highest double held)
    determineOpeningLead();

    if (m_isSolo && !m_finished && !m_roundFinished && m_turn != 1) {
        executeBotTurns();
    }
}

void DominoGame::determineOpeningLead() {
    // Check doubles from [6|6] down to [0|0]
    for (int d = 6; d >= 0; --d) {
        for (int p = 0; p < m_numPlayers; ++p) {
            for (const auto& t : m_hands[p]) {
                if (t.left == d && t.right == d) {
                    m_turn = p + 1;
                    return;
                }
            }
        }
    }

    // Fallback if no doubles held in player hands: highest pip sum
    int maxPips = -1;
    int bestP = 0;
    for (int p = 0; p < m_numPlayers; ++p) {
        for (const auto& t : m_hands[p]) {
            if (t.pips() > maxPips) {
                maxPips = t.pips();
                bestP = p;
            }
        }
    }
    m_turn = bestP + 1;
}

void DominoGame::update(float /*dt*/) {
    if (m_isSolo && !m_finished && !m_roundFinished && m_turn != 1) {
        executeBotTurns();
    }
}

int DominoGame::getLeftEnd() const {
    if (m_board.empty()) return -1;
    return m_board.front().left;
}

int DominoGame::getRightEnd() const {
    if (m_board.empty()) return -1;
    return m_board.back().right;
}

bool DominoGame::canPlayerPlay(int pidx) const {
    if (pidx < 0 || pidx >= m_numPlayers) return false;
    if (m_board.empty()) return !m_hands[pidx].empty();
    int l = getLeftEnd();
    int r = getRightEnd();
    for (const auto& t : m_hands[pidx]) {
        if (t.matches(l) || t.matches(r)) return true;
    }
    return false;
}

bool DominoGame::playTile(int pidx, int tileIdx, const std::string& endChoice) {
    if (pidx < 0 || pidx >= m_numPlayers) return false;
    if (tileIdx < 0 || tileIdx >= (int)m_hands[pidx].size()) return false;
    if (m_roundFinished || m_finished) return false;

    DominoTile tile = m_hands[pidx][tileIdx];

    if (m_board.empty()) {
        m_board.push_back(tile);
    } else {
        int l = getLeftEnd();
        int r = getRightEnd();
        bool playLeft = (endChoice == "left");
        bool playRight = (endChoice == "right");

        if (!playLeft && !playRight) {
            if (tile.matches(r)) playRight = true;
            else if (tile.matches(l)) playLeft = true;
            else return false;
        }

        if (playLeft) {
            if (tile.right == l) {
                m_board.push_front(tile);
            } else if (tile.left == l) {
                tile.flip();
                m_board.push_front(tile);
            } else if (playRight && tile.matches(r)) {
                if (tile.left == r) {
                    m_board.push_back(tile);
                } else {
                    tile.flip();
                    m_board.push_back(tile);
                }
            } else {
                return false;
            }
        } else { // playRight
            if (tile.left == r) {
                m_board.push_back(tile);
            } else if (tile.right == r) {
                tile.flip();
                m_board.push_back(tile);
            } else if (tile.matches(l)) {
                if (tile.right == l) {
                    m_board.push_front(tile);
                } else {
                    tile.flip();
                    m_board.push_front(tile);
                }
            } else {
                return false;
            }
        }
    }

    // Remove played tile
    m_hands[pidx].erase(m_hands[pidx].begin() + tileIdx);
    m_consecutivePasses = 0;

    // Check Round Win (Hand emptied: "Domino" / "Lany")
    if (m_hands[pidx].empty()) {
        int oppPips = 0;
        for (int p = 0; p < m_numPlayers; ++p) {
            if (p != pidx) {
                for (const auto& t : m_hands[p]) oppPips += t.pips();
            }
        }
        int awarded = oppPips + m_rebonPool;
        m_rebonPool = 0;
        m_cumulativeScores[pidx] += awarded;
        m_roundWinner = pidx + 1;
        m_roundPointsAwarded = awarded;
        m_roundFinished = true;

        if (m_cumulativeScores[pidx] >= m_targetScore) {
            m_finished = true;
            m_winner = pidx + 1;
        }
        return true;
    }

    m_turn = (m_turn % m_numPlayers) + 1;
    return true;
}

void DominoGame::passTurn(int pidx) {
    if (pidx < 0 || pidx >= m_numPlayers) return;
    if (m_roundFinished || m_finished) return;

    // Strict Malagasy rule: passing only allowed when player has zero legal moves
    if (canPlayerPlay(pidx)) return;

    m_consecutivePasses++;
    if (m_consecutivePasses >= m_numPlayers) {
        checkBlockedWin();
        return;
    }
    m_turn = (m_turn % m_numPlayers) + 1;
}

void DominoGame::checkBlockedWin() {
    m_roundFinished = true;
    int minPips = 9999;
    int lowestPlayer = 0;
    int tieCount = 0;

    int playerPips[4]{0, 0, 0, 0};
    int totalRemainingPips = 0;

    for (int p = 0; p < m_numPlayers; ++p) {
        for (const auto& t : m_hands[p]) {
            playerPips[p] += t.pips();
        }
        totalRemainingPips += playerPips[p];
        if (playerPips[p] < minPips) {
            minPips = playerPips[p];
            lowestPlayer = p;
            tieCount = 1;
        } else if (playerPips[p] == minPips) {
            tieCount++;
        }
    }

    if (tieCount == 1) {
        // Unique lowest pip holder wins opponents' pips + rollover rebon
        int oppPips = totalRemainingPips - minPips;
        int awarded = oppPips + m_rebonPool;
        m_rebonPool = 0;
        m_cumulativeScores[lowestPlayer] += awarded;
        m_roundWinner = lowestPlayer + 1;
        m_roundPointsAwarded = awarded;

        if (m_cumulativeScores[lowestPlayer] >= m_targetScore) {
            m_finished = true;
            m_winner = lowestPlayer + 1;
        }
    } else {
        // Tie on lowest pip sum: Rebon rollover to next round!
        m_rebonPool += totalRemainingPips;
        m_roundWinner = 0;
        m_roundPointsAwarded = 0;
    }
}

void DominoGame::executeBotTurns() {
    while (m_isSolo && !m_finished && !m_roundFinished && m_turn != 1) {
        int bidx = m_turn - 1;
        std::string botName = (m_numPlayers > 2) ? ("Bot " + std::to_string(bidx)) : "Bot";

        int bestIdx = -1;
        int bestPips = -1;
        std::string bestEnd = "right";

        int l = getLeftEnd();
        int r = getRightEnd();

        for (int i = 0; i < (int)m_hands[bidx].size(); ++i) {
            const auto& t = m_hands[bidx][i];
            if (m_board.empty()) {
                if (t.pips() > bestPips) { bestPips = t.pips(); bestIdx = i; }
            } else {
                if (t.matches(r) && t.pips() > bestPips) {
                    bestPips = t.pips(); bestIdx = i; bestEnd = "right";
                } else if (t.matches(l) && t.pips() > bestPips) {
                    bestPips = t.pips(); bestIdx = i; bestEnd = "left";
                }
            }
        }

        if (bestIdx != -1) {
            DominoTile played = m_hands[bidx][bestIdx];
            playTile(bidx, bestIdx, bestEnd);
            m_lastAction = botName + " played [" + std::to_string(played.left) + "|" + std::to_string(played.right) + "]";
        } else {
            passTurn(bidx);
            m_lastAction = botName + " passed turn";
        }
    }
}

void DominoGame::handleInput(PlayerId player, const std::string& input) {
    if (m_finished) return;

    // Handle next round trigger when a hand has finished
    if (m_roundFinished) {
        if (input.find("\"next_round\"") != std::string::npos || input.find("\"cmd\":\"next_round\"") != std::string::npos) {
            startNewRound();
        }
        return;
    }

    int pidx = (int)player - 1;
    if (pidx < 0 || pidx >= m_numPlayers) return;
    if ((int)player != m_turn) return;

    // TargetScore configuration
    if (input.find("\"set_target\"") != std::string::npos) {
        auto tPos = input.find("\"target\":");
        if (tPos != std::string::npos) {
            int newTarget = 100;
            std::from_chars(input.data() + tPos + 9, input.data() + input.size(), newTarget);
            setTargetScore(newTarget);
        }
        return;
    }

    if (input.find("\"play\"") != std::string::npos || input.find("\"cmd\":\"play\"") != std::string::npos) {
        auto idxPos = input.find("\"index\":");
        int tileIdx = -1;
        if (idxPos != std::string::npos) {
            std::from_chars(input.data() + idxPos + 8, input.data() + input.size(), tileIdx);
        }
        std::string endChoice = "right";
        if (input.find("\"left\"") != std::string::npos) endChoice = "left";

        if (tileIdx >= 0 && tileIdx < (int)m_hands[pidx].size()) {
            DominoTile played = m_hands[pidx][tileIdx];
            if (playTile(pidx, tileIdx, endChoice)) {
                m_lastAction = "You played [" + std::to_string(played.left) + "|" + std::to_string(played.right) + "]";
                clearPendingChoice();
                if (m_isSolo && !m_finished && !m_roundFinished && m_turn != 1) {
                    executeBotTurns();
                }
            }
        }
    } else if (input.find("\"pass\"") != std::string::npos || input.find("\"cmd\":\"pass\"") != std::string::npos) {
        passTurn(pidx);
        m_lastAction = "You passed turn";
        if (m_isSolo && !m_finished && !m_roundFinished && m_turn != 1) {
            executeBotTurns();
        }
    }
}

std::string DominoGame::serializeState() const {
    return serializeStateForPlayer(1);
}

std::string DominoGame::serializeStateForPlayer(PlayerId id) const {
    std::ostringstream oss;
    oss << "{\"type\":\"state\",\"game\":\"domino\","
        << "\"num_players\":" << m_numPlayers << ","
        << "\"turn\":" << m_turn << ","
        << "\"finished\":" << (m_finished ? "true" : "false") << ","
        << "\"winner\":" << m_winner << ","
        << "\"round\":" << m_round << ","
        << "\"round_finished\":" << (m_roundFinished ? "true" : "false") << ","
        << "\"round_winner\":" << m_roundWinner << ","
        << "\"round_points\":" << m_roundPointsAwarded << ","
        << "\"target_score\":" << m_targetScore << ","
        << "\"rebon\":" << m_rebonPool << ","
        << "\"scores\":["
        << m_cumulativeScores[0] << "," << m_cumulativeScores[1] << ","
        << m_cumulativeScores[2] << "," << m_cumulativeScores[3] << "],"
        << "\"series_p1\":" << m_cumulativeScores[0] << ","
        << "\"series_p2\":" << m_cumulativeScores[1] << ","
        << "\"p1_count\":" << m_hands[0].size() << ","
        << "\"p2_count\":" << m_hands[1].size() << ","
        << "\"p3_count\":" << m_hands[2].size() << ","
        << "\"p4_count\":" << m_hands[3].size() << ","
        << "\"boneyard_count\":" << m_deadwood.size() << ","
        << "\"deadwood_count\":" << m_deadwood.size() << ","
        << "\"left_end\":" << getLeftEnd() << ","
        << "\"right_end\":" << getRightEnd() << ",";

    // Board tiles
    oss << "\"board\":[";
    for (size_t i = 0; i < m_board.size(); ++i) {
        if (i > 0) oss << ",";
        oss << "[" << m_board[i].left << "," << m_board[i].right << "]";
    }
    oss << "],";

    // Hands: Strictly mask opponents' hands
    oss << "\"hands\":{";
    bool firstHand = true;
    for (int p = 0; p < m_numPlayers; ++p) {
        int pNum = p + 1;
        // Expose hand if: (1) perspective id == pNum, or (2) id == 0 (debug/host), or (3) round finished
        if (id == 0 || (int)id == pNum || m_roundFinished || m_finished) {
            if (!firstHand) oss << ",";
            firstHand = false;
            oss << "\"" << pNum << "\":[";
            for (size_t t = 0; t < m_hands[p].size(); ++t) {
                if (t > 0) oss << ",";
                oss << "[" << m_hands[p][t].left << "," << m_hands[p][t].right << "]";
            }
            oss << "]";
        }
    }
    oss << "},";

    // ActionStream & Prompts
    oss << "\"available_actions\":[";
    bool canAct = ((int)id == m_turn && !m_finished && !m_roundFinished);
    bool firstAction = true;

    if (canAct) {
        int pidx = (int)id - 1;
        if (canPlayerPlay(pidx)) {
            oss << "{\"id\":\"play\",\"label\":\"Play Tile\",\"enabled\":true,\"style\":\"primary\"}";
            firstAction = false;
        } else {
            oss << "{\"id\":\"pass\",\"label\":\"Pass Turn\",\"enabled\":true,\"style\":\"highlight\"}";
            firstAction = false;
        }
    }

    if (m_roundFinished && !m_finished) {
        if (!firstAction) oss << ",";
        oss << "{\"id\":\"next_round\",\"label\":\"Next Round\",\"enabled\":true,\"style\":\"primary\"}";
    }
    oss << "],";

    if (!m_lastAction.empty()) {
        oss << "\"last_action\":\"" << m_lastAction << "\",";
    }

    oss << "\"game_name\":\"Malagasy Dominoes\"}";
    return oss.str();
}

} // namespace core
} // namespace gamehub
