#pragma once

#include <memory>
#include <string>
#include <gamehub/core/IGamePhaseState.hpp>

namespace gamehub {
namespace core {

class LudoGame;

class ILudoState : public IGamePhaseState {
public:
    virtual ~ILudoState() = default;

    virtual void onEnter(LudoGame& /*game*/) {}
    virtual void onExit(LudoGame& /*game*/) {}
    virtual bool handleCommand(LudoGame& game, PlayerId playerId, const ILudoCommand& command) = 0;
    virtual void update(LudoGame& /*game*/, float /*dt*/) {}

    // Adapter for IGamePhaseState
    void onEnter(Game& game) override;
    void onExit(Game& game) override;
    bool handleCommand(Game& game, PlayerId playerId, const ICommand& command) override;
    void update(Game& game, float dt) override;
};

class LudoWaitingRollState : public ILudoState {
public:
    void onEnter(LudoGame& game) override;
    bool handleCommand(LudoGame& game, PlayerId playerId, const ILudoCommand& command) override;
    void update(LudoGame& game, float dt) override;
    std::string getPhaseName() const override { return "WAITING_ROLL"; }
};

class LudoSelectPawnState : public ILudoState {
public:
    void onEnter(LudoGame& game) override;
    bool handleCommand(LudoGame& game, PlayerId playerId, const ILudoCommand& command) override;
    void update(LudoGame& game, float dt) override;
    std::string getPhaseName() const override { return "SELECT_PAWN"; }
};

class LudoGameOverState : public ILudoState {
public:
    void onEnter(LudoGame& game) override;
    bool handleCommand(LudoGame& /*game*/, PlayerId /*playerId*/, const ILudoCommand& /*command*/) override {
        return false;
    }
    void update(LudoGame& /*game*/, float /*dt*/) override {}
    std::string getPhaseName() const override { return "GAME_OVER"; }
};

} // namespace core
} // namespace gamehub
