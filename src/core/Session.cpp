#include <gamehub/core/Session.hpp>
#include <gamehub/core/Room.hpp>
#include <gamehub/core/INetworkAdapter.hpp>
#include <sstream>

namespace gamehub {
namespace core {

Session::Session(uint32_t id, INetworkAdapter* network)
    : m_id(id), m_network(network) {}

Session::~Session() = default;

void Session::joinRoom(Room* room, uint32_t seat) {
    m_ownedRoom.reset();
    m_room = room;
    m_seat = seat;
}

void Session::leaveRoom() {
    m_ownedRoom.reset();
    m_room = nullptr;
    m_seat = 1;
}

void Session::setRoom(Room* room) {
    m_ownedRoom.reset();
    m_room = room;
    m_seat = 1;
}

void Session::setRoom(std::unique_ptr<Room> room) {
    m_ownedRoom = std::move(room);
    m_room = m_ownedRoom.get();
    m_seat = 1;
}

void Session::send(const std::string& payload) const {
    if (m_network) {
        m_network->sendToClient(m_id, payload);
    }
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
        std::string pong = oss.str();
        send(pong);
        return pong;
    }

    // 2. Delegate in-game inputs to active Room
    if (m_room && !m_room->isFinished()) {
        m_room->handleInput(m_seat, rawMessage);
        std::string reply = m_room->serializeStateForPlayer(m_seat);
        if (m_room->getMaxPlayers() == 1) {
            send(reply);
        }
        return reply;
    }

    return "{\"type\":\"ack\"}";
}

} // namespace core
} // namespace gamehub
