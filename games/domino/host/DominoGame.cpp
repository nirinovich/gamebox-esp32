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
    "Double-six draw dominoes on emerald felt table vs Bot",
    1, 1,
    gamehub::core::InputFlag::ACTION_BUTTONS,
    "domino_solo",
    false
})

REGISTER_GAME("domino_duel", DominoDuel, &createDominoDuel, {
    gamehub::core::GameId("domino_duel"),
    "Domino Duel",
    "DUEL",
    "Head-to-head double-six domino match with Deadwood & Fog of War",
    2, 2,
    gamehub::core::InputFlag::ACTION_BUTTONS,
    "domino_solo",
    true
})

REGISTER_GAME("domino_pvp", DominoPvp, &createDominoPvp, {
    gamehub::core::GameId("domino_pvp"),
    "3P Party Dominoes",
    "PARTY",
    "3-Player social parlor dominoes with Deadwood & Fog of War",
    2, 3,
    gamehub::core::InputFlag::ACTION_BUTTONS,
    "domino_pvp",
    false
})

namespace gamehub {
namespace core {

DominoGame::DominoGame(bool isSolo, int numPlayers, bool isBlockMode)
    : m_isSolo(isSolo), m_numPlayers(std::clamp(numPlayers, 2, 3)), m_isBlockMode(isBlockMode) {
    init();
}

void DominoGame::setNumPlayers(int n) {
    int clamped = std::clamp(n, 2, 3);
    if (clamped == m_numPlayers) return;
    m_numPlayers = clamped;
    init();
}

void DominoGame::init() {
    m_board.clear();
    m_boneyard.clear();
    for (int i = 0; i < 3; ++i) m_hands[i].clear();
    m_turn = 1;
    m_finished = false;
    m_winner = 0;
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

    // 2. Shuffle tiles deterministically with fallback randomness
    static std::mt19937 rng(42);
    std::shuffle(allTiles.begin(), allTiles.end(), rng);

    // 3. Deal 7 tiles per player
    int tileIdx = 0;
    for (int p = 0; p < m_numPlayers; ++p) {
        for (int t = 0; t < 7 && tileIdx < (int)allTiles.size(); ++t) {
            m_hands[p].push_back(allTiles[tileIdx++]);
        }
    }

    // 4. Remaining tiles form the Boneyard
    while (tileIdx < (int)allTiles.size()) {
        m_boneyard.push_back(allTiles[tileIdx++]);
    }
}

void DominoGame::update(float /*dt*/) {
    if (m_isSolo && !m_finished && m_turn != 1) {
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

    DominoTile tile = m_hands[pidx][tileIdx];

    if (m_board.empty()) {
        m_board.push_back(tile);
    } else {
        int l = getLeftEnd();
        int r = getRightEnd();
        bool playLeft = (endChoice == "left");
        bool playRight = (endChoice == "right");

        // If not specified or ambiguous, auto-detect valid placement
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
            } else if (playRight && tile.matches(r)) { // Fallback if left invalid
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
            } else if (tile.matches(l)) { // Fallback if right invalid
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

    // Remove tile from hand
    m_hands[pidx].erase(m_hands[pidx].begin() + tileIdx);
    m_consecutivePasses = 0;

    // Check Win
    if (m_hands[pidx].empty()) {
        m_finished = true;
        m_winner = pidx + 1; // 1-indexed
        return true;
    }

    m_turn = (m_turn % m_numPlayers) + 1;
    return true;
}

bool DominoGame::drawTile(int pidx) {
    if (m_isBlockMode || m_boneyard.empty()) return false;
    m_hands[pidx].push_back(m_boneyard.back());
    m_boneyard.pop_back();
    m_consecutivePasses = 0;
    return true;
}

void DominoGame::passTurn(int pidx) {
    // Only allowed to pass if player cannot play any tile in hand
    if (canPlayerPlay(pidx)) return;

    m_consecutivePasses++;
    if (m_consecutivePasses >= m_numPlayers) {
        checkBlockedWin();
        return;
    }
    m_turn = (m_turn % m_numPlayers) + 1;
}

void DominoGame::checkBlockedWin() {
    m_finished = true;
    int minPips = 999;
    int winningPlayer = 0; // 0 = draw

    for (int p = 0; p < m_numPlayers; ++p) {
        int sum = 0;
        for (const auto& t : m_hands[p]) sum += t.pips();
        if (sum < minPips) {
            minPips = sum;
            winningPlayer = p + 1;
        } else if (sum == minPips) {
            winningPlayer = 0; // Tie on pips
        }
    }
    m_winner = winningPlayer;
}

void DominoGame::executeBotTurns() {
    while (m_isSolo && !m_finished && m_turn != 1) {
        int bidx = m_turn - 1;
        int drawnCount = 0;
        std::string botName = (m_numPlayers == 3) ? ("Bot " + std::to_string(bidx)) : "Bot";

        while (!m_finished && m_turn == bidx + 1) {
            // 1. Try to find highest pip playable tile
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
                if (drawnCount > 0) {
                    m_lastAction = botName + " drew " + std::to_string(drawnCount) + " tile" + (drawnCount > 1 ? "s" : "") + " & played [" + std::to_string(played.left) + "|" + std::to_string(played.right) + "]";
                } else {
                    m_lastAction = botName + " played [" + std::to_string(played.left) + "|" + std::to_string(played.right) + "]";
                }
                break;
            } else if (!m_isBlockMode && !m_boneyard.empty()) {
                drawTile(bidx);
                drawnCount++;
            } else {
                passTurn(bidx);
                if (drawnCount > 0) {
                    m_lastAction = botName + " drew " + std::to_string(drawnCount) + " tile" + (drawnCount > 1 ? "s" : "") + " & passed";
                } else {
                    m_lastAction = botName + " passed turn";
                }
                break;
            }
        }
    }
}

void DominoGame::handleInput(PlayerId player, const std::string& input) {
    if (m_finished) return;
    int pidx = (int)player - 1;
    if (pidx < 0 || pidx >= m_numPlayers) return;
    if ((int)player != m_turn) return;

    if (input.find("\"play\"") != std::string::npos || input.find("\"cmd\":\"play\"") != std::string::npos) {
        clearPendingChoice();
        // Parse tile index and end
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
                if (m_isSolo && !m_finished && m_turn != 1) {
                    executeBotTurns();
                }
            }
        }
    } else if (input.find("\"select_tile\"") != std::string::npos) {
        auto idxPos = input.find("\"index\":");
        int tileIdx = -1;
        if (idxPos != std::string::npos) {
            std::from_chars(input.data() + idxPos + 8, input.data() + input.size(), tileIdx);
        }
        if (tileIdx >= 0 && tileIdx < (int)m_hands[pidx].size()) {
            int l = getLeftEnd();
            int r = getRightEnd();
            const auto& t = m_hands[pidx][tileIdx];
            if (m_board.empty()) {
                playTile(pidx, tileIdx, "left");
            } else if (t.matches(l) && t.matches(r) && l != r) {
                setPendingChoice(pidx + 1, tileIdx);
            } else if (t.matches(l)) {
                playTile(pidx, tileIdx, "left");
            } else if (t.matches(r)) {
                playTile(pidx, tileIdx, "right");
            }
        }
    } else if (input.find("\"draw\"") != std::string::npos || input.find("\"cmd\":\"draw\"") != std::string::npos) {
        if (!m_isBlockMode && drawTile(pidx)) {
            m_lastAction = "You drew a tile";
        }
    } else if (input.find("\"pass\"") != std::string::npos || input.find("\"cmd\":\"pass\"") != std::string::npos) {
        passTurn(pidx);
        m_lastAction = "You passed turn";
        if (m_isSolo && !m_finished && m_turn != 1) {
            executeBotTurns();
        }
    } else if (input.find("\"mode\"") != std::string::npos || input.find("\"set_mode\"") != std::string::npos) {
        if (input.find("\"block\"") != std::string::npos) {
            m_isBlockMode = true;
            m_lastAction = "Mode set to Block Dominoes";
        } else if (input.find("\"draw\"") != std::string::npos) {
            m_isBlockMode = false;
            m_lastAction = "Mode set to Draw Dominoes";
        }
    }
}

std::string DominoGame::serializeState() const {
    return serializeStateForPlayer(0);
}

std::string DominoGame::serializeStateForPlayer(PlayerId id) const {
    std::ostringstream oss;
    oss << "{\"type\":\"state\",\"game\":\"domino\""
        << ",\"turn\":" << m_turn
        << ",\"num_players\":" << m_numPlayers
        << ",\"block_mode\":" << (m_isBlockMode ? "true" : "false")
        << ",\"finished\":" << (m_finished ? "true" : "false")
        << ",\"winner\":" << m_winner
        << ",\"left_end\":" << getLeftEnd()
        << ",\"right_end\":" << getRightEnd()
        << ",\"boneyard_count\":" << m_boneyard.size()
        << ",\"last_action\":\"" << m_lastAction << "\""
        << ",\"scores\":[" << m_hands[0].size() << "," << m_hands[1].size() << "," << m_hands[2].size() << "]"
        << ",\"p1_count\":" << m_hands[0].size()
        << ",\"p2_count\":" << m_hands[1].size()
        << ",\"p3_count\":" << m_hands[2].size();

    // Board line
    oss << ",\"board\":[";
    for (size_t i = 0; i < m_board.size(); ++i) {
        if (i > 0) oss << ",";
        oss << "[" << m_board[i].left << "," << m_board[i].right << "]";
    }
    oss << "]";

    // Hands: If match finished or id == 0, reveal all hands. Otherwise, ONLY reveal hand for player 'id'!
    oss << ",\"hands\":{";
    if (m_finished || id == 0) {
        for (int p = 0; p < m_numPlayers; ++p) {
            if (p > 0) oss << ",";
            oss << "\"" << (p + 1) << "\":[";
            for (size_t i = 0; i < m_hands[p].size(); ++i) {
                if (i > 0) oss << ",";
                oss << "[" << m_hands[p][i].left << "," << m_hands[p][i].right << "]";
            }
            oss << "]";
        }
    } else {
        int p = static_cast<int>(id) - 1;
        if (p >= 0 && p < m_numPlayers) {
            oss << "\"" << (p + 1) << "\":[";
            for (size_t i = 0; i < m_hands[p].size(); ++i) {
                if (i > 0) oss << ",";
                oss << "[" << m_hands[p][i].left << "," << m_hands[p][i].right << "]";
            }
            oss << "]";
        }
    }
    oss << "}";

    // Available Actions
    oss << ",\"available_actions\":[";
    bool isTurn = !m_finished && (id == 0 || static_cast<int>(id) == m_turn);
    int pidx = (id == 0 ? m_turn - 1 : static_cast<int>(id) - 1);
    bool canPlay = (pidx >= 0 && pidx < m_numPlayers) ? canPlayerPlay(pidx) : true;
    bool canDraw = isTurn && !m_isBlockMode && !m_boneyard.empty();
    bool mustDraw = canDraw && !canPlay;
    bool canPass = isTurn && !canPlay && (m_isBlockMode || m_boneyard.empty());

    oss << "{\"id\":\"draw\",\"label\":\"Draw Tile (" << m_boneyard.size() << ")\",\"enabled\":"
        << (canDraw ? "true" : "false")
        << (mustDraw ? ",\"style\":\"highlight\"" : "") << "},";

    oss << "{\"id\":\"pass\",\"label\":\"Pass Turn\",\"enabled\":"
        << (canPass ? "true" : "false") << "}";
    oss << "]";

    // Prompt Modal (if choice pending)
    if (m_pendingChoiceIdx >= 0 && (id == 0 || static_cast<int>(id) == m_pendingChoicePlayer)) {
        oss << ",\"prompt\":{\"title\":\"Place Domino\",\"choices\":["
            << "{\"cmd\":\"play\",\"label\":\"Place Left End\",\"params\":{\"index\":" << m_pendingChoiceIdx << ",\"end\":\"left\"}},"
            << "{\"cmd\":\"play\",\"label\":\"Place Right End\",\"params\":{\"index\":" << m_pendingChoiceIdx << ",\"end\":\"right\"}}"
            << "]}";
    }

    oss << "}";
    return oss.str();
}

} // namespace core
} // namespace gamehub
