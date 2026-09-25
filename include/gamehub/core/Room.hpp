#pragma once

#include <string>
#include <memory>
#include <vector>
#include "GameTypes.hpp"
#include "Game.hpp"
#include "GameFactory.hpp"

namespace gamehub {
namespace core {

class Room {
public:
    Room(const std::string& code, GameType type, const std::string& hostName = "Player 1", size_t maxPlayers = 2);
    virtual ~Room() = default;

    void init();
    void tick(float dt);
    void handleInput(PlayerId id, const std::string& commandJson);
    std::string serializeState() const;
    bool isFinished() const;
    int getWinner() const;

    const std::string& getCode() const { return m_code; }
    const std::string& getHostName() const { return m_hostName; }
    GameType getGameType() const { return m_type; }
    Game* getGame() { return m_game.get(); }

    void addPlayer(PlayerId id);
    void removePlayer(PlayerId id);
    size_t getPlayerCount() const { return m_players.size(); }
    size_t getMaxPlayers() const { return m_maxPlayers; }
    void setMaxPlayers(size_t max) { m_maxPlayers = max; }

private:
    std::string m_code;
    std::string m_hostName;
    GameType m_type;
    size_t m_maxPlayers{2};
    std::unique_ptr<Game> m_game;
    std::vector<PlayerId> m_players;
};

} // namespace core
} // namespace gamehub
