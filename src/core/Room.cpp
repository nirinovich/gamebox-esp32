#include <gamehub/core/Room.hpp>
#include <algorithm>

namespace gamehub {
namespace core {

Room::Room(const std::string& code, GameType type, const std::string& hostName, size_t maxPlayers)
    : m_code(code), m_hostName(hostName.empty() ? "Player 1" : hostName), m_type(type),
      m_maxPlayers(type == GameType::DOMINO_PVP ? std::clamp<size_t>(maxPlayers, 2, 3) : 2),
      m_game(GameFactory::create(type)) {}

void Room::init() {
    if (m_game) {
        m_game->init();
    }
}

void Room::tick(float dt) {
    if (m_game) {
        m_game->update(dt);
    }
}

void Room::handleInput(PlayerId id, const std::string& commandJson) {
    if (m_game) {
        m_game->handleInput(id, commandJson);
    }
}

std::string Room::serializeState() const {
    if (m_game) {
        return m_game->serializeState();
    }
    return "{}";
}

bool Room::isFinished() const {
    return m_game ? m_game->isFinished() : true;
}

int Room::getWinner() const {
    return m_game ? m_game->getWinner() : 0;
}

void Room::addPlayer(PlayerId id) {
    if (std::find(m_players.begin(), m_players.end(), id) == m_players.end()) {
        m_players.push_back(id);
    }
}

void Room::removePlayer(PlayerId id) {
    m_players.erase(std::remove(m_players.begin(), m_players.end(), id), m_players.end());
}

} // namespace core
} // namespace gamehub
