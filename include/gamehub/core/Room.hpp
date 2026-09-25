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
    Room(const std::string& code, GameType type);
    virtual ~Room() = default;

    void init();
    void tick(float dt);
    void handleInput(PlayerId id, const std::string& commandJson);
    std::string serializeState() const;
    bool isFinished() const;
    int getWinner() const;

    const std::string& getCode() const { return m_code; }
    GameType getGameType() const { return m_type; }

    void addPlayer(PlayerId id);
    void removePlayer(PlayerId id);
    size_t getPlayerCount() const { return m_players.size(); }

private:
    std::string m_code;
    GameType m_type;
    std::unique_ptr<Game> m_game;
    std::vector<PlayerId> m_players;
};

} // namespace core
} // namespace gamehub
