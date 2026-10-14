#include "KatroGame.hpp"
#include <gamehub/core/GameRegistry.hpp>
#include <sstream>
#include <algorithm>

namespace {
std::unique_ptr<gamehub::core::Game> createKatroSolo() {
    return std::make_unique<gamehub::core::KatroGame>(true);
}
std::unique_ptr<gamehub::core::Game> createKatroPvp() {
    return std::make_unique<gamehub::core::KatroGame>(false);
}
}

REGISTER_GAME("katro_solo", KatroSolo, &createKatroSolo, {
    gamehub::core::GameId("katro_solo"),
    "Katro Solo",
    "SOLO",
    "Jeu de semailles malgache traditionnel vs Bot",
    1, 1,
    gamehub::core::InputFlag::GRID_POINTER | gamehub::core::InputFlag::ACTION_BUTTONS,
    "katro_solo",
    false
})

REGISTER_GAME("katro_pvp", KatroPvp, &createKatroPvp, {
    gamehub::core::GameId("katro_pvp"),
    "Katro Duel",
    "DUEL",
    "Duel tactique de semailles malgache 4x8",
    2, 2,
    gamehub::core::InputFlag::GRID_POINTER | gamehub::core::InputFlag::ACTION_BUTTONS,
    "katro_pvp",
    true
})

REGISTER_GAME("katro_duel", KatroDuelAlias, &createKatroPvp, {
    gamehub::core::GameId("katro_duel"),
    "Katro Duel",
    "DUEL",
    "Duel tactique de semailles malgache 4x8",
    2, 2,
    gamehub::core::InputFlag::GRID_POINTER | gamehub::core::InputFlag::ACTION_BUTTONS,
    "katro_pvp",
    true
})

namespace gamehub {
namespace core {

// ============================================================================
// Command Pattern Implementations (Double Dispatch)
// ============================================================================

bool SowPitCommand::execute(Game& game, PlayerId playerId) const {
    auto* katro = dynamic_cast<KatroGame*>(&game);
    if (!katro) return false;
    return katro->processCommand(playerId, *this);
}

// ============================================================================
// State Pattern Implementations
// ============================================================================

void KatroTurnState::onEnter(Game& game) {
    auto* katro = dynamic_cast<KatroGame*>(&game);
    if (katro) {
        katro->resetBotTimer();
    }
}

bool KatroTurnState::handleCommand(Game& game, PlayerId playerId, const ICommand& command) {
    auto* katro = dynamic_cast<KatroGame*>(&game);
    if (!katro || katro->isFinished()) return false;
    if (static_cast<int>(playerId) != katro->getTurn()) return false;

    if (command.getType() == "sow") {
        const auto* sowCmd = dynamic_cast<const SowPitCommand*>(&command);
        if (!sowCmd) return false;

        KatroDirection dir = sowCmd->hasDirection() ? sowCmd->getDirection() : katro->getDefaultDirection();
        return katro->playTurn(sowCmd->getCell(), static_cast<int>(playerId), dir);
    }

    return false;
}

void KatroTurnState::update(Game& game, float dt) {
    auto* katro = dynamic_cast<KatroGame*>(&game);
    if (!katro || katro->isFinished()) return;

    int curTurn = katro->getTurn();
    auto* player = katro->getPlayer(curTurn);
    if (player && player->isAutonomous()) {
        katro->tickBotTimer(dt);
        if (katro->getBotTimer() >= KatroGame::BOT_DELAY_SEC) {
            katro->resetBotTimer();
            auto cmd = player->getController()->decideAction(*katro, static_cast<PlayerId>(curTurn));
            if (cmd) {
                cmd->execute(*katro, static_cast<PlayerId>(curTurn));
            }
        }
    }
}

void KatroGameOverState::onEnter(Game& /*game*/) {}

// ============================================================================
// Strategy Pattern Implementations (Autonomous Bot Heuristic)
// ============================================================================

std::unique_ptr<ICommand> KatroBotController::decideAction(const Game& game, PlayerId id) {
    const auto* katro = dynamic_cast<const KatroGame*>(&game);
    if (!katro || katro->isFinished()) return nullptr;
    if (static_cast<int>(id) != katro->getTurn()) return nullptr;

    int bestCell = katro->computeBotMove();
    if (bestCell >= 0) {
        return std::make_unique<SowPitCommand>(bestCell, katro->getDefaultDirection(), true);
    }

    return nullptr;
}

// ============================================================================
// KatroGame Implementation
// ============================================================================

KatroGame::KatroGame(bool isSolo, int cols, int seedsPerCell)
    : m_isSolo(isSolo),
      m_board(KATRO_DEFAULT_ROWS, cols > 0 ? cols : KATRO_DEFAULT_COLS, seedsPerCell > 0 ? seedsPerCell : KATRO_DEFAULT_SEEDS) {
    init();
}

void KatroGame::transitionTo(std::unique_ptr<IGamePhaseState> nextState) {
    if (!nextState) return;
    m_state = std::move(nextState);
    m_state->onEnter(*this);
}

bool KatroGame::processCommand(PlayerId player, const ICommand& cmd) {
    if (m_state) {
        return m_state->handleCommand(*this, player, cmd);
    }
    return false;
}

void KatroGame::init() {
    m_board.reset();
    m_turn     = 1;
    m_finished = false;
    m_winner   = 0;
    m_botTimer = 0.0f;
    m_lastEvents.clear();
    m_moveSeq  = 0;

    m_players.clear();
    m_players.push_back(
        std::make_unique<Player>(1, "Player 1 (South)", std::make_unique<HumanController>())
    );

    if (m_isSolo) {
        m_players.push_back(
            std::make_unique<Player>(2, "Bot (North)", std::make_unique<KatroBotController>())
        );
    } else {
        m_players.push_back(
            std::make_unique<Player>(2, "Player 2 (North)", std::make_unique<HumanController>())
        );
    }

    transitionTo(std::make_unique<KatroTurnState>());
}

void KatroGame::update(float dt) {
    if (m_finished) return;
    if (m_state) {
        m_state->update(*this, dt);
    }
}

void KatroGame::handleInput(PlayerId id, const std::string& commandJson) {
    if (m_finished) return;
    auto cmd = m_commandFactory.createCommand(commandJson);
    if (cmd) {
        cmd->execute(*this, id);
    }
}

std::string KatroGame::serializeState() const {
    int southSeeds = countPlayerSeeds(KatroPlayer::SOUTH);
    int northSeeds = countPlayerSeeds(KatroPlayer::NORTH);

    std::ostringstream oss;
    oss << "{\"type\":\"state\",\"game\":\"katro\""
        << ",\"turn\":"     << m_turn
        << ",\"finished\":" << (m_finished ? "true" : "false")
        << ",\"winner\":"   << m_winner
        << ",\"seq\":"      << m_moveSeq
        << ",\"scores\":["  << southSeeds << "," << northSeeds << "]"
        << ",\"board\":[";

    int totalCells = m_board.getTotalCells();
    for (int i = 0; i < totalCells; ++i) {
        if (i > 0) oss << ",";
        oss << m_board.getSeedCount(i);
    }
    oss << "],\"events\":[";
    for (size_t i = 0; i < m_lastEvents.size(); ++i) {
        if (i > 0) oss << ",";
        const auto& ev = m_lastEvents[i];
        oss << "{\"t\":\"" << ev.type << "\""
            << ",\"from\":" << ev.from
            << ",\"to\":"   << ev.to
            << ",\"count\":" << ev.count << "}";
    }
    oss << "]}";
    return oss.str();
}

bool KatroGame::playTurn(int cell, int playerNum) {
    return playTurn(cell, playerNum, m_defaultDirection);
}

bool KatroGame::playTurn(int cell, int playerNum, KatroDirection dir) {
    if (cell < 0 || cell >= m_board.getTotalCells()) return false;
    KatroPlayer player = (playerNum == 1) ? KatroPlayer::SOUTH : KatroPlayer::NORTH;

    if (!m_board.isPlayerCell(cell, player)) return false;
    if (m_board.getSeedCount(cell) <= 0)     return false;
    if (m_turn != playerNum)                return false;
    if (m_finished)                         return false;

    m_lastEvents.clear();
    m_moveSeq++;

    // Prise des graines de la case de départ
    int seedsInHand = m_board.getPit(cell).takeAllSeeds();
    m_lastEvents.push_back({"pickup", cell, -1, seedsInHand});

    int current = cell;
    int safetyCounter = 0; // Protection anti-boucle infinie (au-delà de 2000 relais)

    while (true) {
        if (++safetyCounter > 2000) break;

        // Semis élémentaire : dépose une graine par case
        while (seedsInHand > 0) {
            current = m_board.nextCell(current, player, dir);
            m_board.getPit(current).addSeeds(1);
            seedsInHand--;
            m_lastEvents.push_back({"drop", -1, current, 1});
        }

        // État de la case d'arrivée AVANT le dépôt de la dernière graine
        bool hadSeedsBefore = (m_board.getSeedCount(current) > 1);
        bool captured = false;

        // Règle de capture : uniquement sur la rangée intérieure du joueur
        if (m_board.isInnerRow(current, player)) {
            if (!m_board.isOpponentInnerRowEmpty(player)) {
                // Cas B : la rangée intérieure adverse a au moins une graine
                int oppInner = m_board.getOppositeInner(current, player);
                if (oppInner >= 0 && m_board.getSeedCount(oppInner) > 0) {
                    int taken = m_board.getPit(oppInner).takeAllSeeds();

                    seedsInHand = m_board.getPit(current).takeAllSeeds() + taken;
                    captured = true;

                    m_lastEvents.push_back({"capture", oppInner, current, taken});
                    m_lastEvents.push_back({"pickup", current, -1, seedsInHand});
                }
            } else {
                // Cas C : toute la rangée intérieure adverse est vide
                int oppOuter = m_board.getOppositeOuter(current, player);
                if (oppOuter >= 0 && m_board.getSeedCount(oppOuter) > 0) {
                    int taken = m_board.getPit(oppOuter).takeAllSeeds();

                    seedsInHand = m_board.getPit(current).takeAllSeeds() + taken;
                    captured = true;

                    m_lastEvents.push_back({"capture", oppOuter, current, taken});
                    m_lastEvents.push_back({"pickup", current, -1, seedsInHand});
                }
            }
        }

        if (captured) {
            continue;
        }

        if (hadSeedsBefore) {
            seedsInHand = m_board.getPit(current).takeAllSeeds();
            m_lastEvents.push_back({"pickup", current, -1, seedsInHand});
            continue;
        } else {
            break;
        }
    }

    checkEndCondition();

    if (!m_finished) {
        m_turn = (playerNum == 1) ? 2 : 1;
        m_botTimer = 0.0f;
    } else {
        transitionTo(std::make_unique<KatroGameOverState>());
    }

    return true;
}

void KatroGame::checkEndCondition() {
    int southSeeds = m_board.countPlayerSeeds(KatroPlayer::SOUTH);
    int northSeeds = m_board.countPlayerSeeds(KatroPlayer::NORTH);
    bool southCanMove = m_board.canPlayerMove(KatroPlayer::SOUTH);
    bool northCanMove = m_board.canPlayerMove(KatroPlayer::NORTH);

    if (southSeeds <= m_earlyStopThreshold || !southCanMove) {
        m_finished = true;
        m_winner   = 2;  // Nord gagne
    } else if (northSeeds <= m_earlyStopThreshold || !northCanMove) {
        m_finished = true;
        m_winner   = 1;  // Sud gagne
    }
}

int KatroGame::computeBotMove() const {
    int bestCell  = -1;
    int bestGain  = -9999;
    int startNord = 2 * m_board.getCols();
    int endNord   = m_board.getTotalCells();

    for (int cell = startNord; cell < endNord; ++cell) {
        if (m_board.getSeedCount(cell) <= 0) continue;

        // Simulation sur copie
        KatroGame sim(false, m_board.getCols(), m_board.getInitialSeeds());
        sim.setBoard(m_board.toSeedVector());
        sim.m_turn     = 2;
        sim.m_finished = false;

        int seedsBefore = sim.countPlayerSeeds(KatroPlayer::NORTH);
        sim.playTurn(cell, 2, m_defaultDirection);
        int seedsAfter  = sim.countPlayerSeeds(KatroPlayer::NORTH);
        int gain        = seedsAfter - seedsBefore;

        if (gain > bestGain || (gain == bestGain && (bestCell == -1 || m_board.getSeedCount(cell) > m_board.getSeedCount(bestCell)))) {
            bestGain = gain;
            bestCell = cell;
        }
    }

    return bestCell;
}

} // namespace core
} // namespace gamehub
