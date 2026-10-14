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

// ============================================================================
// Command Pattern Implementations (Double Dispatch)
// ============================================================================

bool RollDiceCommand::execute(Game& game, PlayerId playerId) const {
    auto* ludo = dynamic_cast<LudoGame*>(&game);
    if (!ludo) return false;
    return ludo->processCommand(playerId, *this);
}

bool MovePawnCommand::execute(Game& game, PlayerId playerId) const {
    auto* ludo = dynamic_cast<LudoGame*>(&game);
    if (!ludo) return false;
    return ludo->processCommand(playerId, *this);
}

// ============================================================================
// State Pattern Implementations
// ============================================================================

void LudoWaitingRollState::onEnter(Game& game) {
    auto* ludo = dynamic_cast<LudoGame*>(&game);
    if (ludo) {
        ludo->resetTurnTimer();
        ludo->resetBotTimer();
    }
}

bool LudoWaitingRollState::handleCommand(Game& game, PlayerId playerId, const ICommand& command) {
    auto* ludo = dynamic_cast<LudoGame*>(&game);
    if (!ludo || ludo->isFinished()) return false;
    if (static_cast<int>(playerId) != ludo->getCurrentTurn()) return false;

    if (command.getType() == "roll") {
        const auto* rollCmd = dynamic_cast<const RollDiceCommand*>(&command);
        int forced = rollCmd ? rollCmd->getForcedValue() : 0;
        return ludo->executeRollDice(forced);
    }
    return false;
}

void LudoWaitingRollState::update(Game& game, float dt) {
    auto* ludo = dynamic_cast<LudoGame*>(&game);
    if (!ludo || ludo->isFinished()) return;

    ludo->tickTurnTimer(dt);
    if (ludo->getTurnTimer() <= 0.0f) {
        ludo->executeRollDice(0);
        return;
    }

    int curSeat = ludo->getCurrentTurn() - 1;
    auto* player = ludo->getPlayerEntity(curSeat);
    if (player && player->isAutonomous()) {
        ludo->tickBotTimer(dt);
        if (ludo->getBotTimer() <= 0.0f) {
            auto cmd = player->getController()->decideAction(*ludo, static_cast<PlayerId>(curSeat + 1));
            if (cmd) {
                cmd->execute(*ludo, static_cast<PlayerId>(curSeat + 1));
            }
        }
    }
}

void LudoSelectPawnState::onEnter(Game& game) {
    auto* ludo = dynamic_cast<LudoGame*>(&game);
    if (ludo) {
        ludo->resetTurnTimer();
        ludo->resetBotTimer();
    }
}

bool LudoSelectPawnState::handleCommand(Game& game, PlayerId playerId, const ICommand& command) {
    auto* ludo = dynamic_cast<LudoGame*>(&game);
    if (!ludo || ludo->isFinished()) return false;
    if (static_cast<int>(playerId) != ludo->getCurrentTurn()) return false;

    if (command.getType() == "move") {
        const auto* moveCmd = dynamic_cast<const MovePawnCommand*>(&command);
        if (!moveCmd) return false;
        return ludo->executeMovePawn(moveCmd->getPawnIndex());
    }
    return false;
}

void LudoSelectPawnState::update(Game& game, float dt) {
    auto* ludo = dynamic_cast<LudoGame*>(&game);
    if (!ludo || ludo->isFinished()) return;

    ludo->tickTurnTimer(dt);
    if (ludo->getTurnTimer() <= 0.0f) {
        int curSeat = ludo->getCurrentTurn() - 1;
        int bestPawn = ludo->pickBestBotPawn(curSeat, ludo->getCurrentDice());
        if (bestPawn != -1) {
            ludo->executeMovePawn(bestPawn);
        } else {
            ludo->setConsecutiveSixes(0);
            ludo->nextTurn();
        }
        return;
    }

    int curSeat = ludo->getCurrentTurn() - 1;
    auto* player = ludo->getPlayerEntity(curSeat);
    if (player && player->isAutonomous()) {
        ludo->tickBotTimer(dt);
        if (ludo->getBotTimer() <= 0.0f) {
            auto cmd = player->getController()->decideAction(*ludo, static_cast<PlayerId>(curSeat + 1));
            if (cmd) {
                cmd->execute(*ludo, static_cast<PlayerId>(curSeat + 1));
            } else {
                ludo->setConsecutiveSixes(0);
                ludo->nextTurn();
            }
        }
    }
}

void LudoGameOverState::onEnter(Game& /*game*/) {}

// ============================================================================
// Strategy Pattern Implementations (Autonomous Bot Heuristic)
// ============================================================================

std::unique_ptr<ICommand> LudoBotController::decideAction(const Game& game, PlayerId id) {
    const auto* ludo = dynamic_cast<const LudoGame*>(&game);
    if (!ludo || ludo->isFinished()) return nullptr;

    if (static_cast<int>(id) != ludo->getCurrentTurn()) {
        return nullptr;
    }

    if (!ludo->isDiceRolled()) {
        return std::make_unique<RollDiceCommand>();
    }

    int curSeat = static_cast<int>(id) - 1;
    int dice = ludo->getCurrentDice();
    int bestPawn = ludo->pickBestBotPawn(curSeat, dice);
    if (bestPawn != -1) {
        return std::make_unique<MovePawnCommand>(bestPawn);
    }

    return nullptr;
}

// ============================================================================
// LudoGame Implementation
// ============================================================================

bool LudoGame::isSafeSquare(int trackCell) {
    return LudoBoard::isSafeSquare(trackCell);
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

void LudoGame::transitionTo(std::unique_ptr<IGamePhaseState> nextState) {
    if (!nextState) return;
    m_state = std::move(nextState);
    m_state->onEnter(*this);
}

bool LudoGame::processCommand(PlayerId player, const ICommand& cmd) {
    if (m_state) {
        return m_state->handleCommand(*this, player, cmd);
    }
    return false;
}

void LudoGame::init() {
    m_finished = false;
    m_winner = 0;
    m_currentTurn = 1;
    m_dice.reset();
    m_consecutiveSixes = 0;
    m_turnTimer = 20.0f;
    m_botTimer = m_botDelay;
    m_lastAction = "Match started";

    static const char* colors[4] = {"red", "green", "yellow", "blue"};
    static const int startTiles[4] = {0, 13, 26, 39};

    m_playerEntities.clear();
    for (int i = 0; i < 4; ++i) {
        m_players[i].id = i + 1;
        m_players[i].color = colors[i];
        m_players[i].startTile = startTiles[i];
        m_players[i].finishedCount = 0;

        std::unique_ptr<PlayerController> controller;
        if (m_isSolo) {
            m_players[i].isBot = (i > 0);
            m_players[i].name = (i == 0) ? "Player 1" : ("Bot " + std::to_string(i));
            if (i == 0) {
                controller = std::make_unique<HumanController>();
            } else {
                controller = std::make_unique<LudoBotController>();
            }
        } else {
            m_players[i].isBot = false;
            m_players[i].name = "Player " + std::to_string(i + 1);
            controller = std::make_unique<HumanController>();
        }

        m_playerEntities.push_back(
            std::make_unique<Player>(static_cast<PlayerId>(i + 1), m_players[i].name, std::move(controller))
        );

        for (int p = 0; p < 4; ++p) {
            m_players[i].pawns[p].id = p;
            m_players[i].pawns[p].step = -1;
            m_players[i].pawns[p].state = PawnState::IN_YARD;
        }
    }

    transitionTo(std::make_unique<LudoWaitingRollState>());
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

void LudoGame::setDiceForTesting(int dice) {
    m_dice.force(dice);
    transitionTo(std::make_unique<LudoSelectPawnState>());
}

void LudoGame::setCurrentTurnForTesting(int seatId) {
    m_currentTurn = seatId;
    m_turnTimer = 20.0f;
    m_botTimer = m_botDelay;
}

bool LudoGame::canPawnMove(int playerIdx, int pawnIdx, int dice) const {
    if (playerIdx < 0 || playerIdx >= m_numPlayers) return false;
    if (pawnIdx < 0 || pawnIdx >= m_pawnCount) return false;
    const auto& p = m_players[playerIdx].pawns[pawnIdx];
    return p.canMove(dice);
}

int LudoGame::getLegalMovesCount(int playerIdx, int dice) const {
    int cnt = 0;
    for (int p = 0; p < m_pawnCount; ++p) {
        if (canPawnMove(playerIdx, p, dice)) cnt++;
    }
    return cnt;
}

void LudoGame::rollDiceForTesting(int forcedRoll) {
    m_dice.reset();
    executeRollDice(forcedRoll);
}

bool LudoGame::executeRollDice(int forcedRoll) {
    if (m_dice.isRolled() || m_finished) return false;

    int roll = forcedRoll;
    if (roll >= 1 && roll <= 6) {
        m_dice.force(roll);
    } else {
        roll = m_dice.roll();
    }

    m_turnTimer = 20.0f;
    m_botTimer = m_botDelay;

    int curSeat = m_currentTurn - 1;
    m_lastAction = m_players[curSeat].name + " rolled a " + std::to_string(roll);

    if (roll == 6) {
        m_consecutiveSixes++;
        if (m_consecutiveSixes == 3) {
            m_lastAction = m_players[curSeat].name + " rolled three 6s in a row (Turn forfeited)";
            m_consecutiveSixes = 0;
            m_dice.reset();
            nextTurn();
            return true;
        }
    }

    int legal = getLegalMovesCount(curSeat, roll);
    if (legal == 0) {
        if (roll == 6) {
            // Still entitled to bonus roll even if couldn't move
            m_dice.reset();
            m_turnTimer = 20.0f;
            m_botTimer = m_botDelay;
            // Remains in LudoWaitingRollState
        } else {
            m_consecutiveSixes = 0;
            m_dice.reset();
            nextTurn();
        }
    } else {
        transitionTo(std::make_unique<LudoSelectPawnState>());
    }

    return true;
}

bool LudoGame::executeMovePawn(int pawnIdx) {
    int curSeat = m_currentTurn - 1;
    if (curSeat < 0 || curSeat >= m_numPlayers) return false;
    if (pawnIdx < 0 || pawnIdx >= m_pawnCount) return false;
    if (!m_dice.isRolled()) return false;
    if (!canPawnMove(curSeat, pawnIdx, m_dice.getValue())) return false;

    auto& p = m_players[curSeat].pawns[pawnIdx];
    bool bonusRoll = false;

    if (p.state == PawnState::IN_YARD) {
        p.rollout();
        m_lastAction = m_players[curSeat].name + " moved pawn #" + std::to_string(pawnIdx + 1) + " out of Yard";
    } else {
        p.advance(m_dice.getValue());
        if (p.state == PawnState::FINISHED) {
            m_players[curSeat].finishedCount++;
            m_lastAction = m_players[curSeat].name + " moved pawn #" + std::to_string(pawnIdx + 1) + " HOME!";
            bonusRoll = true;

            if (m_players[curSeat].finishedCount == m_pawnCount) {
                m_finished = true;
                m_winner = m_players[curSeat].id;
                m_lastAction = m_players[curSeat].name + " won the match!";
                transitionTo(std::make_unique<LudoGameOverState>());
                return true;
            }
        } else if (p.state == PawnState::IN_HOME) {
            m_lastAction = m_players[curSeat].name + " advanced pawn #" + std::to_string(pawnIdx + 1) + " into Home Stretch";
        } else {
            int trackCell = p.getTrackCell(m_players[curSeat].startTile);
            bool isSafe = isSafeSquare(trackCell);

            if (!isSafe) {
                bool captured = false;
                for (int opp = 0; opp < m_numPlayers; ++opp) {
                    if (opp == curSeat) continue;
                    for (int opPawn = 0; opPawn < m_pawnCount; ++opPawn) {
                        auto& op = m_players[opp].pawns[opPawn];
                        if (op.state == PawnState::ON_TRACK &&
                            op.getTrackCell(m_players[opp].startTile) == trackCell) {
                            op.returnToYard();
                            captured = true;
                            m_lastAction = m_players[curSeat].name + " captured " + m_players[opp].name + "'s pawn!";
                        }
                    }
                }
                if (captured) {
                    bonusRoll = true;
                }
            }
        }
    }

    if (m_dice.getValue() == 6 || bonusRoll) {
        m_dice.reset();
        m_turnTimer = 20.0f;
        m_botTimer = m_botDelay;
        transitionTo(std::make_unique<LudoWaitingRollState>());
    } else {
        m_consecutiveSixes = 0;
        m_dice.reset();
        nextTurn();
    }

    return true;
}

void LudoGame::nextTurn() {
    int curSeat = m_currentTurn - 1;
    m_currentTurn = ((curSeat + 1) % m_numPlayers) + 1;
    m_dice.reset();
    m_consecutiveSixes = 0;
    m_turnTimer = 20.0f;
    m_botTimer = m_botDelay;
    transitionTo(std::make_unique<LudoWaitingRollState>());
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
    if (m_state) {
        m_state->update(*this, dt);
    }
}

void LudoGame::handleInput(PlayerId player, const std::string& input) {
    if (m_finished) return;
    auto cmd = m_commandFactory.createCommand(input);
    if (cmd) {
        cmd->execute(*this, player);
    }
}

std::string LudoGame::serializeState() const {
    return serializeStateForPlayer(1);
}

std::string LudoGame::serializeStateForPlayer(PlayerId /*id*/) const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"type\":\"state\",";
    oss << "\"game\":\"ludo\",";
    oss << "\"is_quick\":" << (m_isQuick ? "true" : "false") << ",";
    oss << "\"pawn_count\":" << m_pawnCount << ",";
    oss << "\"current_turn\":" << m_currentTurn << ",";
    oss << "\"dice\":" << m_dice.getValue() << ",";
    oss << "\"dice_rolled\":" << (m_dice.isRolled() ? "true" : "false") << ",";
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
        bool isBot = (i < static_cast<int>(m_playerEntities.size()))
            ? m_playerEntities[i]->isAutonomous()
            : pl.isBot;
        oss << "{\"id\":" << pl.id
            << ",\"color\":\"" << pl.color << "\""
            << ",\"name\":\"" << pl.name << "\""
            << ",\"is_bot\":" << (isBot ? "true" : "false")
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
    if (m_dice.isRolled() && !m_finished) {
        int curSeat = m_currentTurn - 1;
        bool first = true;
        for (int p = 0; p < m_pawnCount; ++p) {
            if (canPawnMove(curSeat, p, m_dice.getValue())) {
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
