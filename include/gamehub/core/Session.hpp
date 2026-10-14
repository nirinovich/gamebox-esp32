#pragma once

#include <memory>
#include <string>
#include <cstdint>

namespace gamehub {
namespace core {

class Room;
class INetworkAdapter;

class Session {
public:
    explicit Session(uint32_t id, INetworkAdapter* network = nullptr);
    virtual ~Session();

    uint32_t getId() const { return m_id; }
    void setName(const std::string& name) { m_name = name; }
    const std::string& getName() const { return m_name; }

    void setNetworkAdapter(INetworkAdapter* adapter) { m_network = adapter; }
    INetworkAdapter* getNetworkAdapter() const { return m_network; }

    Room* getActiveRoom() const { return m_room; }
    uint32_t getSeat() const { return m_seat; }

    void joinRoom(Room* room, uint32_t seat);
    void leaveRoom();

    void setRoom(Room* room);
    void setRoom(std::unique_ptr<Room> room);

    void send(const std::string& payload) const;

    std::string handleMessage(const std::string& rawMessage);

private:
    uint32_t m_id;
    std::string m_name{"Guest"};
    Room* m_room{nullptr};
    uint32_t m_seat{1};
    INetworkAdapter* m_network{nullptr};
    std::unique_ptr<Room> m_ownedRoom{nullptr};
};

} // namespace core
} // namespace gamehub
