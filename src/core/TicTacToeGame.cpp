#include <gamehub/core/TicTacToeGame.hpp>
#include <sstream>

namespace gamehub {
namespace core {

TicTacToeGame::TicTacToeGame(bool isSolo)
    : m_isSolo(isSolo), m_bot(isSolo ? std::make_unique<BotController>() : nullptr) {}

void TicTacToeGame::init() {
    m_board.fill(0);
    m_turn = 1;
    m_finished = false;
    m_winner = 0;
}

void TicTacToeGame::update(float /*dt*/) {
    // In solo mode, bot can also take its turn if pending
    if (m_isSolo && !m_finished && m_turn == 2 && m_bot) {
        int bestMove = m_bot->getMove(m_board, 2);
        if (bestMove >= 0 && bestMove < 9 && m_board[bestMove] == 0) {
            m_board[bestMove] = 2;
            checkGameState();
            if (!m_finished) {
                m_turn = 1;
            }
        }
    }
}

void TicTacToeGame::handleInput(PlayerId id, const std::string& commandJson) {
    if (m_finished) return;

    auto cellPos = commandJson.find("\"cell\":");
    if (cellPos == std::string::npos) return;

    int cell = -1;
    try {
        cell = std::stoi(commandJson.substr(cellPos + 7));
    } catch (...) {
        return;
    }

    if (cell < 0 || cell >= 9 || m_board[cell] != 0) return;

    if (m_isSolo) {
        // Player 1 move
        if (m_turn == 1) {
            m_board[cell] = 1;
            checkGameState();
            if (!m_finished) {
                m_turn = 2;
                // Bot executes immediately for responsive turn-based play
                if (m_bot) {
                    int botMove = m_bot->getMove(m_board, 2);
                    if (botMove >= 0 && botMove < 9 && m_board[botMove] == 0) {
                        m_board[botMove] = 2;
                        checkGameState();
                        if (!m_finished) {
                            m_turn = 1;
                        }
                    }
                }
            }
        }
    } else {
        // Multiplayer PvP move
        if (id == static_cast<PlayerId>(m_turn)) {
            m_board[cell] = m_turn;
            checkGameState();
            if (!m_finished) {
                m_turn = (m_turn == 1) ? 2 : 1;
            }
        }
    }
}

bool TicTacToeGame::checkWinFor(int mark) const {
    const int wins[8][3] = {
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, // rows
        {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, // cols
        {0, 4, 8}, {2, 4, 6}             // diagonals
    };

    for (auto& w : wins) {
        if (m_board[w[0]] == mark && m_board[w[1]] == mark && m_board[w[2]] == mark) {
            return true;
        }
    }
    return false;
}

bool TicTacToeGame::isBoardFull() const {
    for (int cell : m_board) {
        if (cell == 0) return false;
    }
    return true;
}

void TicTacToeGame::checkGameState() {
    if (checkWinFor(1)) {
        m_finished = true;
        m_winner = 1;
    } else if (checkWinFor(2)) {
        m_finished = true;
        m_winner = 2;
    } else if (isBoardFull()) {
        m_finished = true;
        m_winner = -1; // Draw
    }
}

std::string TicTacToeGame::serializeState() const {
    std::ostringstream oss;
    oss << "{\"type\":\"state\",\"game\":\"tictactoe\",\"turn\":" << m_turn
        << ",\"finished\":" << (m_finished ? "true" : "false")
        << ",\"winner\":" << m_winner
        << ",\"board\":[";

    for (size_t i = 0; i < m_board.size(); ++i) {
        if (i > 0) oss << ",";
        oss << m_board[i];
    }
    oss << "]}";
    return oss.str();
}

} // namespace core
} // namespace gamehub
