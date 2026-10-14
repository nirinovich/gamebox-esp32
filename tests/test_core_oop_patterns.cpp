#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <gamehub/core/Game.hpp>
#include <gamehub/core/ICommand.hpp>
#include <gamehub/core/IGamePhaseState.hpp>
#include <gamehub/core/PlayerController.hpp>
#include <gamehub/core/Player.hpp>

namespace {

// Mock game to verify core OOP patterns
class MockGame : public gamehub::core::Game {
public:
    MockGame() = default;

    void init() override {
        m_initialized = true;
    }

    void update(float dt) override {
        if (m_state) {
            m_state->update(*this, dt);
        }
    }

    void handleInput(gamehub::core::PlayerId id, const std::string& /*commandJson*/) override {
        // In the new architecture, handleInput will deserialize via factory
        m_lastPlayerId = id;
    }

    std::string serializeState() const override {
        return "{\"mock\":true}";
    }

    bool isFinished() const override { return m_finished; }
    int getWinner() const override { return m_winner; }

    void transitionTo(std::unique_ptr<gamehub::core::IGamePhaseState> nextState) {
        if (m_state) {
            m_state->onExit(*this);
        }
        m_state = std::move(nextState);
        if (m_state) {
            m_state->onEnter(*this);
        }
    }

    gamehub::core::IGamePhaseState* getActiveState() const {
        return m_state.get();
    }

    bool executeCommand(gamehub::core::PlayerId id, const gamehub::core::ICommand& cmd) {
        if (!m_state) return false;
        return m_state->handleCommand(*this, id, cmd);
    }

    int testValue{0};
    bool stateEntered{false};
    bool stateExited{false};

private:
    bool m_initialized{false};
    bool m_finished{false};
    int m_winner{0};
    gamehub::core::PlayerId m_lastPlayerId{0};
    std::unique_ptr<gamehub::core::IGamePhaseState> m_state;
};

// Mock Command
class IncrementCommand : public gamehub::core::ICommand {
public:
    explicit IncrementCommand(int delta) : m_delta(delta) {}

    bool execute(gamehub::core::Game& game, gamehub::core::PlayerId id) const override {
        auto* mock = dynamic_cast<MockGame*>(&game);
        if (!mock) return false;
        return mock->executeCommand(id, *this);
    }

    std::string getType() const override { return "increment"; }
    int getDelta() const { return m_delta; }

private:
    int m_delta;
};

// Mock State
class ActivePhaseState : public gamehub::core::IGamePhaseState {
public:
    void onEnter(gamehub::core::Game& game) override {
        auto* mock = dynamic_cast<MockGame*>(&game);
        if (mock) mock->stateEntered = true;
    }

    void onExit(gamehub::core::Game& game) override {
        auto* mock = dynamic_cast<MockGame*>(&game);
        if (mock) mock->stateExited = true;
    }

    bool handleCommand(gamehub::core::Game& game, gamehub::core::PlayerId /*playerId*/, const gamehub::core::ICommand& command) override {
        if (command.getType() == "increment") {
            const auto* incCmd = dynamic_cast<const IncrementCommand*>(&command);
            if (incCmd) {
                auto* mock = dynamic_cast<MockGame*>(&game);
                if (mock) {
                    mock->testValue += incCmd->getDelta();
                    return true;
                }
            }
        }
        return false;
    }

    void update(gamehub::core::Game& game, float dt) override {
        auto* mock = dynamic_cast<MockGame*>(&game);
        if (mock) mock->testValue += static_cast<int>(dt);
    }

    std::string getPhaseName() const override { return "ACTIVE_PHASE"; }
};

// Mock Command Factory
class MockCommandFactory : public gamehub::core::CommandFactory {
public:
    std::unique_ptr<gamehub::core::ICommand> createCommand(const std::string& jsonString) const override {
        if (jsonString.find("\"cmd\":\"increment\"") != std::string::npos) {
            return std::make_unique<IncrementCommand>(5);
        }
        return nullptr;
    }
};

// Mock Bot Controller
class DeterministicBotController : public gamehub::core::BotController {
public:
    std::unique_ptr<gamehub::core::ICommand> decideAction(const gamehub::core::Game& /*game*/, gamehub::core::PlayerId /*id*/) override {
        return std::make_unique<IncrementCommand>(10);
    }
};

} // namespace

int main() {
    std::cout << "[TEST] Running Core OOP Patterns tests..." << std::endl;

    // Test 1: State Pattern lifecycle and double dispatch
    {
        MockGame game;
        game.init();

        auto state = std::make_unique<ActivePhaseState>();
        game.transitionTo(std::move(state));

        assert(game.stateEntered == true);
        assert(game.getActiveState() != nullptr);
        assert(game.getActiveState()->getPhaseName() == "ACTIVE_PHASE");

        IncrementCommand cmd(7);
        bool success = cmd.execute(game, 1);
        assert(success == true);
        assert(game.testValue == 7);

        game.update(3.0f);
        assert(game.testValue == 10);

        game.transitionTo(nullptr);
        assert(game.stateExited == true);
    }

    // Test 2: Command Factory deserialization
    {
        MockCommandFactory factory;
        auto validCmd = factory.createCommand("{\"cmd\":\"increment\"}");
        assert(validCmd != nullptr);
        assert(validCmd->getType() == "increment");

        auto invalidCmd = factory.createCommand("{\"cmd\":\"unknown\"}");
        assert(invalidCmd == nullptr);
    }

    // Test 3: Strategy Pattern & Player entity
    {
        auto humanCtrl = std::make_unique<gamehub::core::HumanController>();
        auto* rawHuman = humanCtrl.get();
        gamehub::core::Player humanPlayer(1, "Alice", std::move(humanCtrl));

        assert(!humanPlayer.isAutonomous());
        assert(rawHuman->decideAction(MockGame{}, 1) == nullptr);

        rawHuman->queueCommand(std::make_unique<IncrementCommand>(3));
        assert(rawHuman->hasPendingAction());

        auto action = rawHuman->decideAction(MockGame{}, 1);
        assert(action != nullptr);
        assert(action->getType() == "increment");
        assert(!rawHuman->hasPendingAction());

        auto botCtrl = std::make_unique<DeterministicBotController>();
        gamehub::core::Player botPlayer(2, "Bot-1", std::move(botCtrl));

        assert(botPlayer.isAutonomous());
        auto botAction = botPlayer.getController()->decideAction(MockGame{}, 2);
        assert(botAction != nullptr);
        assert(botAction->getType() == "increment");
    }

    std::cout << "[SUCCESS] All Core OOP Patterns tests passed!" << std::endl;
    return 0;
}
