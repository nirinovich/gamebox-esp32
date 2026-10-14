#pragma once

#include <string>
#include "GameTypes.hpp"
#include "GameEventListener.hpp"

namespace gamehub {
namespace core {

class Game {
public:
    virtual ~Game() = default;

    virtual void init() = 0;
    virtual void update(float dt) = 0;
    virtual void handleInput(PlayerId id, const std::string& commandJson) = 0;
    virtual std::string serializeState() const = 0;
    virtual std::string serializeStateForPlayer(PlayerId /*id*/) const { return serializeState(); }
    virtual bool isFinished() const = 0;
    virtual int getWinner() const = 0;
    virtual void setPlayerCount(int /*count*/) {}
    virtual bool isRealTime() const { return false; }
    virtual int getMoveSeq() const { return 0; }

    virtual void setEventListener(class GameEventListener* listener) { m_listener = listener; }
    class GameEventListener* getEventListener() const { return m_listener; }

protected:
    class GameEventListener* m_listener{nullptr};
};

} // namespace core
} // namespace gamehub
