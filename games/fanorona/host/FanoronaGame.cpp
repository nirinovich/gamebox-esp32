#include "FanoronaGame.hpp"
#include <gamehub/core/GameRegistry.hpp>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <charconv>

namespace gamehub {
namespace core {

namespace {
std::unique_ptr<Game> createFanoronaSolo() {
    return std::make_unique<FanoronaGame>(true, 1);
}

std::unique_ptr<Game> createFanoronaDuel() {
    return std::make_unique<FanoronaGame>(false, 1);
}
} // namespace

REGISTER_GAME("fanorona_solo", FanoronaSolo, &createFanoronaSolo, {
    gamehub::core::GameId("fanorona_solo"),
    "Fanorona Solo",
    "SOLO",
    "Jeu traditionnel malgache 5x9 vs Bot avec captures successives",
    1, 1,
    gamehub::core::InputFlag::GRID_POINTER | gamehub::core::InputFlag::ACTION_BUTTONS,
    "fanorona_solo",
    false
})

REGISTER_GAME("fanorona_duel", FanoronaDuel, &createFanoronaDuel, {
    gamehub::core::GameId("fanorona_duel"),
    "Fanorona Duel",
    "DUEL",
    "Duel strategique malgache 5x9 par percussion et aspiration",
    2, 2,
    gamehub::core::InputFlag::GRID_POINTER | gamehub::core::InputFlag::ACTION_BUTTONS,
    "fanorona_duel",
    true
})

REGISTER_GAME_ALIAS("fanorona_pvp", FanoronaPvpAlias, &createFanoronaDuel)

REGISTER_GAME_ALIAS("fanorona", FanoronaAlias, &createFanoronaSolo)

FanoronaGame::FanoronaGame(bool isSolo, int startingPlayer)
    : m_isSolo(isSolo), m_startingPlayer(startingPlayer) {
    initGraph();
    init();
}

bool FanoronaGame::isStrongNode(int node) {
    if (node < 0 || node >= NUM_INTERSECTIONS) return false;
    int r = node / COLS;
    int c = node % COLS;
    return isStrongNode(r, c);
}

bool FanoronaGame::isStrongNode(int r, int c) {
    return ((r + c) % 2) == 0;
}

void FanoronaGame::initGraph() {
    for (int i = 0; i < NUM_INTERSECTIONS; ++i) {
        m_adj[i].clear();
        int r = i / COLS;
        int c = i % COLS;

        // 1. Orthogonal connections
        if (r > 0)          m_adj[i].push_back({(r - 1) * COLS + c, -1, 0});
        if (r < ROWS - 1)   m_adj[i].push_back({(r + 1) * COLS + c,  1, 0});
        if (c > 0)          m_adj[i].push_back({r * COLS + (c - 1),  0, -1});
        if (c < COLS - 1)   m_adj[i].push_back({r * COLS + (c + 1),  0,  1});

        // 2. Diagonal connections ONLY on traditional strong intersections
        if (isStrongNode(r, c)) {
            if (r > 0 && c > 0)                 m_adj[i].push_back({(r - 1) * COLS + (c - 1), -1, -1});
            if (r > 0 && c < COLS - 1)          m_adj[i].push_back({(r - 1) * COLS + (c + 1), -1,  1});
            if (r < ROWS - 1 && c > 0)          m_adj[i].push_back({(r + 1) * COLS + (c - 1),  1, -1});
            if (r < ROWS - 1 && c < COLS - 1)   m_adj[i].push_back({(r + 1) * COLS + (c + 1),  1,  1});
        }
    }
}

const std::vector<FanoronaEdge>& FanoronaGame::getEdges(int node) const {
    static const std::vector<FanoronaEdge> emptyEdges;
    if (node < 0 || node >= NUM_INTERSECTIONS) return emptyEdges;
    return m_adj[node];
}

bool FanoronaGame::isConnected(int from, int to) const {
    if (from < 0 || from >= NUM_INTERSECTIONS || to < 0 || to >= NUM_INTERSECTIONS) return false;
    for (const auto& edge : m_adj[from]) {
        if (edge.to == to) return true;
    }
    return false;
}

FanoronaEdge FanoronaGame::getEdge(int from, int to) const {
    if (from >= 0 && from < NUM_INTERSECTIONS) {
        for (const auto& edge : m_adj[from]) {
            if (edge.to == to) return edge;
        }
    }
    return {-1, 0, 0};
}

int FanoronaGame::getPieceAt(int node) const {
    if (node < 0 || node >= NUM_INTERSECTIONS) return 0;
    return m_board[node];
}

int FanoronaGame::getPieceAt(int r, int c) const {
    if (r < 0 || r >= ROWS || c < 0 || c >= COLS) return 0;
    return m_board[r * COLS + c];
}

void FanoronaGame::setStartingPlayer(int player) {
    if (player == 1 || player == 2) {
        m_startingPlayer = player;
    }
}

void FanoronaGame::resetBoard() {
    m_board.fill(0);

    // Row 0: 9 Player 1 (dark / foncées)
    for (int c = 0; c < COLS; ++c) m_board[c] = 1;

    // Row 1: 9 Player 1 (dark / foncées)
    for (int c = 0; c < COLS; ++c) m_board[COLS + c] = 1;

    // Row 2 (Middle): 2, 1, 2, 1, 0, 2, 1, 2, 1
    const int midRow[COLS] = {2, 1, 2, 1, 0, 2, 1, 2, 1};
    for (int c = 0; c < COLS; ++c) m_board[2 * COLS + c] = midRow[c];

    // Row 3: 9 Player 2 (light / claires)
    for (int c = 0; c < COLS; ++c) m_board[3 * COLS + c] = 2;

    // Row 4: 9 Player 2 (light / claires)
    for (int c = 0; c < COLS; ++c) m_board[4 * COLS + c] = 2;
}

void FanoronaGame::init() {
    resetBoard();
    m_turn = m_startingPlayer;
    m_finished = false;
    m_winner = 0;
    m_winReason.clear();
    m_lastAction = "Partie initialisée.";

    m_inMultiCapture = false;
    m_activePiece = -1;
    m_visitedPositions.clear();
    m_lastDirection = {0, 0};

    m_moveCounter = 0;
    m_lastMove = {};

    // If solo mode and Player 2 starts (traditional White starts), bot takes opening turn
    if (m_isSolo && m_turn == 2) {
        executeBotTurn();
    }
}

void FanoronaGame::setPlayerCount(int count) {
    m_isSolo = (count <= 1);
}

void FanoronaGame::update(float /*dt*/) {
    // Solo bot turns are sequenced step-by-step via client bot_move to allow smooth animations
}

std::vector<int> FanoronaGame::getApproachCaptures(int from, int to) const {
    std::vector<int> captured;
    if (from < 0 || from >= NUM_INTERSECTIONS || to < 0 || to >= NUM_INTERSECTIONS) return captured;
    if (!isConnected(from, to)) return captured;

    int movingPlayer = m_board[from];
    if (movingPlayer != 1 && movingPlayer != 2) return captured;
    int opponent = (movingPlayer == 1) ? 2 : 1;

    int from_r = from / COLS;
    int from_c = from % COLS;
    int to_r = to / COLS;
    int to_c = to % COLS;
    int dr = to_r - from_r;
    int dc = to_c - from_c;

    int r = to_r + dr;
    int c = to_c + dc;
    while (r >= 0 && r < ROWS && c >= 0 && c < COLS) {
        int node = r * COLS + c;
        if (m_board[node] == opponent) {
            captured.push_back(node);
            r += dr;
            c += dc;
        } else {
            break;
        }
    }
    return captured;
}

std::vector<int> FanoronaGame::getWithdrawalCaptures(int from, int to) const {
    std::vector<int> captured;
    if (from < 0 || from >= NUM_INTERSECTIONS || to < 0 || to >= NUM_INTERSECTIONS) return captured;
    if (!isConnected(from, to)) return captured;

    int movingPlayer = m_board[from];
    if (movingPlayer != 1 && movingPlayer != 2) return captured;
    int opponent = (movingPlayer == 1) ? 2 : 1;

    int from_r = from / COLS;
    int from_c = from % COLS;
    int to_r = to / COLS;
    int to_c = to % COLS;
    int dr = to_r - from_r;
    int dc = to_c - from_c;

    int r = from_r - dr;
    int c = from_c - dc;
    while (r >= 0 && r < ROWS && c >= 0 && c < COLS) {
        int node = r * COLS + c;
        if (m_board[node] == opponent) {
            captured.push_back(node);
            r -= dr;
            c -= dc;
        } else {
            break;
        }
    }
    return captured;
}

bool FanoronaGame::hasAnyLegalCaptures(int player) const {
    for (int from = 0; from < NUM_INTERSECTIONS; ++from) {
        if (m_board[from] != player) continue;
        for (const auto& edge : m_adj[from]) {
            int to = edge.to;
            if (m_board[to] != 0) continue;
            if (!getApproachCaptures(from, to).empty() || !getWithdrawalCaptures(from, to).empty()) {
                return true;
            }
        }
    }
    return false;
}

bool FanoronaGame::hasAnyLegalMoves(int player) const {
    if (m_inMultiCapture) {
        if (m_activePiece < 0 || m_board[m_activePiece] != player) return false;
        return canContinueCaptureFrom(m_activePiece, m_lastDirection, m_visitedPositions);
    }

    if (hasAnyLegalCaptures(player)) return true;

    // If no captures, check for any ordinary move to empty connected neighbor
    for (int from = 0; from < NUM_INTERSECTIONS; ++from) {
        if (m_board[from] != player) continue;
        for (const auto& edge : m_adj[from]) {
            if (m_board[edge.to] == 0) return true;
        }
    }
    return false;
}

bool FanoronaGame::canContinueCaptureFrom(int node, const std::pair<int, int>& lastDir, const std::vector<int>& visited) const {
    if (node < 0 || node >= NUM_INTERSECTIONS || m_board[node] == 0) return false;

    for (const auto& edge : m_adj[node]) {
        int to = edge.to;
        if (m_board[to] != 0) continue;

        // Cannot revisit any intersection already traversed in this series
        if (std::find(visited.begin(), visited.end(), to) != visited.end()) continue;

        // Must use a direction different from the previous move
        if (edge.dr == lastDir.first && edge.dc == lastDir.second) continue;

        // Must yield a capture (approach or withdrawal)
        if (!getApproachCaptures(node, to).empty() || !getWithdrawalCaptures(node, to).empty()) {
            return true;
        }
    }
    return false;
}

std::vector<int> FanoronaGame::getValidSuccessiveMoves(int node) const {
    std::vector<int> validMoves;
    if (!m_inMultiCapture || node != m_activePiece) return validMoves;

    for (const auto& edge : m_adj[node]) {
        int to = edge.to;
        if (m_board[to] != 0) continue;
        if (std::find(m_visitedPositions.begin(), m_visitedPositions.end(), to) != m_visitedPositions.end()) continue;
        if (edge.dr == m_lastDirection.first && edge.dc == m_lastDirection.second) continue;

        if (!getApproachCaptures(node, to).empty() || !getWithdrawalCaptures(node, to).empty()) {
            validMoves.push_back(to);
        }
    }
    return validMoves;
}

bool FanoronaGame::canMakeMove(int player, int from, int to, std::string* error) const {
    if (m_finished) {
        if (error) *error = "La partie est terminée.";
        return false;
    }
    if (player != m_turn) {
        if (error) *error = "Ce n'est pas votre tour.";
        return false;
    }
    if (from < 0 || from >= NUM_INTERSECTIONS || to < 0 || to >= NUM_INTERSECTIONS) {
        if (error) *error = "Coordonnées hors limites.";
        return false;
    }
    if (m_board[from] != player) {
        if (error) *error = "La pierre sélectionnée n'appartient pas au joueur actif.";
        return false;
    }
    if (m_board[to] != 0) {
        if (error) *error = "L'intersection cible est déjà occupée.";
        return false;
    }
    if (!isConnected(from, to)) {
        if (error) *error = "Déplacement interdit : aucune connexion entre ces intersections.";
        return false;
    }

    if (m_inMultiCapture) {
        if (from != m_activePiece) {
            if (error) *error = "Série de captures en cours : vous devez continuer avec la même pierre.";
            return false;
        }
        if (std::find(m_visitedPositions.begin(), m_visitedPositions.end(), to) != m_visitedPositions.end()) {
            if (error) *error = "Une intersection déjà visitée lors de cette série ne peut pas être retraversée.";
            return false;
        }
        int dr = (to / COLS) - (from / COLS);
        int dc = (to % COLS) - (from % COLS);
        if (dr == m_lastDirection.first && dc == m_lastDirection.second) {
            if (error) *error = "La capture successive doit utiliser une direction différente du mouvement précédent.";
            return false;
        }
        if (getApproachCaptures(from, to).empty() && getWithdrawalCaptures(from, to).empty()) {
            if (error) *error = "Ce mouvement ne produit aucune capture successive légale.";
            return false;
        }
        return true;
    }

    bool capturesAvailable = hasAnyLegalCaptures(player);
    bool thisMoveCaptures = (!getApproachCaptures(from, to).empty() || !getWithdrawalCaptures(from, to).empty());

    if (capturesAvailable && !thisMoveCaptures) {
        if (error) *error = "Priorité des captures : une capture est obligatoire lorsqu'elle est possible.";
        return false;
    }

    return true;
}

bool FanoronaGame::executeMove(int player, int from, int to, const std::string& captureType) {
    std::string err;
    if (!canMakeMove(player, from, to, &err)) {
        m_lastAction = err;
        return false;
    }

    auto appCaptures = getApproachCaptures(from, to);
    auto witCaptures = getWithdrawalCaptures(from, to);
    std::vector<int> chosenCaptures;

    bool hasApp = !appCaptures.empty();
    bool hasWit = !witCaptures.empty();

    if (hasApp && hasWit) {
        if (captureType == "withdrawal" || captureType == "aspiration" || captureType == "W") {
            chosenCaptures = witCaptures;
        } else if (captureType == "approach" || captureType == "percussion" || captureType == "A") {
            chosenCaptures = appCaptures;
        } else {
            // If both are available and not explicitly specified, default to approach
            chosenCaptures = appCaptures;
        }
    } else if (hasApp) {
        chosenCaptures = appCaptures;
    } else if (hasWit) {
        chosenCaptures = witCaptures;
    }

    // Execute physical movement
    m_board[to] = player;
    m_board[from] = 0;

    // Remove captured stones
    for (int cNode : chosenCaptures) {
        m_board[cNode] = 0;
    }

    int dr = (to / COLS) - (from / COLS);
    int dc = (to % COLS) - (from % COLS);

    if (!chosenCaptures.empty()) {
        if (!m_inMultiCapture) {
            m_visitedPositions = {from, to};
        } else {
            m_visitedPositions.push_back(to);
        }
        m_activePiece = to;
        m_lastDirection = {dr, dc};

        // Rule 3.7: Strict multi-capture continuation
        if (canContinueCaptureFrom(to, m_lastDirection, m_visitedPositions)) {
            m_inMultiCapture = true;
            std::ostringstream oss;
            oss << "Joueur " << player << " capture " << chosenCaptures.size()
                << " pierre(s). Capture successive obligatoire !";
            m_lastAction = oss.str();
        } else {
            m_inMultiCapture = false;
            m_activePiece = -1;
            m_visitedPositions.clear();
            m_lastDirection = {0, 0};
            m_turn = (m_turn == 1) ? 2 : 1;

            std::ostringstream oss;
            oss << "Joueur " << player << " capture " << chosenCaptures.size()
                << " pierre(s). Fin de la série.";
            m_lastAction = oss.str();
        }
    } else {
        // Ordinary move (paika)
        m_inMultiCapture = false;
        m_activePiece = -1;
        m_visitedPositions.clear();
        m_lastDirection = {0, 0};
        m_turn = (m_turn == 1) ? 2 : 1;

        std::ostringstream oss;
        oss << "Joueur " << player << " effectue un déplacement ordinaire.";
        m_lastAction = oss.str();
    }

    // Record move metadata for client animation
    m_lastMove.id = ++m_moveCounter;
    m_lastMove.player = player;
    m_lastMove.from = from;
    m_lastMove.to = to;
    m_lastMove.captureType = (chosenCaptures.empty() ? "" : (captureType.empty() ? (hasApp ? "approach" : "withdrawal") : captureType));
    m_lastMove.captured = chosenCaptures;

    // Evaluate victory conditions
    checkGameState();

    return true;
}

void FanoronaGame::handleInput(PlayerId id, const std::string& commandJson) {
    if (m_finished) return;

    // Check if client triggered bot move in solo mode
    if (m_isSolo && (commandJson.find("\"cmd\":\"bot_move\"") != std::string::npos ||
                     commandJson.find("\"bot_move\"") != std::string::npos)) {
        if (m_turn == 2 && !m_finished) {
            executeBotStep();
        }
        return;
    }

    // End voluntary multi-capture series
    if (m_inMultiCapture && (commandJson.find("\"cmd\":\"end_turn\"") != std::string::npos ||
                             commandJson.find("\"end_turn\"") != std::string::npos)) {
        m_inMultiCapture = false;
        m_activePiece = -1;
        m_visitedPositions.clear();
        m_lastDirection = {0, 0};
        m_turn = (m_turn == 1) ? 2 : 1;
        m_lastAction = "Fin volontaire de la série de captures.";
        checkGameState();
        return;
    }

    int playerNum = m_isSolo ? 1 : static_cast<int>(id);
    if (playerNum != m_turn) return;

    // 1. Move by from / to
    auto fromPos = commandJson.find("\"from\":");
    auto toPos = commandJson.find("\"to\":");

    if (fromPos != std::string::npos && toPos != std::string::npos) {
        int from = -1;
        int to = -1;
        std::from_chars(commandJson.data() + fromPos + 7, commandJson.data() + commandJson.size(), from);
        std::from_chars(commandJson.data() + toPos + 5, commandJson.data() + commandJson.size(), to);

        std::string capType;
        auto capPos = commandJson.find("\"captureType\":");
        if (capPos != std::string::npos) {
            auto q1 = commandJson.find("\"", capPos + 14);
            if (q1 != std::string::npos) {
                auto q2 = commandJson.find("\"", q1 + 1);
                if (q2 != std::string::npos) {
                    capType = commandJson.substr(q1 + 1, q2 - q1 - 1);
                }
            }
        }

        executeMove(playerNum, from, to, capType);
        return;
    }

    // 2. Restart / Replay command
    if (commandJson.find("\"cmd\":\"restart\"") != std::string::npos ||
        commandJson.find("\"cmd\":\"reset\"") != std::string::npos) {
        init();
        return;
    }
}

bool FanoronaGame::checkAllStonesCaptured(int& winner, std::string& reason) const {
    int p1 = 0;
    int p2 = 0;
    for (int cell : m_board) {
        if (cell == 1) p1++;
        else if (cell == 2) p2++;
    }

    if (p1 == 0) {
        winner = 2;
        reason = "Victoire du Joueur 2 : toutes les pierres du Joueur 1 ont été éliminées.";
        return true;
    }
    if (p2 == 0) {
        winner = 1;
        reason = "Victoire du Joueur 1 : toutes les pierres du Joueur 2 ont été éliminées.";
        return true;
    }
    return false;
}

bool FanoronaGame::checkImmobilization(int player, int& winner, std::string& reason) const {
    if (hasAnyLegalMoves(player)) return false;

    winner = (player == 1) ? 2 : 1;
    if (player == 1) {
        reason = "Victoire du Joueur 2 : Joueur 1 est immobilisé (aucun déplacement légal).";
    } else {
        reason = "Victoire du Joueur 1 : Joueur 2 est immobilisé (aucun déplacement légal).";
    }
    return true;
}

void FanoronaGame::checkGameState() {
    int w = 0;
    std::string r;

    if (checkAllStonesCaptured(w, r)) {
        m_finished = true;
        m_winner = w;
        m_winReason = r;
        m_lastAction = r;
        if (m_listener) m_listener->onGameFinished(static_cast<PlayerId>(m_winner));
        return;
    }

    if (checkImmobilization(m_turn, w, r)) {
        m_finished = true;
        m_winner = w;
        m_winReason = r;
        m_lastAction = r;
        if (m_listener) m_listener->onGameFinished(static_cast<PlayerId>(m_winner));
        return;
    }
}

bool FanoronaGame::executeBotStep() {
    if (m_finished || m_turn != 2) return false;

    struct CandidateMove {
        int from;
        int to;
        std::string capType;
        int capturesCount;
        int score;
    };

    std::vector<CandidateMove> candidates;

    if (m_inMultiCapture) {
        // Must continue with active piece
        auto validMoves = getValidSuccessiveMoves(m_activePiece);
        for (int to : validMoves) {
            auto app = getApproachCaptures(m_activePiece, to);
            auto wit = getWithdrawalCaptures(m_activePiece, to);

            if (!app.empty()) {
                int score = static_cast<int>(app.size()) * 10 + (isStrongNode(to) ? 3 : 0);
                candidates.push_back({m_activePiece, to, "approach", static_cast<int>(app.size()), score});
            }
            if (!wit.empty()) {
                int score = static_cast<int>(wit.size()) * 10 + (isStrongNode(to) ? 3 : 0);
                candidates.push_back({m_activePiece, to, "withdrawal", static_cast<int>(wit.size()), score});
            }
        }
    } else {
        bool capturesAvailable = hasAnyLegalCaptures(2);

        for (int from = 0; from < NUM_INTERSECTIONS; ++from) {
            if (m_board[from] != 2) continue;

            for (const auto& edge : m_adj[from]) {
                int to = edge.to;
                if (m_board[to] != 0) continue;

                auto app = getApproachCaptures(from, to);
                auto wit = getWithdrawalCaptures(from, to);

                if (capturesAvailable) {
                    if (!app.empty()) {
                        int score = static_cast<int>(app.size()) * 10 + (isStrongNode(to) ? 3 : 0);
                        candidates.push_back({from, to, "approach", static_cast<int>(app.size()), score});
                    }
                    if (!wit.empty()) {
                        int score = static_cast<int>(wit.size()) * 10 + (isStrongNode(to) ? 3 : 0);
                        candidates.push_back({from, to, "withdrawal", static_cast<int>(wit.size()), score});
                    }
                } else {
                    // Ordinary move
                    int score = (isStrongNode(to) ? 2 : 0) + (4 - std::abs(to / COLS - 2)) + (4 - std::abs(to % COLS - 4));
                    candidates.push_back({from, to, "", 0, score});
                }
            }
        }
    }

    if (candidates.empty()) {
        checkGameState();
        return false;
    }

    // Pick highest scored move
    auto bestIt = std::max_element(candidates.begin(), candidates.end(),
        [](const CandidateMove& a, const CandidateMove& b) {
            return a.score < b.score;
        });

    return executeMove(2, bestIt->from, bestIt->to, bestIt->capType);
}

void FanoronaGame::executeBotTurn() {
    // Autonomous decision loop for bot (Player 2)
    while (!m_finished && m_turn == 2) {
        if (!executeBotStep()) break;
    }
}

std::string FanoronaGame::serializeState() const {
    int p1Count = 0;
    int p2Count = 0;
    for (int p : m_board) {
        if (p == 1) p1Count++;
        else if (p == 2) p2Count++;
    }

    std::ostringstream oss;
    oss << "{\"type\":\"state\",\"game\":\"fanorona\""
        << ",\"turn\":" << m_turn
        << ",\"finished\":" << (m_finished ? "true" : "false")
        << ",\"winner\":" << m_winner
        << ",\"scores\":[" << p1Count << "," << p2Count << "]"
        << ",\"p1_score\":" << p1Count
        << ",\"p2_score\":" << p2Count
        << ",\"reason\":\"" << m_winReason << "\""
        << ",\"last_action\":\"" << m_lastAction << "\""
        << ",\"p1_count\":" << p1Count
        << ",\"p2_count\":" << p2Count
        << ",\"in_multi\":" << (m_inMultiCapture ? "true" : "false")
        << ",\"active_piece\":" << m_activePiece
        << ",\"visited\":[";

    for (size_t i = 0; i < m_visitedPositions.size(); ++i) {
        if (i > 0) oss << ",";
        oss << m_visitedPositions[i];
    }
    oss << "],\"must_capture\":" << (hasAnyLegalCaptures(m_turn) ? "true" : "false")
        << ",\"last_move\":{";

    if (m_lastMove.from >= 0 && m_lastMove.to >= 0) {
        oss << "\"id\":" << m_lastMove.id
            << ",\"player\":" << m_lastMove.player
            << ",\"from\":" << m_lastMove.from
            << ",\"to\":" << m_lastMove.to
            << ",\"type\":\"" << m_lastMove.captureType << "\""
            << ",\"captured\":[";
        for (size_t i = 0; i < m_lastMove.captured.size(); ++i) {
            if (i > 0) oss << ",";
            oss << m_lastMove.captured[i];
        }
        oss << "]}";
    } else {
        oss << "\"id\":0,\"player\":0,\"from\":-1,\"to\":-1,\"type\":\"\",\"captured\":[]}";
    }

    oss << ",\"board\":[";

    for (size_t i = 0; i < m_board.size(); ++i) {
        if (i > 0) oss << ",";
        oss << m_board[i];
    }
    oss << "]}";

    return oss.str();
}

} // namespace core
} // namespace gamehub
