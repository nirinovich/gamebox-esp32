#pragma once

#include <memory>
#include <string>
#include <cstdint>

namespace gamehub {
namespace core {

class Room;

class Session {
public:
    explicit Session(uint32_t id);
    virtual ~Session();

    uint32_t getId() const { return m_id; }
    std::string handleMessage(const std::string& rawMessage);

    Room* getActiveRoom() const { return m_room.get(); }
    void setRoom(std::unique_ptr<Room> room);

private:
    uint32_t m_id;
    std::unique_ptr<Room> m_room;
};

} // namespace core
} // namespace gamehub
