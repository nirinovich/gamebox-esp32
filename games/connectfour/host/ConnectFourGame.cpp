#include "ConnectFourGame.hpp"
#include <gamehub/core/GameRegistry.hpp>
#include <sstream>
#include <algorithm>
#include <limits>
#include <cstdlib>
#include <charconv>

namespace {
std::unique_ptr<gamehub::core::Game> createConnect4Solo() {
    return std::make_unique<gamehub::core::ConnectFourGame>(true);
}
std::unique_ptr<gamehub::core::Game> createConnect4Pvp() {
    return std::make_unique<gamehub::core::ConnectFourGame>(false);
}
}

REGISTER_GAME("connect4_solo", Connect4Solo, &createConnect4Solo, {
    gamehub::core::GameId("connect4_solo"),
    "Connect Four Bot",
    "SOLO",
    "Gravity disc drops with physical elastic bounce",
    1, 1,
    gamehub::core::InputFlag::GRID_POINTER | gamehub::core::InputFlag::ACTION_BUTTONS,
    "connect4_solo",
    false
})

REGISTER_GAME("connect4_pvp", Connect4Pvp, &createConnect4Pvp, {
    gamehub::core::GameId("connect4_pvp"),
    "Connect Four Duel",
    "DUEL",
    "Vertical four-in-a-row column targeting showdown",
    2, 2,
    gamehub::core::InputFlag::GRID_POINTER | gamehub::core::InputFlag::ACTION_BUTTONS,
    "connect4_pvp",
    true
})

namespace gamehub {
namespace core {

ConnectFourGame::ConnectFourGame(bool vsBot, std::unique_ptr<IConnectFourStrategy> botStrategy)
    : m_vsBot(vsBot), m_botStrategy(std::move(botStrategy)) {
    if (m_vsBot && !m_botStrategy) {
        m_botStrategy = std::make_unique<ConnectFourMinimaxStrategy>();
    }
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
    auto [ptr, ec] = std::from_chars(input.data() + pos + 6, input.data() + input.size(), col);
    if (ec != std::errc{}) return;
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

// ---- Bot Strategy Delegation -----------------------------------------

int ConnectFourGame::bestBotCol() {
    if (m_botStrategy) {
        return m_botStrategy->computeMove(m_board, 2);
    }
    return 3;
}

} // namespace core
} // namespace gamehub
