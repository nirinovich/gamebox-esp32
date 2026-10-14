#pragma once

#include <memory>
#include <string>
#include <gamehub/core/IGamePhaseState.hpp>

namespace gamehub {
namespace core {

class KatroGame;
class IKatroCommand;

class IKatroState : public IGamePhaseState {
public:
    ~IKatroState() override = default;

    void onEnter(Game& game) override;
    bool handleCommand(Game& game, PlayerId playerId, const ICommand& command) override;
    void update(Game& game, float dt) override;

    virtual void onEnterKatro(KatroGame& /*game*/) {}
    virtual bool handleKatroCommand(KatroGame& /*game*/, PlayerId /*playerId*/, const IKatroCommand& /*command*/) { return false; }
    virtual void updateKatro(KatroGame& /*game*/, float /*dt*/) {}
};

class KatroTurnState : public IKatroState {
public:
    void onEnterKatro(KatroGame& game) override;
    bool handleKatroCommand(KatroGame& game, PlayerId playerId, const IKatroCommand& command) override;
    void updateKatro(KatroGame& game, float dt) override;
    std::string getPhaseName() const override { return "TURN_ACTIVE"; }
};

class KatroGameOverState : public IKatroState {
public:
    void onEnterKatro(KatroGame& game) override;
    bool handleKatroCommand(KatroGame& /*game*/, PlayerId /*playerId*/, const IKatroCommand& /*command*/) override {
        return false;
    }
    void updateKatro(KatroGame& /*game*/, float /*dt*/) override {}
    std::string getPhaseName() const override { return "GAME_OVER"; }
};

} // namespace core
} // namespace gamehub
