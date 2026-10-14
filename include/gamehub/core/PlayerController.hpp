#pragma once

#include <memory>
#include <queue>
#include <string>
#include "GameTypes.hpp"
#include "ICommand.hpp"

namespace gamehub {
namespace core {

class Game;

/**
 * @brief GoF Strategy Pattern interface for player decision-making.
 * Decouples game rules from input sources (human network action streams vs autonomous bot heuristics).
 */
class PlayerController {
public:
    virtual ~PlayerController() = default;

    /**
     * @brief Compute or dequeue the next action for this player.
     * @return A command to execute, or nullptr if no action is pending.
     */
    virtual std::unique_ptr<ICommand> decideAction(const Game& game, PlayerId id) = 0;

    /**
     * @brief Whether this controller acts autonomously without human network inputs.
     */
    virtual bool isAutonomous() const = 0;

    /**
     * @brief Queue a command received from the network session (used by HumanController).
     */
    virtual void queueCommand(std::unique_ptr<ICommand> /*cmd*/) {}
};

/**
 * @brief Strategy implementation for connected human clients.
 * Consumes commands pushed into its internal FIFO queue from incoming network sessions.
 */
class HumanController : public PlayerController {
public:
    HumanController() = default;
    ~HumanController() override = default;

    bool isAutonomous() const override { return false; }

    void queueCommand(std::unique_ptr<ICommand> cmd) override {
        if (cmd) {
            m_commandQueue.push(std::move(cmd));
        }
    }

    std::unique_ptr<ICommand> decideAction(const Game& /*game*/, PlayerId /*id*/) override {
        if (m_commandQueue.empty()) {
            return nullptr;
        }
        auto cmd = std::move(m_commandQueue.front());
        m_commandQueue.pop();
        return cmd;
    }

    bool hasPendingAction() const {
        return !m_commandQueue.empty();
    }

    void clearQueue() {
        std::queue<std::unique_ptr<ICommand>> empty;
        std::swap(m_commandQueue, empty);
    }

private:
    std::queue<std::unique_ptr<ICommand>> m_commandQueue;
};

/**
 * @brief Base strategy implementation for autonomous bot controllers.
 * Subclasses implement game-specific search heuristics (e.g. Minimax, greedy pip evaluation, capture maximizing).
 */
class BotController : public PlayerController {
public:
    BotController() = default;
    ~BotController() override = default;

    bool isAutonomous() const override { return true; }
};

} // namespace core
} // namespace gamehub
