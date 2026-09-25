#include <gamehub/core/BattleshipGame.hpp>
#include <sstream>
#include <cstring>

namespace gamehub {
namespace core {

constexpr int BattleshipGame::FLEET[5];

BattleshipGame::BattleshipGame() {
    init();
}

void BattleshipGame::init() {
    for (int i = 0; i < 2; ++i) {
        for (auto& row : m_grids[i].ships) row.fill(0);
        for (auto& row : m_grids[i].shots) row.fill(false);
        m_grids[i].hits = 0;
        m_grids[i].totalShipCells = 0;
        m_grids[i].ready = false;
        m_nextShipId[i] = 1;
    }
    m_turn = 1;
    m_finished = false;
    m_winner = 0;
    m_phase = Phase::PLACEMENT;
}

void BattleshipGame::update(float /*dt*/) {}

bool BattleshipGame::placeShip(int pidx, int shipId, int row, int col, int len, bool horiz) {
    if (row < 0 || col < 0) return false;
    if (horiz && col + len > SIZE) return false;
    if (!horiz && row + len > SIZE) return false;
    // Check overlap
    for (int i = 0; i < len; ++i) {
        int r = horiz ? row : row + i;
        int c = horiz ? col + i : col;
        if (r >= SIZE || c >= SIZE) return false;
        if (m_grids[pidx].ships[r][c] != 0) return false;
    }
    for (int i = 0; i < len; ++i) {
        int r = horiz ? row : row + i;
        int c = horiz ? col + i : col;
        m_grids[pidx].ships[r][c] = shipId;
        m_grids[pidx].totalShipCells++;
    }
    return true;
}

bool BattleshipGame::handlePlacement(int pidx, const std::string& input) {
    // input: {"cmd":"place","row":R,"col":C,"len":L,"dir":"H"/"V"}
    auto getInt = [&](const std::string& key) -> int {
        std::string needle = "\"" + key + "\":";
        auto pos = input.find(needle);
        if (pos == std::string::npos) return -1;
        try { return std::stoi(input.substr(pos + needle.size())); } catch (...) { return -1; }
    };
    int row = getInt("row");
    int col = getInt("col");
    int len = getInt("len");
    bool horiz = input.find("\"H\"") != std::string::npos || input.find("\"dir\":\"H\"") != std::string::npos;

    // Validate length against expected ship
    int fleetIndex = m_nextShipId[pidx] - 1;
    if (fleetIndex >= 5) return false; // all ships placed
    if (len != FLEET[fleetIndex]) return false;

    if (!placeShip(pidx, m_nextShipId[pidx], row, col, len, horiz)) return false;
    m_nextShipId[pidx]++;

    // All 5 ships placed?
    if (m_nextShipId[pidx] > 5) {
        m_grids[pidx].ready = true;
        if (m_grids[0].ready && m_grids[1].ready) {
            m_phase = Phase::BATTLE;
        }
    }
    return true;
}

bool BattleshipGame::handleAutoPlace(int pidx) {
    for (auto& row : m_grids[pidx].ships) row.fill(0);
    m_grids[pidx].totalShipCells = 0;
    m_nextShipId[pidx] = 1;
    for (int shipIdx = 0; shipIdx < 5; ++shipIdx) {
        int len = FLEET[shipIdx];
        bool placed = false;
        for (int attempts = 0; attempts < 500 && !placed; ++attempts) {
            bool horiz = (std::rand() % 2 == 0);
            int r = std::rand() % (horiz ? SIZE : (SIZE - len + 1));
            int c = std::rand() % (horiz ? (SIZE - len + 1) : SIZE);
            if (placeShip(pidx, shipIdx + 1, r, c, len, horiz)) {
                placed = true;
            }
        }
        if (!placed) return false;
    }
    m_nextShipId[pidx] = 6;
    m_grids[pidx].ready = true;
    if (m_grids[0].ready && m_grids[1].ready) {
        m_phase = Phase::BATTLE;
    }
    return true;
}

bool BattleshipGame::handleFire(int pidx, const std::string& input) {
    // input: {"cmd":"fire","row":R,"col":C}
    auto getInt = [&](const std::string& key) -> int {
        std::string needle = "\"" + key + "\":";
        auto pos = input.find(needle);
        if (pos == std::string::npos) return -1;
        try { return std::stoi(input.substr(pos + needle.size())); } catch (...) { return -1; }
    };
    int row = getInt("row");
    int col = getInt("col");
    if (row < 0 || col < 0 || row >= SIZE || col >= SIZE) return false;

    int targetIdx = 1 - pidx;
    if (m_grids[targetIdx].shots[row][col]) return false; // already fired

    m_grids[targetIdx].shots[row][col] = true;
    if (m_grids[targetIdx].ships[row][col] != 0) {
        m_grids[targetIdx].hits++;
        if (m_grids[targetIdx].hits >= m_grids[targetIdx].totalShipCells) {
            m_finished = true;
            m_winner = pidx + 1; // 1-indexed
        }
    }
    if (!m_finished) m_turn = (m_turn == 1) ? 2 : 1;
    return true;
}

void BattleshipGame::handleInput(PlayerId player, const std::string& input) {
    if (m_finished) return;
    int pidx = (int)player - 1;
    if (pidx < 0 || pidx > 1) return;

    if (m_phase == Phase::PLACEMENT) {
        if (input.find("\"auto_place\"") != std::string::npos || input.find("\"deploy\"") != std::string::npos) {
            handleAutoPlace(pidx);
        } else if (input.find("\"place\"") != std::string::npos || input.find("\"cmd\":\"place\"") != std::string::npos) {
            handlePlacement(pidx, input);
        }
    } else {
        if ((int)player != m_turn) return;
        if (input.find("\"fire\"") != std::string::npos || input.find("\"cmd\":\"fire\"") != std::string::npos) {
            handleFire(pidx, input);
        }
    }
}

std::string BattleshipGame::serializeGrid(int forPlayer) const {
    // forPlayer is 1-indexed
    int ownIdx    = forPlayer - 1;
    int enemyIdx  = 1 - ownIdx;
    std::ostringstream oss;

    // Own board (ships visible + shots)
    oss << "\"own\":[";
    for (int r = 0; r < SIZE; ++r) {
        for (int c = 0; c < SIZE; ++c) {
            if (r > 0 || c > 0) oss << ",";
            // 0=water,1=ship,2=hit,3=miss
            bool shot = m_grids[ownIdx].shots[r][c];
            int  ship = m_grids[ownIdx].ships[r][c];
            if (shot && ship)  oss << 2;
            else if (shot)     oss << 3;
            else if (ship)     oss << 1;
            else               oss << 0;
        }
    }
    oss << "]";

    // Enemy board (only shots visible)
    oss << ",\"enemy\":[";
    for (int r = 0; r < SIZE; ++r) {
        for (int c = 0; c < SIZE; ++c) {
            if (r > 0 || c > 0) oss << ",";
            bool shot = m_grids[enemyIdx].shots[r][c];
            int  ship = m_grids[enemyIdx].ships[r][c];
            if (shot && ship)  oss << 2; // hit
            else if (shot)     oss << 3; // miss
            else               oss << 0; // unknown
        }
    }
    oss << "]";
    return oss.str();
}

std::string BattleshipGame::serializeState() const {
    std::ostringstream oss;
    oss << "{\"type\":\"state\",\"game\":\"battleship\""
        << ",\"phase\":\"" << (m_phase == Phase::PLACEMENT ? "placement" : "battle") << "\""
        << ",\"turn\":" << m_turn
        << ",\"p1ready\":" << (m_grids[0].ready ? "true" : "false")
        << ",\"p2ready\":" << (m_grids[1].ready ? "true" : "false")
        << ",\"ships_left_p1\":" << (m_grids[0].totalShipCells - m_grids[0].hits)
        << ",\"ships_left_p2\":" << (m_grids[1].totalShipCells - m_grids[1].hits)
        << ",\"finished\":" << (m_finished ? "true" : "false")
        << ",\"winner\":" << m_winner
        // Include both grid perspectives (server-authoritative; client filters)
        << ",\"p1\":{" << serializeGrid(1) << "}"
        << ",\"p2\":{" << serializeGrid(2) << "}"
        << "}";
    return oss.str();
}

} // namespace core
} // namespace gamehub
