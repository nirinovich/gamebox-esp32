#include <gamehub/core/Room.hpp>
#include <algorithm>

namespace gamehub {
namespace core {

Room::Room(const std::string& code, GameType type, const std::string& hostName, size_t maxPlayers)
    : m_code(code), m_hostName(hostName.empty() ? "Player 1" : hostName), m_type(type),
      m_maxPlayers(type == GameType::DOMINO_PVP ? std::clamp<size_t>(maxPlayers, 2, 3) : 
                   (type == GameType::SNAKE_SOLO || type == GameType::TICTACTOE_SOLO ||
                    type == GameType::CONNECT_FOUR_SOLO || type == GameType::DOMINO_SOLO ||
                    type == GameType::PONG_SOLO || type == GameType::TRON_SOLO ||
                    type == GameType::BATTLESHIP_SOLO ? 1 : 2)),
      m_game(GameFactory::create(type)),
      m_state(std::make_unique<WaitingRoomState>()) {}

void Room::init() {
    if (m_game) {
        m_game->init();
    }
}

void Room::transitionTo(std::unique_ptr<RoomState> newState) {
    if (newState) {
        m_state = std::move(newState);
    }
}

RoomLifecycle Room::getLifecycle() const {
    return m_state ? m_state->getLifecycle() : RoomLifecycle::DISSOLVED;
}

const char* Room::getStateName() const {
    return m_state ? m_state->getName() : "DISSOLVED";
}

bool Room::canAcceptPlayer() const {
    return m_state ? m_state->canAcceptPlayer(*this) : false;
}

bool Room::canStartGame() const {
    return m_state ? m_state->canStartGame(*this) : false;
}

bool Room::canProcessInput() const {
    return m_state ? m_state->canProcessInput(*this) : false;
}

bool Room::shouldTick() const {
    return m_state ? m_state->shouldTick(*this) : false;
}

void Room::tick(float dt) {
    if (m_game && shouldTick()) {
        m_game->update(dt);
        if (m_game->isFinished()) {
            transitionTo(std::make_unique<FinishedRoomState>());
        }
    }
}

void Room::handleInput(PlayerId id, const std::string& commandJson) {
    if (m_game && canProcessInput()) {
        m_game->handleInput(id, commandJson);
        if (m_game->isFinished()) {
            transitionTo(std::make_unique<FinishedRoomState>());
        }
    }
}

std::string Room::serializeState() const {
    if (m_game) {
        return m_game->serializeState();
    }
    return "{}";
}

std::string Room::serializeStateForPlayer(PlayerId id) const {
    if (m_game) {
        return m_game->serializeStateForPlayer(id);
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
        if (m_players.size() >= m_maxPlayers) {
            transitionTo(std::make_unique<ActiveRoomState>());
        }
    }
}

void Room::removePlayer(PlayerId id) {
    m_players.erase(std::remove(m_players.begin(), m_players.end(), id), m_players.end());
    if (m_players.empty()) {
        transitionTo(std::make_unique<DissolvedRoomState>());
    }
}

} // namespace core
} // namespace gamehub
