#include <gamehub/core/Room.hpp>
#include <algorithm>
#include <sstream>

namespace gamehub {
namespace core {

Room::Room(const std::string& code, GameId gameId, const std::string& hostName, size_t maxPlayers)
    : m_code(code), m_hostName(hostName.empty() ? "Player 1" : hostName),
      m_gameId(gameId),
      m_maxPlayers(maxPlayers),
      m_game(GameRegistry::instance().create(gameId.view())),
      m_state(std::make_unique<WaitingRoomState>()) {
    if (m_maxPlayers == 0) {
        const auto* reg = GameRegistry::instance().find(gameId.view());
        if (reg) {
            m_maxPlayers = reg->descriptor.maxPlayers;
        } else {
            m_maxPlayers = 2;
        }
    }
    if (m_game && m_maxPlayers > 1) {
        m_game->setPlayerCount(static_cast<int>(m_maxPlayers));
    }
    if (m_game) {
        m_game->setEventListener(this);
    }
}

void Room::onGameFinished(PlayerId winner) {
    if (winner == 1) m_p1SeriesWins++;
    else if (winner == 2) m_p2SeriesWins++;
    m_seriesGameCount++;
    transitionTo(std::make_unique<FinishedRoomState>());
}

void Room::onRoundFinished(PlayerId /*roundWinner*/, int /*pointsAwarded*/) {
    // Observable round completion hook
}

void Room::onStateInvalidated() {
    // Observable state mutation hook
}

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
    }
}

void Room::handleInput(PlayerId id, const std::string& commandJson) {
    if (m_game && canProcessInput()) {
        m_game->handleInput(id, commandJson);
    }
}

std::string Room::serializeState() const {
    if (m_game) {
        std::string s = m_game->serializeState();
        if (s.size() > 1 && s.back() == '}') {
            s.pop_back();
            std::ostringstream oss;
            oss << ",\"series_p1\":" << m_p1SeriesWins
                << ",\"series_p2\":" << m_p2SeriesWins
                << ",\"series_match\":" << m_seriesGameCount << "}";
            s += oss.str();
        }
        return s;
    }
    return "{}";
}

std::string Room::serializeStateForPlayer(PlayerId id) const {
    if (m_game) {
        std::string s = m_game->serializeStateForPlayer(id);
        if (s.size() > 1 && s.back() == '}') {
            s.pop_back();
            std::ostringstream oss;
            oss << ",\"series_p1\":" << m_p1SeriesWins
                << ",\"series_p2\":" << m_p2SeriesWins
                << ",\"series_match\":" << m_seriesGameCount << "}";
            s += oss.str();
        }
        return s;
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

int Room::getPlayerSeat(PlayerId id) const {
    for (size_t i = 0; i < m_players.size(); ++i) {
        if (m_players[i] == id) {
            return static_cast<int>(i + 1);
        }
    }
    return 1;
}

void Room::startEarly() {
    if (m_players.size() >= 2 && m_players.size() < m_maxPlayers) {
        m_maxPlayers = m_players.size();
        if (m_game) {
            m_game->setPlayerCount(static_cast<int>(m_maxPlayers));
        }
        transitionTo(std::make_unique<ActiveRoomState>());
    }
}

void Room::removePlayer(PlayerId id) {
    m_players.erase(std::remove(m_players.begin(), m_players.end(), id), m_players.end());
    if (m_players.empty()) {
        transitionTo(std::make_unique<DissolvedRoomState>());
    }
}

void Room::requestRematch(PlayerId id) {
    if (std::find(m_rematchRequests.begin(), m_rematchRequests.end(), id) == m_rematchRequests.end()) {
        m_rematchRequests.push_back(id);
    }
}

bool Room::isRematchReady() const {
    return !m_players.empty() && m_rematchRequests.size() >= m_players.size();
}

void Room::resetForRematch() {
    m_rematchRequests.clear();
    if (m_game) {
        m_game->init();
    }
    transitionTo(std::make_unique<ActiveRoomState>());
}

} // namespace core
} // namespace gamehub

