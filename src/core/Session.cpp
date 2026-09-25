#include <gamehub/core/Session.hpp>
#include <gamehub/core/Room.hpp>
#include <sstream>

namespace gamehub {
namespace core {

Session::Session(uint32_t id) : m_id(id) {}

Session::~Session() = default;

void Session::setRoom(std::unique_ptr<Room> room) {
    m_room = std::move(room);
}

std::string Session::handleMessage(const std::string& rawMessage) {
    // 1. Handle Ping
    if (rawMessage.find("\"ping\"") != std::string::npos) {
        std::string ts = "0";
        auto tsPos = rawMessage.find("\"ts\":");
        if (tsPos != std::string::npos) {
            auto start = tsPos + 5;
            auto end = rawMessage.find_first_of(",}", start);
            if (end != std::string::npos) {
                ts = rawMessage.substr(start, end - start);
            } else {
                ts = rawMessage.substr(start);
            }
        }
        std::ostringstream oss;
        oss << "{\"type\":\"pong\",\"ts\":" << ts << "}";
        return oss.str();
    }

    // 2. Handle Game Start (Solo Snake & Solo TicTacToe & Solo Connect 4)
    if (rawMessage.find("\"start\"") != std::string::npos) {
        if (rawMessage.find("\"snake_solo\"") != std::string::npos) {
            m_room = std::make_unique<Room>("SOLO_SNAKE", GameType::SNAKE_SOLO);
            m_room->init();
            return m_room->serializeState();
        } else if (rawMessage.find("\"ttt_solo\"") != std::string::npos) {
            m_room = std::make_unique<Room>("SOLO_TTT", GameType::TICTACTOE_SOLO);
            m_room->init();
            return m_room->serializeState();
        } else if (rawMessage.find("\"connect4_solo\"") != std::string::npos) {
            m_room = std::make_unique<Room>("SOLO_C4", GameType::CONNECT_FOUR_SOLO);
            m_room->init();
            return m_room->serializeState();
        } else if (rawMessage.find("\"domino_solo\"") != std::string::npos) {
            m_room = std::make_unique<Room>("SOLO_DOMINO", GameType::DOMINO_SOLO);
            m_room->init();
            return m_room->serializeState();
        }
    }

    // 3. Handle Direction Input
    if (rawMessage.find("\"dir\"") != std::string::npos && m_room) {
        m_room->handleInput(1, rawMessage);
        return m_room->serializeState();
    }

    // 4. Handle Move Placement (TTT / Battleship / Domino)
    if ((rawMessage.find("\"place\"") != std::string::npos ||
         rawMessage.find("\"play\"") != std::string::npos ||
         rawMessage.find("\"draw\"") != std::string::npos ||
         rawMessage.find("\"pass\"") != std::string::npos) && m_room) {
        m_room->handleInput(1, rawMessage);
        return m_room->serializeState();
    }

    // 5. Handle Connect 4 Drop
    if (rawMessage.find("\"drop\"") != std::string::npos && m_room) {
        m_room->handleInput(1, rawMessage);
        return m_room->serializeState();
    }

    // 6. Handle Battleship Fire & Auto Place
    if ((rawMessage.find("\"fire\"") != std::string::npos || rawMessage.find("\"auto_place\"") != std::string::npos) && m_room) {
        m_room->handleInput(1, rawMessage);
        return m_room->serializeState();
    }

    // 7. Handle Simulation Tick
    if (rawMessage.find("\"tick\"") != std::string::npos && m_room) {
        m_room->tick(0.066f);
        return m_room->serializeState();
    }

    return "{\"type\":\"ack\"}";
}

} // namespace core
} // namespace gamehub
