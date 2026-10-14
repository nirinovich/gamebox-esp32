#pragma once

#include <string>
#include <memory>
#include <vector>
#include "GameTypes.hpp"
#include "Game.hpp"
#include "GameRegistry.hpp"
#include "RoomState.hpp"

namespace gamehub {
namespace core {

class Room {
public:
    Room(const std::string& code, GameId gameId, const std::string& hostName = "Player 1", size_t maxPlayers = 2);
    virtual ~Room() = default;

    void init();
    void tick(float dt);
    void handleInput(PlayerId id, const std::string& commandJson);
    std::string serializeState() const;
    std::string serializeStateForPlayer(PlayerId id) const;
    bool isFinished() const;
    int getWinner() const;

    const std::string& getCode() const { return m_code; }
    const std::string& getHostName() const { return m_hostName; }
    const GameId& getGameId() const { return m_gameId; }
    Game* getGame() { return m_game.get(); }
    int getMoveSeq() const { return m_game ? m_game->getMoveSeq() : 0; }

    void addPlayer(PlayerId id);
    void removePlayer(PlayerId id);
    size_t getPlayerCount() const { return m_players.size(); }
    size_t getMaxPlayers() const { return m_maxPlayers; }
    void setMaxPlayers(size_t max) { m_maxPlayers = max; }
    const std::vector<PlayerId>& getPlayers() const { return m_players; }

    // GoF State Pattern methods
    RoomLifecycle getLifecycle() const;
    const char* getStateName() const;
    void transitionTo(std::unique_ptr<RoomState> newState);
    bool canAcceptPlayer() const;
    bool canStartGame() const;
    bool canProcessInput() const;
    bool shouldTick() const;
    void startEarly();
    // Rematch & Series Score
    void requestRematch(PlayerId id);
    bool isRematchReady() const;
    void resetForRematch();
    size_t getRematchRequestCount() const { return m_rematchRequests.size(); }
    int getP1SeriesWins() const { return m_p1SeriesWins; }
    int getP2SeriesWins() const { return m_p2SeriesWins; }
    int getSeriesGameCount() const { return m_seriesGameCount; }
    int getSeriesMatchCount() const { return getSeriesGameCount(); }

private:
    std::string m_code;
    std::string m_hostName;
    GameId m_gameId{};
    size_t m_maxPlayers{2};
    std::unique_ptr<Game> m_game;
    std::vector<PlayerId> m_players;
    std::unique_ptr<RoomState> m_state;
    int m_p1SeriesWins{0};
    int m_p2SeriesWins{0};
    int m_seriesGameCount{0};
    std::vector<PlayerId> m_rematchRequests;
};

} // namespace core
} // namespace gamehub
