// ============================================================================
// KatroGame.cpp — Implémentation fidèle du Katro tsotra malgache
//
// Référence officielle :
//   Luc TIENNOT, "Les jeux de semailles à Madagascar",
//   Bulletin de l'APMEP / Publimath AVM14034, pages 74 à 76.
//
// Principes algorithmiques :
//   - Plateau 4×4 par défaut (2 rangées Sud, 2 rangées Nord).
//   - Circuits fermés indépendants pour chaque joueur (2×C cases).
//   - Sens horaire (par défaut) et antihoraire supportés.
//   - Semis élémentaire avec relais dès qu'on arrive sur une case occupée.
//   - Règle exacte de capture en vis-à-vis (inner-row priority, outer si toute
//     la rangée intérieure adverse est vide).
//   - Réintégration immédiate des graines capturées au relais du coup.
//   - Conservation absolue du nombre total de graines.
// ============================================================================

#include "KatroGame.hpp"
#include <sstream>
#include <algorithm>

namespace gamehub {
namespace core {

// ============================================================================
// Constructeur
// ============================================================================

KatroGame::KatroGame(bool isSolo, int cols, int seedsPerCell)
    : m_rows(KATRO_DEFAULT_ROWS)
    , m_cols(cols > 0 ? cols : KATRO_DEFAULT_COLS)
    , m_totalCells(m_rows * m_cols)
    , m_seedsPerCell(seedsPerCell > 0 ? seedsPerCell : KATRO_DEFAULT_SEEDS)
    , m_isSolo(isSolo) {
    init();
}

// ============================================================================
// init() — Réinitialisation du plateau
// ============================================================================

void KatroGame::init() {
    m_board.assign(m_totalCells, m_seedsPerCell);
    m_turn     = 1;
    m_finished = false;
    m_winner   = 0;
    m_botTimer = 0.0f;
    m_lastEvents.clear();
    m_moveSeq  = 0;
}

// ============================================================================
// update() — Boucle de tick : exécute le coup du Bot (Nord) en mode solo
// ============================================================================

void KatroGame::update(float dt) {
    if (!m_isSolo || m_finished || m_turn != 2) return;

    m_botTimer += dt;
    if (m_botTimer < BOT_DELAY_SEC) return;
    m_botTimer = 0.0f;

    int bestCell = computeBotMove();
    if (bestCell >= 0) {
        playTurn(bestCell, 2, m_defaultDirection);
    }
}

// ============================================================================
// handleInput() — Décodage JSON de la commande utilisateur
// Commande acceptée : {"cmd":"sow","cell":N, ["dir":"cw"|"ccw"]}
// ============================================================================

void KatroGame::handleInput(PlayerId id, const std::string& commandJson) {
    if (m_finished) return;
    int playerNum = static_cast<int>(id);
    if (playerNum != m_turn) return;

    // Extraction de la case ("cell": N)
    const std::string cellKey = "\"cell\":";
    auto pos = commandJson.find(cellKey);
    if (pos == std::string::npos) return;

    int cell = -1;
    try {
        cell = std::stoi(commandJson.substr(pos + cellKey.size()));
    } catch (...) {
        return;
    }

    if (cell < 0 || cell >= m_totalCells) return;

    // Détection optionnelle du sens ("dir":"cw" ou "dir":"ccw")
    KatroDirection dir = m_defaultDirection;
    if (commandJson.find("\"ccw\"") != std::string::npos ||
        commandJson.find("\"counter_clockwise\"") != std::string::npos) {
        dir = KatroDirection::COUNTER_CLOCKWISE;
    } else if (commandJson.find("\"cw\"") != std::string::npos ||
               commandJson.find("\"clockwise\"") != std::string::npos) {
        dir = KatroDirection::CLOCKWISE;
    }

    playTurn(cell, playerNum, dir);
}

// ============================================================================
// serializeState() — Sérialisation JSON pour le frontend
// ============================================================================

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
    for (int i = 0; i < m_totalCells; ++i) {
        if (i > 0) oss << ",";
        oss << m_board[i];
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

// ============================================================================
// Géométrie du plateau et identification des cases
//
// Plateau à 4 rangées de C colonnes (numérotées de bas en haut) :
//   Rangée 3 (Nord ext) : 3*C .. 4*C - 1   [ex 4×4: 12, 13, 14, 15]
//   Rangée 2 (Nord int) : 2*C .. 3*C - 1   [ex 4×4:  8,  9, 10, 11]
//   ------------------ Centre du plateau ------------------
//   Rangée 1 (Sud int)  :   C .. 2*C - 1   [ex 4×4:  4,  5,  6,  7]
//   Rangée 0 (Sud ext)  :   0 ..   C - 1   [ex 4×4:  0,  1,  2,  3]
// ============================================================================

bool KatroGame::isPlayerCell(int cellIndex, KatroPlayer player) const {
    if (cellIndex < 0 || cellIndex >= m_totalCells) return false;
    if (player == KatroPlayer::SOUTH) {
        return cellIndex < 2 * m_cols;
    } else {
        return cellIndex >= 2 * m_cols && cellIndex < m_totalCells;
    }
}

bool KatroGame::isPlayerCell(int cellIndex, int playerNum) const {
    return isPlayerCell(cellIndex, (playerNum == 1) ? KatroPlayer::SOUTH : KatroPlayer::NORTH);
}

bool KatroGame::isInnerRow(int cellIndex, KatroPlayer player) const {
    if (cellIndex < 0 || cellIndex >= m_totalCells) return false;
    if (player == KatroPlayer::SOUTH) {
        return cellIndex >= m_cols && cellIndex < 2 * m_cols;  // Rangée 1
    } else {
        return cellIndex >= 2 * m_cols && cellIndex < 3 * m_cols;  // Rangée 2
    }
}

bool KatroGame::isInnerRow(int cellIndex, int playerNum) const {
    return isInnerRow(cellIndex, (playerNum == 1) ? KatroPlayer::SOUTH : KatroPlayer::NORTH);
}

bool KatroGame::isOuterRow(int cellIndex, KatroPlayer player) const {
    if (cellIndex < 0 || cellIndex >= m_totalCells) return false;
    if (player == KatroPlayer::SOUTH) {
        return cellIndex < m_cols;  // Rangée 0
    } else {
        return cellIndex >= 3 * m_cols && cellIndex < m_totalCells;  // Rangée 3
    }
}

// ============================================================================
// nextCell() — Case suivante dans le circuit fermé du joueur
//
// Circuit pour Sud (rangées 0 et 1) :
//   - CLOCKWISE (horaire) :
//       Rangée 0 (ext) : droite → gauche (col C-1 .. 0)
//       Virage gauche : (0, 0) → (1, 0)
//       Rangée 1 (int) : gauche → droite (col 0 .. C-1)
//       Virage droite : (1, C-1) → (0, C-1)
//   - COUNTER_CLOCKWISE (antihoraire) :
//       Rangée 0 (ext) : gauche → droite (col 0 .. C-1)
//       Virage droite : (0, C-1) → (1, C-1)
//       Rangée 1 (int) : droite → gauche (col C-1 .. 0)
//       Virage gauche : (1, 0) → (0, 0)
//
// Circuit pour Nord (rangées 2 et 3) :
//   - CLOCKWISE (horaire) :
//       Rangée 2 (int) : droite → gauche (col C-1 .. 0)
//       Virage gauche : (2, 0) → (3, 0)
//       Rangée 3 (ext) : gauche → droite (col 0 .. C-1)
//       Virage droite : (3, C-1) → (2, C-1)
//   - COUNTER_CLOCKWISE (antihoraire) :
//       Rangée 2 (int) : gauche → droite (col 0 .. C-1)
//       Virage droite : (2, C-1) → (3, C-1)
//       Rangée 3 (ext) : droite → gauche (col C-1 .. 0)
//       Virage gauche : (3, 0) → (2, 0)
// ============================================================================

int KatroGame::nextCell(int cellIndex, KatroPlayer player, KatroDirection dir) const {
    int row = cellIndex / m_cols;
    int col = cellIndex % m_cols;

    if (player == KatroPlayer::SOUTH) {
        if (dir == KatroDirection::CLOCKWISE) {
            if (row == 0) {
                if (col > 0) return cellIndex - 1;
                else         return m_cols;  // (1, 0)
            } else { // row == 1
                if (col < m_cols - 1) return cellIndex + 1;
                else                  return m_cols - 1;  // (0, C-1)
            }
        } else { // COUNTER_CLOCKWISE
            if (row == 0) {
                if (col < m_cols - 1) return cellIndex + 1;
                else                  return 2 * m_cols - 1;  // (1, C-1)
            } else { // row == 1
                if (col > 0) return cellIndex - 1;
                else         return 0;  // (0, 0)
            }
        }
    } else { // KatroPlayer::NORTH
        if (dir == KatroDirection::CLOCKWISE) {
            if (row == 2) {
                if (col > 0) return cellIndex - 1;
                else         return 3 * m_cols;  // (3, 0)
            } else { // row == 3
                if (col < m_cols - 1) return cellIndex + 1;
                else                  return 3 * m_cols - 1;  // (2, C-1)
            }
        } else { // COUNTER_CLOCKWISE
            if (row == 2) {
                if (col < m_cols - 1) return cellIndex + 1;
                else                  return 4 * m_cols - 1;  // (3, C-1)
            } else { // row == 3
                if (col > 0) return cellIndex - 1;
                else         return 2 * m_cols;  // (2, 0)
            }
        }
    }
}

// ============================================================================
// getOppositeInner() — Case adverse sur la rangée intérieure (même colonne)
// ============================================================================

int KatroGame::getOppositeInner(int cellIndex, KatroPlayer player) const {
    if (!isInnerRow(cellIndex, player)) return -1;
    int col = cellIndex % m_cols;
    if (player == KatroPlayer::SOUTH) {
        return 2 * m_cols + col;  // Rangée 2, colonne col
    } else {
        return 1 * m_cols + col;  // Rangée 1, colonne col
    }
}

// ============================================================================
// getOppositeOuter() — Case adverse sur la rangée extérieure (même colonne)
// ============================================================================

int KatroGame::getOppositeOuter(int cellIndex, KatroPlayer player) const {
    if (!isInnerRow(cellIndex, player)) return -1;
    int col = cellIndex % m_cols;
    if (player == KatroPlayer::SOUTH) {
        return 3 * m_cols + col;  // Rangée 3, colonne col
    } else {
        return 0 * m_cols + col;  // Rangée 0, colonne col
    }
}

// ============================================================================
// isOpponentInnerRowEmpty() — Vérifie si toute la rangée intérieure adverse est vide
// ============================================================================

bool KatroGame::isOpponentInnerRowEmpty(KatroPlayer player) const {
    int start = (player == KatroPlayer::SOUTH) ? (2 * m_cols) : (1 * m_cols);
    int end   = start + m_cols;
    for (int i = start; i < end; ++i) {
        if (m_board[i] > 0) return false;
    }
    return true;
}

// ============================================================================
// playTurn() — Exécute un coup complet selon les règles de Luc Tiennot (Publimath)
//
// Déroulement rigoureux :
//   1. Validation : case du camp, non vide, tour du joueur.
//   2. Prise de toutes les graines de la case sélectionnée.
//   3. Semis élémentaire : 1 graine par case dans le circuit du joueur.
//   4. Arrivée de la dernière graine dans la case 'current' :
//      - Évaluation de l'état de la case AVANT le dépôt : hadSeedsBefore.
//      - Si 'current' est sur la RANGÉE EXTÉRIEURE :
//          * Pas de capture possible.
//          * Si hadSeedsBefore == true → relais normal : prendre toutes les graines
//            de 'current' et relancer un nouveau semis élémentaire.
//          * Si hadSeedsBefore == false → fin du coup !
//      - Si 'current' est sur la RANGÉE INTÉRIEURE :
//          * Vérifier possibilité de capture en face :
//            a) Si la rangée intérieure adverse n'est pas entièrement vide :
//               vérifier la case intérieure adverse dans la même colonne.
//               Si elle contient des graines → CAPTURE de toutes ses graines.
//            b) Si TOUTE la rangée intérieure adverse est vide :
//               vérifier la case extérieure adverse dans la même colonne.
//               Si elle contient des graines → CAPTURE de toutes ses graines.
//          * Si une capture a eu lieu :
//            Les graines capturées sont ajoutées aux graines de la case d'arrivée 'current'.
//            La nouvelle collection est prise en main et un nouveau semis élémentaire
//            est lancé depuis 'current' dans le même sens.
//          * Si aucune capture n'a eu lieu :
//            Appliquer la règle normale de relais ou de fin :
//            si hadSeedsBefore == true → relais normal (reprendre les graines de 'current').
//            si hadSeedsBefore == false → fin du coup !
// ============================================================================

bool KatroGame::playTurn(int cell, int playerNum) {
    return playTurn(cell, playerNum, m_defaultDirection);
}

bool KatroGame::playTurn(int cell, int playerNum, KatroDirection dir) {
    if (cell < 0 || cell >= m_totalCells) return false;
    KatroPlayer player = (playerNum == 1) ? KatroPlayer::SOUTH : KatroPlayer::NORTH;

    if (!isPlayerCell(cell, player)) return false;
    if (m_board[cell] <= 0)          return false;
    if (m_turn != playerNum)         return false;
    if (m_finished)                  return false;

    m_lastEvents.clear();
    m_moveSeq++;

    // Prise des graines de la case de départ
    int seedsInHand = m_board[cell];
    m_board[cell]   = 0;
    m_lastEvents.push_back({"pickup", cell, -1, seedsInHand});

    int current = cell;
    int safetyCounter = 0; // Protection anti-boucle infinie (au-delà de 2000 relais)

    while (true) {
        if (++safetyCounter > 2000) break;

        // Semis élémentaire : dépose une graine par case
        while (seedsInHand > 0) {
            current = nextCell(current, player, dir);
            m_board[current]++;
            seedsInHand--;
            m_lastEvents.push_back({"drop", -1, current, 1});
        }

        // État de la case d'arrivée AVANT le dépôt de la dernière graine
        // Puisqu'on vient d'ajouter 1 graine, elle contenait des graines si > 1.
        bool hadSeedsBefore = (m_board[current] > 1);
        bool captured = false;

        // Règle de capture : uniquement sur la rangée intérieure du joueur
        if (isInnerRow(current, player)) {
            if (!isOpponentInnerRowEmpty(player)) {
                // Cas B : la rangée intérieure adverse a au moins une graine
                int oppInner = getOppositeInner(current, player);
                if (oppInner >= 0 && m_board[oppInner] > 0) {
                    int taken = m_board[oppInner];
                    m_board[oppInner] = 0;

                    // Les graines capturées sont ajoutées à la case d'arrivée
                    seedsInHand = m_board[current] + taken;
                    m_board[current] = 0;
                    captured = true;

                    m_lastEvents.push_back({"capture", oppInner, current, taken});
                    m_lastEvents.push_back({"pickup", current, -1, seedsInHand});
                }
            } else {
                // Cas C : toute la rangée intérieure adverse est vide
                int oppOuter = getOppositeOuter(current, player);
                if (oppOuter >= 0 && m_board[oppOuter] > 0) {
                    int taken = m_board[oppOuter];
                    m_board[oppOuter] = 0;

                    // Les graines capturées sont ajoutées à la case d'arrivée
                    seedsInHand = m_board[current] + taken;
                    m_board[current] = 0;
                    captured = true;

                    m_lastEvents.push_back({"capture", oppOuter, current, taken});
                    m_lastEvents.push_back({"pickup", current, -1, seedsInHand});
                }
            }
        }

        if (captured) {
            // Relais avec les graines capturées + celles de la case d'arrivée
            continue;
        }

        // Si aucune capture n'a eu lieu : règle générale de relais / arrêt
        if (hadSeedsBefore) {
            // La case contenait déjà des graines → relais normal
            seedsInHand = m_board[current];
            m_board[current] = 0;
            m_lastEvents.push_back({"pickup", current, -1, seedsInHand});
            continue;
        } else {
            // La case était vide avant le dépôt (contient 1 seule graine) → fin du coup
            break;
        }
    }

    // Vérification de la condition de fin de partie
    checkEndCondition();

    // Si la partie continue, passe le tour à l'adversaire
    if (!m_finished) {
        m_turn = (playerNum == 1) ? 2 : 1;
        m_botTimer = 0.0f;
    }

    return true;
}

// ============================================================================
// checkEndCondition() — Détection de la fin de partie
// ============================================================================

void KatroGame::checkEndCondition() {
    int southSeeds = countPlayerSeeds(KatroPlayer::SOUTH);
    int northSeeds = countPlayerSeeds(KatroPlayer::NORTH);
    bool southCanMove = canPlayerMove(KatroPlayer::SOUTH);
    bool northCanMove = canPlayerMove(KatroPlayer::NORTH);

    if (southSeeds <= m_earlyStopThreshold || !southCanMove) {
        m_finished = true;
        m_winner   = 2;  // Nord gagne
    } else if (northSeeds <= m_earlyStopThreshold || !northCanMove) {
        m_finished = true;
        m_winner   = 1;  // Sud gagne
    }
}

bool KatroGame::canPlayerMove(KatroPlayer player) const {
    int start = (player == KatroPlayer::SOUTH) ? 0 : (2 * m_cols);
    int end   = start + (2 * m_cols);
    for (int i = start; i < end; ++i) {
        if (m_board[i] > 0) return true;
    }
    return false;
}

// ============================================================================
// Accesseurs et comptages
// ============================================================================

int KatroGame::getSeedCount(int cellIndex) const {
    if (cellIndex < 0 || cellIndex >= m_totalCells) return -1;
    return m_board[cellIndex];
}

int KatroGame::countPlayerSeeds(KatroPlayer player) const {
    int start = (player == KatroPlayer::SOUTH) ? 0 : (2 * m_cols);
    int end   = start + (2 * m_cols);
    int sum   = 0;
    for (int i = start; i < end; ++i) {
        sum += m_board[i];
    }
    return sum;
}

int KatroGame::countPlayerSeeds(int playerNum) const {
    return countPlayerSeeds((playerNum == 1) ? KatroPlayer::SOUTH : KatroPlayer::NORTH);
}

int KatroGame::getTotalSeedsOnBoard() const {
    int sum = 0;
    for (int count : m_board) sum += count;
    return sum;
}

int KatroGame::getScore(int playerNum) const {
    return countPlayerSeeds(playerNum);
}

void KatroGame::setBoard(const std::vector<int>& board) {
    if (static_cast<int>(board.size()) == m_totalCells) {
        m_board = board;
    }
}

// ============================================================================
// computeBotMove() — Bot heuristique pour Nord (P2)
//
// Évalue chaque coup légal de Nord et choisit celui qui maximise
// le nombre de graines finales de Nord après le coup.
// ============================================================================

int KatroGame::computeBotMove() const {
    int bestCell  = -1;
    int bestGain  = -9999;
    int startNord = 2 * m_cols;
    int endNord   = m_totalCells;

    for (int cell = startNord; cell < endNord; ++cell) {
        if (m_board[cell] <= 0) continue;

        // Simulation sur copie
        KatroGame sim(false, m_cols, m_seedsPerCell);
        sim.m_board    = m_board;
        sim.m_turn     = 2;
        sim.m_finished = false;

        int seedsBefore = sim.countPlayerSeeds(KatroPlayer::NORTH);
        sim.playTurn(cell, 2, m_defaultDirection);
        int seedsAfter  = sim.countPlayerSeeds(KatroPlayer::NORTH);
        int gain        = seedsAfter - seedsBefore;

        if (gain > bestGain || (gain == bestGain && (bestCell == -1 || m_board[cell] > m_board[bestCell]))) {
            bestGain = gain;
            bestCell = cell;
        }
    }

    return bestCell;
}

} // namespace core
} // namespace gamehub
