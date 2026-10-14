#pragma once

#include <memory>
#include <string>
#include <gamehub/core/IGamePhaseState.hpp>

namespace gamehub {
namespace core {

class DominoGame;

class IDominoState : public IGamePhaseState {
public:
    virtual ~IDominoState() = default;

    virtual void onEnter(DominoGame& /*game*/) {}
    virtual void onExit(DominoGame& /*game*/) {}
    virtual bool handleCommand(DominoGame& game, PlayerId playerId, const IDominoCommand& command) = 0;
    virtual void update(DominoGame& /*game*/, float /*dt*/) {}

    // Generic adapter hooks for IGamePhaseState
    void onEnter(Game& game) override;
    void onExit(Game& game) override;
    bool handleCommand(Game& game, PlayerId playerId, const ICommand& command) override;
    void update(Game& game, float dt) override;
};

class DominoPlayState : public IDominoState {
public:
    void onEnter(DominoGame& game) override;
    bool handleCommand(DominoGame& game, PlayerId playerId, const IDominoCommand& command) override;
    void update(DominoGame& game, float dt) override;
    std::string getPhaseName() const override { return "ROUND_ACTIVE"; }
};

class DominoPendingChoiceState : public IDominoState {
public:
    DominoPendingChoiceState(int playerIndex, int tileIndex)
        : m_playerIndex(playerIndex), m_tileIndex(tileIndex) {}

    void onEnter(DominoGame& game) override;
    bool handleCommand(DominoGame& game, PlayerId playerId, const IDominoCommand& command) override;
    std::string getPhaseName() const override { return "PENDING_CHOICE"; }

    int getPlayerIndex() const { return m_playerIndex; }
    int getTileIndex() const { return m_tileIndex; }

private:
    int m_playerIndex;
    int m_tileIndex;
};

class DominoRoundOverState : public IDominoState {
public:
    void onEnter(DominoGame& game) override;
    bool handleCommand(DominoGame& game, PlayerId playerId, const IDominoCommand& command) override;
    std::string getPhaseName() const override { return "ROUND_FINISHED"; }
};

class DominoGameOverState : public IDominoState {
public:
    void onEnter(DominoGame& game) override;
    bool handleCommand(DominoGame& /*game*/, PlayerId /*playerId*/, const IDominoCommand& /*command*/) override {
        return false;
    }
    std::string getPhaseName() const override { return "GAME_OVER"; }
};

} // namespace core
} // namespace gamehub
