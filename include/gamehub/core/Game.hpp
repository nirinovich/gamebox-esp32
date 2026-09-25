#pragma once

#include <string>
#include "GameTypes.hpp"

namespace gamehub {
namespace core {

class Game {
public:
    virtual ~Game() = default;

    virtual void init() = 0;
    virtual void update(float dt) = 0;
    virtual void handleInput(PlayerId id, const std::string& commandJson) = 0;
    virtual std::string serializeState() const = 0;
    virtual bool isFinished() const = 0;
    virtual int getWinner() const = 0;
};

} // namespace core
} // namespace gamehub
