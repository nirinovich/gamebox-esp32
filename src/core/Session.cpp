#include <gamehub/core/Session.hpp>
#include <gamehub/core/Room.hpp>
#include <sstream>

namespace gamehub {
namespace core {

Session::Session(uint32_t id) : m_id(id) {}

Session::~Session() = default;

void Session::setRoom(Room* room) {
    m_ownedRoom.reset();
    m_room = room;
}

void Session::setRoom(std::unique_ptr<Room> room) {
    m_ownedRoom = std::move(room);
    m_room = m_ownedRoom.get();
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

    // 2. Delegate in-game inputs to active Room
    if (m_room) {
        m_room->handleInput(1, rawMessage);
        return m_room->serializeState();
    }

    return "{\"type\":\"ack\"}";
}

} // namespace core
} // namespace gamehub
