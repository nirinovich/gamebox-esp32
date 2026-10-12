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
    void setName(const std::string& name) { m_name = name; }
    const std::string& getName() const { return m_name; }

    Room* getActiveRoom() const { return m_room; }
    void setRoom(Room* room);
    void setRoom(std::unique_ptr<Room> room);

    std::string handleMessage(const std::string& rawMessage);

private:
    uint32_t m_id;
    std::string m_name{"Guest"};
    Room* m_room{nullptr};
    std::unique_ptr<Room> m_ownedRoom{nullptr};
};

} // namespace core
} // namespace gamehub
