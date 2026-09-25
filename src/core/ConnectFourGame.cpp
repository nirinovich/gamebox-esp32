#include <gamehub/core/ConnectFourGame.hpp>
#include <sstream>
#include <algorithm>
#include <limits>
#include <cstdlib>

namespace gamehub {
namespace core {

static constexpr int BOT_DEPTH = 5;

ConnectFourGame::ConnectFourGame(bool vsBot) : m_vsBot(vsBot) {
    init();
}

void ConnectFourGame::init() {
    for (auto& col : m_board) col.fill(0);
    m_turn = 1;
    m_finished = false;
    m_winner = 0;
}

void ConnectFourGame::update(float /*dt*/) {
    if (!m_finished && m_vsBot && m_turn == 2) {
        int col = bestBotCol();
        dropPiece(col, 2);
    }
}

bool ConnectFourGame::dropPiece(int col, int player) {
    if (col < 0 || col >= COLS) return false;
    // Find lowest empty row in column
    for (int r = 0; r < ROWS; ++r) {
        if (m_board[col][r] == 0) {
            m_board[col][r] = player;
            if (checkWin(player)) {
                m_finished = true;
                m_winner = player;
            } else if (isBoardFull()) {
                m_finished = true;
                m_winner = -1;
            } else {
                m_turn = (m_turn == 1) ? 2 : 1;
            }
            return true;
        }
    }
    return false; // column full
}

bool ConnectFourGame::checkWin(int p) const {
    // Horizontal
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c <= COLS - 4; ++c)
            if (m_board[c][r]==p && m_board[c+1][r]==p && m_board[c+2][r]==p && m_board[c+3][r]==p) return true;
    // Vertical
    for (int c = 0; c < COLS; ++c)
        for (int r = 0; r <= ROWS - 4; ++r)
            if (m_board[c][r]==p && m_board[c][r+1]==p && m_board[c][r+2]==p && m_board[c][r+3]==p) return true;
    // Diagonal /
    for (int c = 0; c <= COLS - 4; ++c)
        for (int r = 0; r <= ROWS - 4; ++r)
            if (m_board[c][r]==p && m_board[c+1][r+1]==p && m_board[c+2][r+2]==p && m_board[c+3][r+3]==p) return true;
    // Diagonal backslash
    for (int c = 0; c <= COLS - 4; ++c)
        for (int r = 3; r < ROWS; ++r)
            if (m_board[c][r]==p && m_board[c+1][r-1]==p && m_board[c+2][r-2]==p && m_board[c+3][r-3]==p) return true;
    return false;
}

bool ConnectFourGame::isBoardFull() const {
    for (int c = 0; c < COLS; ++c)
        if (m_board[c][ROWS-1] == 0) return false;
    return true;
}

void ConnectFourGame::handleInput(PlayerId player, const std::string& input) {
    if (m_finished) return;
    // Expect input: {"cmd":"drop","col":N}
    auto pos = input.find("\"col\":");
    if (pos == std::string::npos) return;
    int col = -1;
    try { col = std::stoi(input.substr(pos + 6)); } catch (...) { return; }
    if ((int)player == m_turn) {
        if (dropPiece(col, m_turn)) {
            if (m_vsBot && !m_finished && m_turn == 2) {
                int botCol = bestBotCol();
                dropPiece(botCol, 2);
            }
        }
    }
}

std::string ConnectFourGame::serializeState() const {
    std::ostringstream oss;
    oss << "{\"type\":\"state\",\"game\":\"connect4\""
        << ",\"turn\":" << m_turn
        << ",\"finished\":" << (m_finished ? "true" : "false")
        << ",\"winner\":" << m_winner
        << ",\"board\":[";
    // Emit as flat array row-major from top (row ROWS-1) to bottom (row 0), left to right
    for (int r = ROWS - 1; r >= 0; --r) {
        for (int c = 0; c < COLS; ++c) {
            if (r < ROWS-1 || c > 0) oss << ",";
            oss << m_board[c][r];
        }
    }
    oss << "]}";
    return oss.str();
}

// ---- Minimax -----------------------------------------------------------

int ConnectFourGame::scoreWindow(int a, int b, int c, int d, int p) const {
    int opp = (p == 1) ? 2 : 1;
    int pCount = (a==p)+(b==p)+(c==p)+(d==p);
    int eCount = (a==0)+(b==0)+(c==0)+(d==0);
    int oCount = (a==opp)+(b==opp)+(c==opp)+(d==opp);
    if (oCount > 0) return 0;
    if (pCount == 4) return 100;
    if (pCount == 3 && eCount == 1) return 5;
    if (pCount == 2 && eCount == 2) return 2;
    return 0;
}

int ConnectFourGame::heuristicScore(int p) const {
    int score = 0;
    // Prefer centre column
    for (int r = 0; r < ROWS; ++r) if (m_board[COLS/2][r] == p) score += 3;
    // Horizontal windows
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c <= COLS-4; ++c)
            score += scoreWindow(m_board[c][r],m_board[c+1][r],m_board[c+2][r],m_board[c+3][r],p);
    // Vertical
    for (int c = 0; c < COLS; ++c)
        for (int r = 0; r <= ROWS-4; ++r)
            score += scoreWindow(m_board[c][r],m_board[c][r+1],m_board[c][r+2],m_board[c][r+3],p);
    // Diagonal /
    for (int c = 0; c <= COLS-4; ++c)
        for (int r = 0; r <= ROWS-4; ++r)
            score += scoreWindow(m_board[c][r],m_board[c+1][r+1],m_board[c+2][r+2],m_board[c+3][r+3],p);
    // Diagonal backslash
    for (int c = 0; c <= COLS-4; ++c)
        for (int r = 3; r < ROWS; ++r)
            score += scoreWindow(m_board[c][r],m_board[c+1][r-1],m_board[c+2][r-2],m_board[c+3][r-3],p);
    return score;
}

int ConnectFourGame::minimax(int depth, int alpha, int beta, bool maximizing) {
    if (checkWin(2)) return 1000 + depth;
    if (checkWin(1)) return -(1000 + depth);
    if (isBoardFull() || depth == 0) return heuristicScore(2) - heuristicScore(1);

    if (maximizing) {
        int best = std::numeric_limits<int>::min();
        for (int c = 0; c < COLS; ++c) {
            int r = -1;
            for (int row = 0; row < ROWS; ++row) if (m_board[c][row] == 0) { r = row; break; }
            if (r == -1) continue;
            m_board[c][r] = 2;
            best = std::max(best, minimax(depth-1, alpha, beta, false));
            m_board[c][r] = 0;
            alpha = std::max(alpha, best);
            if (alpha >= beta) break;
        }
        return best;
    } else {
        int best = std::numeric_limits<int>::max();
        for (int c = 0; c < COLS; ++c) {
            int r = -1;
            for (int row = 0; row < ROWS; ++row) if (m_board[c][row] == 0) { r = row; break; }
            if (r == -1) continue;
            m_board[c][r] = 1;
            best = std::min(best, minimax(depth-1, alpha, beta, true));
            m_board[c][r] = 0;
            beta = std::min(beta, best);
            if (alpha >= beta) break;
        }
        return best;
    }
}

int ConnectFourGame::bestBotCol() {
    int bestScore = std::numeric_limits<int>::min();
    int bestCol = 3; // default centre
    for (int c = 0; c < COLS; ++c) {
        int r = -1;
        for (int row = 0; row < ROWS; ++row) if (m_board[c][row] == 0) { r = row; break; }
        if (r == -1) continue;
        m_board[c][r] = 2;
        int score = minimax(BOT_DEPTH, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), false);
        m_board[c][r] = 0;
        if (score > bestScore) { bestScore = score; bestCol = c; }
    }
    return bestCol;
}

} // namespace core
} // namespace gamehub
