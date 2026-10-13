#include "LudoGame.hpp"
#include <gamehub/core/GameRegistry.hpp>
#include <algorithm>
#include <random>
#include <sstream>

namespace {
std::unique_ptr<gamehub::core::Game> createLudoSolo() {
    return std::make_unique<gamehub::core::LudoGame>(true, 4, true);
}
std::unique_ptr<gamehub::core::Game> createLudoQuick() {
    return std::make_unique<gamehub::core::LudoGame>(false, 4, true);
}
std::unique_ptr<gamehub::core::Game> createLudoClassic() {
    return std::make_unique<gamehub::core::LudoGame>(false, 4, false);
}
}

REGISTER_GAME("ludo_solo", LudoSolo, &createLudoSolo, {
    gamehub::core::GameId("ludo_solo"),
    "Ludo Solo",
    "SOLO",
    "Quick Ludo match (2 pawns) vs 3 Tactical Bots",
    1, 1,
    gamehub::core::InputFlag::ACTION_BUTTONS,
    "ludo_solo",
    false
})

REGISTER_GAME("ludo_quick", LudoQuick, &createLudoQuick, {
    gamehub::core::GameId("ludo_quick"),
    "Ludo Quick Match",
    "PARTY",
    "2-4P Quick Ludo (2 pawns) with Star SafeSquares & Captures",
    2, 4,
    gamehub::core::InputFlag::ACTION_BUTTONS,
    "ludo_quick",
    false
})

REGISTER_GAME("ludo_classic", LudoClassic, &createLudoClassic, {
    gamehub::core::GameId("ludo_classic"),
    "Ludo Classic",
    "PARTY",
    "2-4P Classic Ludo (4 pawns) full board championship",
    2, 4,
    gamehub::core::InputFlag::ACTION_BUTTONS,
    "ludo_classic",
    false
})

namespace gamehub {
namespace core {

bool LudoGame::isSafeSquare(int trackCell) {
    static constexpr int SAFE_SQUARES[8] = {0, 8, 13, 21, 26, 34, 39, 47};
    for (int s : SAFE_SQUARES) {
        if (s == trackCell) return true;
    }
    return false;
}

LudoGame::LudoGame(bool isSolo, int numPlayers, bool isQuick)
    : m_isSolo(isSolo),
      m_numPlayers(std::clamp(numPlayers, 2, 4)),
      m_isQuick(isQuick),
      m_pawnCount(isQuick ? 2 : 4) {
    init();
}

void LudoGame::setPlayerCount(int count) {
    int clamped = std::clamp(count, 2, 4);
    if (clamped == m_numPlayers) return;
    m_numPlayers = clamped;
    init();
}

void LudoGame::setQuickMode(bool q) {
    m_isQuick = q;
    m_pawnCount = q ? 2 : 4;
    init();
}

void LudoGame::init() {
    m_finished = false;
    m_winner = 0;
    m_currentTurn = 1;
    m_currentDice = 0;
    m_diceRolled = false;
    m_consecutiveSixes = 0;
    m_turnTimer = 20.0f;
    m_botTimer = m_botDelay;
    m_lastAction = "Match started";

    static const char* colors[4] = {"red", "green", "yellow", "blue"};
    static const int startTiles[4] = {0, 13, 26, 39};

    for (int i = 0; i < 4; ++i) {
        m_players[i].id = i + 1;
        m_players[i].color = colors[i];
        m_players[i].startTile = startTiles[i];
        m_players[i].finishedCount = 0;

        if (m_isSolo) {
            m_players[i].isBot = (i > 0);
            m_players[i].name = (i == 0) ? "Player 1" : ("Bot " + std::to_string(i));
        } else {
            m_players[i].isBot = false;
            m_players[i].name = "Player " + std::to_string(i + 1);
        }

        for (int p = 0; p < 4; ++p) {
            m_players[i].pawns[p].id = p;
            m_players[i].pawns[p].step = -1;
            m_players[i].pawns[p].state = PawnState::IN_YARD;
        }
    }
}

void LudoGame::setPawnStepForTesting(int playerIdx, int pawnIdx, int step) {
    if (playerIdx < 0 || playerIdx >= 4 || pawnIdx < 0 || pawnIdx >= 4) return;
    auto& p = m_players[playerIdx].pawns[pawnIdx];
    p.step = step;
    if (step < 0) {
        p.state = PawnState::IN_YARD;
    } else if (step <= 50) {
        p.state = PawnState::ON_TRACK;
    } else if (step < 56) {
        p.state = PawnState::IN_HOME;
    } else {
        p.state = PawnState::FINISHED;
        m_players[playerIdx].finishedCount++;
    }
}

bool LudoGame::canPawnMove(int playerIdx, int pawnIdx, int dice) const {
    if (playerIdx < 0 || playerIdx >= m_numPlayers) return false;
    if (pawnIdx < 0 || pawnIdx >= m_pawnCount) return false;
    const auto& p = m_players[playerIdx].pawns[pawnIdx];
    if (p.state == PawnState::FINISHED) return false;

    if (p.state == PawnState::IN_YARD) {
        return dice == 6;
    }
    return (p.step + dice) <= 56;
}

int LudoGame::getLegalMovesCount(int playerIdx, int dice) const {
    int cnt = 0;
    for (int p = 0; p < m_pawnCount; ++p) {
        if (canPawnMove(playerIdx, p, dice)) cnt++;
    }
    return cnt;
}

void LudoGame::rollDiceForTesting(int forcedRoll) {
    m_diceRolled = false;
    rollDice(forcedRoll);
}

void LudoGame::rollDice(int forcedRoll) {
    if (m_diceRolled || m_finished) return;

    int roll = forcedRoll;
    if (roll < 1 || roll > 6) {
        std::random_device rd;
        std::mt19937 g(rd());
        std::uniform_int_distribution<int> dist(1, 6);
        roll = dist(g);
    }

    m_currentDice = roll;
    m_diceRolled = true;
    m_turnTimer = 20.0f;
    m_botTimer = m_botDelay;

    int curSeat = m_currentTurn - 1;
    m_lastAction = m_players[curSeat].name + " rolled a " + std::to_string(roll);

    if (roll == 6) {
        m_consecutiveSixes++;
        if (m_consecutiveSixes == 3) {
            m_lastAction = m_players[curSeat].name + " rolled three 6s in a row (Turn forfeited)";
            m_consecutiveSixes = 0;
            m_diceRolled = false;
            nextTurn();
            return;
        }
    }

    int legal = getLegalMovesCount(curSeat, roll);
    if (legal == 0) {
        if (roll == 6) {
            // Still entitled to bonus roll even if couldn't move
            m_diceRolled = false;
            m_turnTimer = 20.0f;
            m_botTimer = m_botDelay;
        } else {
            m_consecutiveSixes = 0;
            m_diceRolled = false;
            nextTurn();
        }
    }
}

bool LudoGame::executeMove(int playerIdx, int pawnIdx) {
    if (playerIdx < 0 || playerIdx >= m_numPlayers) return false;
    if (pawnIdx < 0 || pawnIdx >= m_pawnCount) return false;
    if (!m_diceRolled) return false;
    if (!canPawnMove(playerIdx, pawnIdx, m_currentDice)) return false;

    auto& p = m_players[playerIdx].pawns[pawnIdx];
    bool bonusRoll = false;

    if (p.state == PawnState::IN_YARD) {
        p.step = 0;
        p.state = PawnState::ON_TRACK;
        m_lastAction = m_players[playerIdx].name + " moved pawn #" + std::to_string(pawnIdx + 1) + " out of Yard";
    } else {
        p.step += m_currentDice;
        if (p.step == 56) {
            p.state = PawnState::FINISHED;
            m_players[playerIdx].finishedCount++;
            m_lastAction = m_players[playerIdx].name + " moved pawn #" + std::to_string(pawnIdx + 1) + " HOME!";
            bonusRoll = true;

            if (m_players[playerIdx].finishedCount == m_pawnCount) {
                m_finished = true;
                m_winner = m_players[playerIdx].id;
                m_lastAction = m_players[playerIdx].name + " won the match!";
                return true;
            }
        } else if (p.step >= 51) {
            p.state = PawnState::IN_HOME;
            m_lastAction = m_players[playerIdx].name + " advanced pawn #" + std::to_string(pawnIdx + 1) + " into Home Stretch";
        } else {
            p.state = PawnState::ON_TRACK;
            int trackCell = (m_players[playerIdx].startTile + p.step) % 52;
            bool isSafe = isSafeSquare(trackCell);

            if (!isSafe) {
                bool captured = false;
                for (int opp = 0; opp < m_numPlayers; ++opp) {
                    if (opp == playerIdx) continue;
                    for (int opPawn = 0; opPawn < m_pawnCount; ++opPawn) {
                        auto& op = m_players[opp].pawns[opPawn];
                        if (op.state == PawnState::ON_TRACK &&
                            op.getTrackCell(m_players[opp].startTile) == trackCell) {
                            op.state = PawnState::IN_YARD;
                            op.step = -1;
                            captured = true;
                            m_lastAction = m_players[playerIdx].name + " captured " + m_players[opp].name + "'s pawn!";
                        }
                    }
                }
                if (captured) {
                    bonusRoll = true;
                }
            }
        }
    }

    if (m_currentDice == 6 || bonusRoll) {
        m_diceRolled = false;
        m_turnTimer = 20.0f;
        m_botTimer = m_botDelay;
    } else {
        m_consecutiveSixes = 0;
        m_diceRolled = false;
        nextTurn();
    }

    return true;
}

void LudoGame::nextTurn() {
    int curSeat = m_currentTurn - 1;
    m_currentTurn = ((curSeat + 1) % m_numPlayers) + 1;
    m_currentDice = 0;
    m_diceRolled = false;
    m_consecutiveSixes = 0;
    m_turnTimer = 20.0f;
    m_botTimer = m_botDelay;
}

int LudoGame::pickBestBotPawn(int playerIdx, int dice) const {
    int bestPawn = -1;
    int bestScore = -9999;

    for (int pIdx = 0; pIdx < m_pawnCount; ++pIdx) {
        if (!canPawnMove(playerIdx, pIdx, dice)) continue;
        const auto& p = m_players[playerIdx].pawns[pIdx];

        int score = 0;
        if (p.state == PawnState::IN_YARD) {
            score = 350; // High priority to get pawns out
        } else if (p.step + dice == 56) {
            score = 800; // Finish pawn
        } else if (p.step + dice >= 51) {
            score = 400 + (p.step + dice); // Enter home stretch
        } else {
            int targetCell = (m_players[playerIdx].startTile + p.step + dice) % 52;
            bool canCapture = false;
            if (!isSafeSquare(targetCell)) {
                for (int opp = 0; opp < m_numPlayers; ++opp) {
                    if (opp == playerIdx) continue;
                    for (int opPawn = 0; opPawn < m_pawnCount; ++opPawn) {
                        const auto& op = m_players[opp].pawns[opPawn];
                        if (op.state == PawnState::ON_TRACK &&
                            op.getTrackCell(m_players[opp].startTile) == targetCell) {
                            canCapture = true;
                            break;
                        }
                    }
                }
            }
            if (canCapture) {
                score = 1000; // Capturing opponent is highest tactical priority
            } else if (isSafeSquare(targetCell)) {
                score = 250; // Land on safe star
            } else {
                score = 100 + (p.step + dice); // Progress furthest pawn
            }
        }

        if (score > bestScore) {
            bestScore = score;
            bestPawn = pIdx;
        }
    }

    return bestPawn;
}

void LudoGame::update(float dt) {
    if (m_finished) return;

    m_turnTimer -= dt;
    if (m_turnTimer <= 0.0f) {
        int curSeat = m_currentTurn - 1;
        if (!m_diceRolled) {
            rollDice();
        } else {
            int bestPawn = pickBestBotPawn(curSeat, m_currentDice);
            if (bestPawn != -1) {
                executeMove(curSeat, bestPawn);
            } else {
                m_consecutiveSixes = 0;
                m_diceRolled = false;
                nextTurn();
            }
        }
        return;
    }

    int curSeat = m_currentTurn - 1;
    if (m_players[curSeat].isBot) {
        m_botTimer -= dt;
        if (m_botTimer <= 0.0f) {
            if (!m_diceRolled) {
                rollDice();
            } else {
                int bestPawn = pickBestBotPawn(curSeat, m_currentDice);
                if (bestPawn != -1) {
                    executeMove(curSeat, bestPawn);
                } else {
                    m_consecutiveSixes = 0;
                    m_diceRolled = false;
                    nextTurn();
                }
            }
        }
    }
}

void LudoGame::handleInput(PlayerId player, const std::string& input) {
    if (m_finished) return;
    int seatIdx = static_cast<int>(player) - 1;
    if (seatIdx < 0 || seatIdx >= m_numPlayers) return;
    if (m_currentTurn != static_cast<int>(player)) return;

    if (input.find("\"cmd\":\"roll\"") != std::string::npos ||
        input.find("\"action\":\"roll\"") != std::string::npos) {
        if (!m_diceRolled) {
            rollDice();
        }
        return;
    }

    if (input.find("\"cmd\":\"move\"") != std::string::npos ||
        input.find("\"action\":\"move\"") != std::string::npos) {
        int pawnIdx = 0;
        auto pos = input.find("\"pawn\":");
        if (pos != std::string::npos) {
            try {
                pawnIdx = std::stoi(input.substr(pos + 7));
            } catch (...) {
                pawnIdx = 0;
            }
        }
        executeMove(seatIdx, pawnIdx);
    }
}

std::string LudoGame::serializeState() const {
    return serializeStateForPlayer(1);
}

std::string LudoGame::serializeStateForPlayer(PlayerId /*id*/) const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"game\":\"ludo\",";
    oss << "\"is_quick\":" << (m_isQuick ? "true" : "false") << ",";
    oss << "\"pawn_count\":" << m_pawnCount << ",";
    oss << "\"current_turn\":" << m_currentTurn << ",";
    oss << "\"dice\":" << m_currentDice << ",";
    oss << "\"dice_rolled\":" << (m_diceRolled ? "true" : "false") << ",";
    oss << "\"consecutive_sixes\":" << m_consecutiveSixes << ",";
    oss << "\"turn_time_left\":" << std::max(0.0f, m_turnTimer) << ",";
    oss << "\"finished\":" << (m_finished ? "true" : "false") << ",";
    oss << "\"winner\":" << m_winner << ",";
    oss << "\"last_action\":\"" << m_lastAction << "\",";

    // Players
    oss << "\"players\":[";
    for (int i = 0; i < m_numPlayers; ++i) {
        if (i > 0) oss << ",";
        const auto& pl = m_players[i];
        oss << "{\"id\":" << pl.id
            << ",\"color\":\"" << pl.color << "\""
            << ",\"name\":\"" << pl.name << "\""
            << ",\"is_bot\":" << (pl.isBot ? "true" : "false")
            << ",\"start_tile\":" << pl.startTile
            << ",\"finished_count\":" << pl.finishedCount
            << ",\"pawns\":[";

        for (int p = 0; p < m_pawnCount; ++p) {
            if (p > 0) oss << ",";
            const auto& pw = pl.pawns[p];
            static const char* stateNames[] = {"IN_YARD", "ON_TRACK", "IN_HOME", "FINISHED"};
            oss << "{\"id\":" << pw.id
                << ",\"state\":\"" << stateNames[static_cast<int>(pw.state)] << "\""
                << ",\"step\":" << pw.step
                << ",\"track_cell\":" << pw.getTrackCell(pl.startTile) << "}";
        }
        oss << "]}";
    }
    oss << "],";

    // Legal moves for currently active player
    oss << "\"legal_moves\":[";
    if (m_diceRolled && !m_finished) {
        int curSeat = m_currentTurn - 1;
        bool first = true;
        for (int p = 0; p < m_pawnCount; ++p) {
            if (canPawnMove(curSeat, p, m_currentDice)) {
                if (!first) oss << ",";
                oss << p;
                first = false;
            }
        }
    }
    oss << "]";

    oss << "}";
    return oss.str();
}

} // namespace core
} // namespace gamehub
