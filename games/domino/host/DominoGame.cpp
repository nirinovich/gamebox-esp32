#include "DominoGame.hpp"
#include <gamehub/core/GameRegistry.hpp>
#include <iostream>
#include <sstream>
#include <algorithm>

namespace gamehub {
namespace core {

// Static self-registration in GameRegistry
REGISTER_GAME("domino_pvp", DominoPvp, []() -> std::unique_ptr<Game> {
    return std::make_unique<DominoGame>(false, 2, 100);
}, {
    gamehub::core::GameId("domino_pvp"),
    "Dominoes Duel",
    "DUEL",
    "Double-six Malagasy Tsipy Dominoes 1v1",
    2, 2,
    gamehub::core::InputFlag::ACTION_BUTTONS,
    "domino_duel",
    true
})

REGISTER_GAME("domino", DominoDuelAlias, []() -> std::unique_ptr<Game> {
    return std::make_unique<DominoGame>(false, 2, 100);
}, {
    gamehub::core::GameId("domino"),
    "Dominoes Duel",
    "DUEL",
    "Double-six Malagasy Tsipy Dominoes 1v1",
    2, 2,
    gamehub::core::InputFlag::ACTION_BUTTONS,
    "domino_duel",
    true
})

REGISTER_GAME("domino_solo", DominoSolo, []() -> std::unique_ptr<Game> {
    return std::make_unique<DominoGame>(true, 2, 100);
}, {
    gamehub::core::GameId("domino_solo"),
    "Dominoes Solo",
    "SOLO",
    "Double-six Malagasy Tsipy Dominoes vs Bot",
    1, 1,
    gamehub::core::InputFlag::ACTION_BUTTONS,
    "domino_solo",
    false
})

// ============================================================================
// Command Pattern Implementations (Double Dispatch)
// ============================================================================

bool PlayTileCommand::execute(Game& game, PlayerId playerId) const {
    auto* domino = dynamic_cast<DominoGame*>(&game);
    if (!domino) return false;
    return domino->processCommand(playerId, *this);
}

bool PassTurnCommand::execute(Game& game, PlayerId playerId) const {
    auto* domino = dynamic_cast<DominoGame*>(&game);
    if (!domino) return false;
    return domino->processCommand(playerId, *this);
}

bool ChooseEndCommand::execute(Game& game, PlayerId playerId) const {
    auto* domino = dynamic_cast<DominoGame*>(&game);
    if (!domino) return false;
    return domino->processCommand(playerId, *this);
}

// ============================================================================
// State Pattern Implementations
// ============================================================================

void DominoPlayState::onEnter(Game& /*game*/) {}

bool DominoPlayState::handleCommand(Game& game, PlayerId playerId, const ICommand& command) {
    auto* domino = dynamic_cast<DominoGame*>(&game);
    if (!domino) return false;

    int pidx = static_cast<int>(playerId) - 1;
    if (pidx < 0 || pidx >= domino->getNumPlayers()) return false;
    if (domino->getTurn() != pidx + 1) return false;

    if (command.getType() == "play") {
        const auto* playCmd = dynamic_cast<const PlayTileCommand*>(&command);
        if (!playCmd) return false;

        int tileIdx = playCmd->getIndex();
        const auto& hand = domino->getHand(pidx);
        if (tileIdx < 0 || static_cast<size_t>(tileIdx) >= hand.size()) return false;

        const auto& tile = hand.getTile(tileIdx);
        int lEnd = domino->getLeftEnd();
        int rEnd = domino->getRightEnd();

        bool canL = (lEnd == -1) || tile.matches(lEnd);
        bool canR = (rEnd == -1) || tile.matches(rEnd);

        if (!canL && !canR) return false;

        // Ambiguous choice when tile can attach to both distinct ends
        if (canL && canR && lEnd != rEnd && playCmd->getEndChoice().empty()) {
            domino->transitionTo(std::make_unique<DominoPendingChoiceState>(pidx, tileIdx));
            return true;
        }

        std::string chosen = playCmd->getEndChoice();
        if (chosen.empty()) {
            chosen = canR ? "right" : "left";
        }

        return domino->executePlayTile(pidx, tileIdx, chosen);
    } else if (command.getType() == "pass") {
        if (domino->canPlayerPlay(pidx)) {
            return false; // Passing is forbidden if a legal move exists
        }
        domino->executePassTurn(pidx);
        return true;
    }

    return false;
}

void DominoPlayState::update(Game& game, float /*dt*/) {
    auto* domino = dynamic_cast<DominoGame*>(&game);
    if (!domino || domino->isFinished() || domino->isRoundFinished()) return;

    int curPidx = domino->getTurn() - 1;
    auto* player = domino->getPlayer(curPidx);
    if (player && player->isAutonomous()) {
        auto action = player->getController()->decideAction(game, static_cast<PlayerId>(curPidx + 1));
        if (action) {
            action->execute(game, static_cast<PlayerId>(curPidx + 1));
        }
    }
}

void DominoPendingChoiceState::onEnter(Game& game) {
    auto* domino = dynamic_cast<DominoGame*>(&game);
    if (domino) {
        domino->setPendingChoice(m_playerIndex, m_tileIndex);
    }
}

bool DominoPendingChoiceState::handleCommand(Game& game, PlayerId playerId, const ICommand& command) {
    auto* domino = dynamic_cast<DominoGame*>(&game);
    if (!domino) return false;

    int pidx = static_cast<int>(playerId) - 1;
    if (pidx != m_playerIndex) return false;

    if (command.getType() == "choose_end") {
        const auto* chooseCmd = dynamic_cast<const ChooseEndCommand*>(&command);
        if (!chooseCmd) return false;

        std::string choice = chooseCmd->getChoice();
        int tileIdx = m_tileIndex;

        domino->clearPendingChoice();
        domino->transitionTo(std::make_unique<DominoPlayState>());
        return domino->executePlayTile(pidx, tileIdx, choice);
    }

    return false;
}

void DominoRoundOverState::onEnter(Game& game) {
    auto* domino = dynamic_cast<DominoGame*>(&game);
    if (domino) {
        domino->setLastAction("Round Finished");
    }
}

bool DominoRoundOverState::handleCommand(Game& game, PlayerId /*playerId*/, const ICommand& command) {
    auto* domino = dynamic_cast<DominoGame*>(&game);
    if (!domino) return false;

    if (command.getType() == "play" || command.getType() == "start_round") {
        domino->startNewRound();
        return true;
    }
    return false;
}

void DominoGameOverState::onEnter(Game& game) {
    auto* domino = dynamic_cast<DominoGame*>(&game);
    if (domino) {
        domino->setLastAction("Match Concluded");
    }
}

// ============================================================================
// Strategy Pattern Implementation
// ============================================================================

std::unique_ptr<ICommand> DominoBotController::decideAction(const Game& game, PlayerId id) {
    const auto* domino = dynamic_cast<const DominoGame*>(&game);
    if (!domino) return nullptr;

    int bidx = static_cast<int>(id) - 1;
    if (bidx < 0 || bidx >= domino->getNumPlayers()) return nullptr;

    const auto& hand = domino->getHand(bidx);
    int lEnd = domino->getLeftEnd();
    int rEnd = domino->getRightEnd();

    int bestIdx = -1;
    int bestPips = -1;
    std::string bestEnd = "right";

    for (size_t i = 0; i < hand.size(); ++i) {
        const auto& t = hand.getTile(i);
        bool canL = (lEnd == -1) || t.matches(lEnd);
        bool canR = (rEnd == -1) || t.matches(rEnd);

        if (canL || canR) {
            int p = t.pips();
            if (t.left == t.right) p += 100; // Prefer doubles

            if (p > bestPips) {
                bestPips = p;
                bestIdx = static_cast<int>(i);
                bestEnd = canR ? "right" : "left";
            }
        }
    }

    if (bestIdx != -1) {
        return std::make_unique<PlayTileCommand>(bestIdx, bestEnd);
    } else {
        return std::make_unique<PassTurnCommand>();
    }
}

// ============================================================================
// DominoGame Implementation
// ============================================================================

DominoGame::DominoGame(bool isSolo, int numPlayers, int targetScore)
    : m_isSolo(isSolo), m_numPlayers(numPlayers), m_targetScore(targetScore), m_hands(4) {
    if (m_numPlayers < 2) m_numPlayers = 2;
    if (m_numPlayers > 4) m_numPlayers = 4;
    m_state = std::make_unique<DominoPlayState>();
}

void DominoGame::setNumPlayers(int n) {
    if (n >= 2 && n <= 4) {
        m_numPlayers = n;
        init();
    }
}

void DominoGame::transitionTo(std::unique_ptr<IGamePhaseState> nextState) {
    if (m_state) {
        m_state->onExit(*this);
    }
    m_state = std::move(nextState);
    if (m_state) {
        m_state->onEnter(*this);
    }
}

bool DominoGame::processCommand(PlayerId player, const ICommand& cmd) {
    if (!m_state) return false;
    return m_state->handleCommand(*this, player, cmd);
}

void DominoGame::init() {
    m_finished = false;
    m_winner = 0;
    m_round = 1;
    m_roundFinished = false;
    m_roundWinner = 0;
    m_roundPointsAwarded = 0;
    m_rebonPool = 0;
    m_consecutivePasses = 0;
    m_lastAction = "Match started";

    for (int i = 0; i < 4; ++i) {
        m_cumulativeScores[i] = 0;
    }

    // Configure Player strategy entities
    m_players.clear();
    for (int i = 0; i < m_numPlayers; ++i) {
        std::unique_ptr<PlayerController> ctrl;
        std::string name;
        if (m_isSolo && i > 0) {
            ctrl = std::make_unique<DominoBotController>();
            name = (m_numPlayers > 2) ? ("Bot " + std::to_string(i)) : "Bot";
        } else {
            ctrl = std::make_unique<HumanController>();
            name = "Player " + std::to_string(i + 1);
        }
        m_players.push_back(std::make_unique<Player>(static_cast<PlayerId>(i + 1), name, std::move(ctrl)));
    }

    startNewRound();
}

void DominoGame::startNewRound() {
    m_roundFinished = false;
    m_roundWinner = 0;
    m_roundPointsAwarded = 0;
    m_consecutivePasses = 0;
    clearPendingChoice();
    m_board.clear();

    m_tileSet.shuffleAndDeal(m_numPlayers, m_hands, m_rng);
    determineOpeningLead();
    transitionTo(std::make_unique<DominoPlayState>());
}

void DominoGame::determineOpeningLead() {
    for (int d = 6; d >= 0; --d) {
        for (int p = 0; p < m_numPlayers; ++p) {
            for (const auto& t : m_hands[p].getTiles()) {
                if (t.left == d && t.right == d) {
                    m_turn = p + 1;
                    return;
                }
            }
        }
    }

    int highestPips = -1;
    int leadPlayer = 1;
    for (int p = 0; p < m_numPlayers; ++p) {
        for (const auto& t : m_hands[p].getTiles()) {
            if (t.pips() > highestPips) {
                highestPips = t.pips();
                leadPlayer = p + 1;
            }
        }
    }
    m_turn = leadPlayer;
}

void DominoGame::update(float dt) {
    if (m_state) {
        m_state->update(*this, dt);
    }
}

void DominoGame::handleInput(PlayerId player, const std::string& input) {
    auto cmd = m_commandFactory.createCommand(input);
    if (cmd) {
        cmd->execute(*this, player);
    }
}

bool DominoGame::canPlayerPlay(int pidx) const {
    if (pidx < 0 || pidx >= m_numPlayers) return false;
    return m_hands[pidx].canMatchAny(m_board.getLeftEnd(), m_board.getRightEnd());
}

void DominoGame::advanceTurn() {
    m_turn = (m_turn % m_numPlayers) + 1;
}

bool DominoGame::executePlayTile(int pidx, int tileIdx, const std::string& endChoice) {
    if (pidx < 0 || pidx >= m_numPlayers) return false;
    if (tileIdx < 0 || static_cast<size_t>(tileIdx) >= m_hands[pidx].size()) return false;

    DominoTile tile = m_hands[pidx].getTile(tileIdx);
    if (!m_board.placeTile(tile, endChoice)) {
        return false;
    }

    m_hands[pidx].removeTile(tileIdx);
    m_consecutivePasses = 0;

    auto* player = getPlayer(pidx);
    std::string actor = player ? player->getName() : ("Player " + std::to_string(pidx + 1));
    m_lastAction = actor + " played [" + std::to_string(tile.left) + "|" + std::to_string(tile.right) + "]";

    // Check hand emptied win
    if (m_hands[pidx].empty()) {
        int oppPips = 0;
        for (int p = 0; p < m_numPlayers; ++p) {
            if (p != pidx) {
                oppPips += m_hands[p].calculateTotalPips();
            }
        }
        int roundPoints = oppPips + m_rebonPool;
        m_cumulativeScores[pidx] += roundPoints;
        m_rebonPool = 0;

        setRoundFinished(true, pidx + 1, roundPoints);
        if (m_cumulativeScores[pidx] >= m_targetScore) {
            setFinished(true, pidx + 1);
            transitionTo(std::make_unique<DominoGameOverState>());
        } else {
            transitionTo(std::make_unique<DominoRoundOverState>());
        }
        return true;
    }

    advanceTurn();
    return true;
}

void DominoGame::executePassTurn(int pidx) {
    m_consecutivePasses++;
    auto* player = getPlayer(pidx);
    std::string actor = player ? player->getName() : ("Player " + std::to_string(pidx + 1));
    m_lastAction = actor + " passed turn";

    if (m_consecutivePasses >= m_numPlayers) {
        checkBlockedWin();
        return;
    }

    advanceTurn();
}

void DominoGame::checkBlockedWin() {
    int minPips = 9999;
    int winnerPidx = -1;
    bool tie = false;

    for (int p = 0; p < m_numPlayers; ++p) {
        int pips = m_hands[p].calculateTotalPips();
        if (pips < minPips) {
            minPips = pips;
            winnerPidx = p;
            tie = false;
        } else if (pips == minPips) {
            tie = true;
        }
    }

    if (tie) {
        int totalTablePips = 0;
        for (int p = 0; p < m_numPlayers; ++p) {
            totalTablePips += m_hands[p].calculateTotalPips();
        }
        m_rebonPool += totalTablePips;
        m_lastAction = "Round Blocked - Tie! " + std::to_string(totalTablePips) + " pts added to Rebon pool";

        setRoundFinished(true, 0, 0);
        transitionTo(std::make_unique<DominoRoundOverState>());
    } else {
        int oppPips = 0;
        for (int p = 0; p < m_numPlayers; ++p) {
            if (p != winnerPidx) {
                oppPips += m_hands[p].calculateTotalPips();
            }
        }
        int roundPoints = oppPips + m_rebonPool;
        m_cumulativeScores[winnerPidx] += roundPoints;
        m_rebonPool = 0;

        m_lastAction = "Round Blocked - Player " + std::to_string(winnerPidx + 1) + " wins with lowest pips (" + std::to_string(minPips) + ")";
        setRoundFinished(true, winnerPidx + 1, roundPoints);

        if (m_cumulativeScores[winnerPidx] >= m_targetScore) {
            setFinished(true, winnerPidx + 1);
            transitionTo(std::make_unique<DominoGameOverState>());
        } else {
            transitionTo(std::make_unique<DominoRoundOverState>());
        }
    }
}

std::string DominoGame::serializeState() const {
    return serializeStateForPlayer(1);
}

std::string DominoGame::serializeStateForPlayer(PlayerId id) const {
    std::ostringstream ss;
    ss << "{";
    ss << "\"type\":\"state\",";
    ss << "\"game\":\"domino\",";
    ss << "\"num_players\":" << m_numPlayers << ",";
    ss << "\"target_score\":" << m_targetScore << ",";
    ss << "\"rebon\":" << m_rebonPool << ",";
    ss << "\"round\":" << m_round << ",";
    ss << "\"round_finished\":" << (m_roundFinished ? "true" : "false") << ",";
    ss << "\"round_winner\":" << m_roundWinner << ",";
    ss << "\"round_points_awarded\":" << m_roundPointsAwarded << ",";
    ss << "\"is_solo\":" << (m_isSolo ? "true" : "false") << ",";
    ss << "\"turn\":" << m_turn << ",";
    ss << "\"finished\":" << (m_finished ? "true" : "false") << ",";
    ss << "\"winner\":" << m_winner << ",";
    ss << "\"left_end\":" << m_board.getLeftEnd() << ",";
    ss << "\"right_end\":" << m_board.getRightEnd() << ",";
    ss << "\"deadwood_count\":" << m_tileSet.getDeadwoodCount() << ",";
    ss << "\"boneyard_count\":" << m_tileSet.getDeadwoodCount() << ",";
    ss << "\"last_action\":\"" << m_lastAction << "\",";

    ss << "\"cumulative_scores\":[";
    for (int p = 0; p < m_numPlayers; ++p) {
        ss << m_cumulativeScores[p];
        if (p < m_numPlayers - 1) ss << ",";
    }
    ss << "],";

    for (int p = 0; p < m_numPlayers; ++p) {
        ss << "\"p" << (p + 1) << "_count\":" << m_hands[p].size() << ",";
    }

    ss << "\"board\":[";
    const auto& boardTiles = m_board.getTiles();
    for (size_t i = 0; i < boardTiles.size(); ++i) {
        ss << "[" << boardTiles[i].left << "," << boardTiles[i].right << "]";
        if (i < boardTiles.size() - 1) ss << ",";
    }
    ss << "],";

    // Hands: Strictly mask opponents' hands (Fog-of-War)
    ss << "\"hands\":{";
    bool firstHand = true;
    for (int p = 0; p < m_numPlayers; ++p) {
        int pNum = p + 1;
        if (id == 0 || static_cast<int>(id) == pNum || m_roundFinished || m_finished) {
            if (!firstHand) ss << ",";
            firstHand = false;
            ss << "\"" << pNum << "\":[";
            const auto& pTiles = m_hands[p].getTiles();
            for (size_t t = 0; t < pTiles.size(); ++t) {
                if (t > 0) ss << ",";
                ss << "[" << pTiles[t].left << "," << pTiles[t].right << "]";
            }
            ss << "]";
        }
    }
    ss << "},";

    int clientPidx = static_cast<int>(id) - 1;
    if (clientPidx >= 0 && clientPidx < m_numPlayers) {
        ss << "\"my_hand\":[";
        const auto& myTiles = m_hands[clientPidx].getTiles();
        for (size_t i = 0; i < myTiles.size(); ++i) {
            ss << "[" << myTiles[i].left << "," << myTiles[i].right << "]";
            if (i < myTiles.size() - 1) ss << ",";
        }
        ss << "],";
    } else {
        ss << "\"my_hand\":[],";
    }

    if (m_pendingChoicePlayer == static_cast<int>(id) && m_pendingChoiceIdx != -1) {
        ss << "\"pending_choice\":true,";
        ss << "\"pending_tile_idx\":" << m_pendingChoiceIdx << ",";
    } else {
        ss << "\"pending_choice\":false,";
    }

    ss << "\"available_actions\":[";
    bool needComma = false;
    if (m_turn == static_cast<int>(id) && !m_roundFinished && !m_finished) {
        if (m_pendingChoicePlayer == static_cast<int>(id) && m_pendingChoiceIdx != -1) {
            ss << "{\"id\":\"choose_left\",\"action\":\"choose_end\",\"label\":\"Place Left\",\"params\":{\"choice\":\"left\"}},";
            ss << "{\"id\":\"choose_right\",\"action\":\"choose_end\",\"label\":\"Place Right\",\"params\":{\"choice\":\"right\"}}";
            needComma = true;
        } else {
            bool hasPlayable = canPlayerPlay(clientPidx);
            if (!hasPlayable) {
                ss << "{\"id\":\"pass\",\"action\":\"pass\",\"label\":\"Pass Turn\"}";
                needComma = true;
            } else {
                ss << "{\"id\":\"play\",\"action\":\"play\",\"label\":\"Play Tile\"}";
                needComma = true;
            }
        }
    }
    if (m_roundFinished && !m_finished) {
        if (needComma) ss << ",";
        ss << "{\"id\":\"play\",\"action\":\"play\",\"label\":\"Next Round\"}";
    }
    ss << "]";

    ss << "}";
    return ss.str();
}

} // namespace core
} // namespace gamehub
