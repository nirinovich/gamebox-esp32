#pragma once

#include <memory>
#include <string>
#include <gamehub/core/IGamePhaseState.hpp>

namespace gamehub {
namespace core {

class KatroGame;

class KatroTurnState : public IGamePhaseState {
public:
    void onEnter(Game& game) override;
    bool handleCommand(Game& game, PlayerId playerId, const ICommand& command) override;
    void update(Game& game, float dt) override;
    std::string getPhaseName() const override { return "TURN_ACTIVE"; }
};

class KatroGameOverState : public IGamePhaseState {
public:
    void onEnter(Game& game) override;
    bool handleCommand(Game& /*game*/, PlayerId /*playerId*/, const ICommand& /*command*/) override {
        return false;
    }
    void update(Game& /*game*/, float /*dt*/) override {}
    std::string getPhaseName() const override { return "GAME_OVER"; }
};

} // namespace core
} // namespace gamehub
