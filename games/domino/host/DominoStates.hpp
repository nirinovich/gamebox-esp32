#pragma once

#include <memory>
#include <string>
#include <gamehub/core/IGamePhaseState.hpp>

namespace gamehub {
namespace core {

class DominoGame;

class DominoPlayState : public IGamePhaseState {
public:
    void onEnter(Game& game) override;
    bool handleCommand(Game& game, PlayerId playerId, const ICommand& command) override;
    void update(Game& game, float dt) override;
    std::string getPhaseName() const override { return "ROUND_ACTIVE"; }
};

class DominoPendingChoiceState : public IGamePhaseState {
public:
    DominoPendingChoiceState(int playerIndex, int tileIndex)
        : m_playerIndex(playerIndex), m_tileIndex(tileIndex) {}

    void onEnter(Game& game) override;
    bool handleCommand(Game& game, PlayerId playerId, const ICommand& command) override;
    std::string getPhaseName() const override { return "PENDING_CHOICE"; }

    int getPlayerIndex() const { return m_playerIndex; }
    int getTileIndex() const { return m_tileIndex; }

private:
    int m_playerIndex;
    int m_tileIndex;
};

class DominoRoundOverState : public IGamePhaseState {
public:
    void onEnter(Game& game) override;
    bool handleCommand(Game& game, PlayerId playerId, const ICommand& command) override;
    std::string getPhaseName() const override { return "ROUND_FINISHED"; }
};

class DominoGameOverState : public IGamePhaseState {
public:
    void onEnter(Game& game) override;
    bool handleCommand(Game& /*game*/, PlayerId /*playerId*/, const ICommand& /*command*/) override {
        return false;
    }
    std::string getPhaseName() const override { return "GAME_OVER"; }
};

} // namespace core
} // namespace gamehub
