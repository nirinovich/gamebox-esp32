#include <iostream>
#undef NDEBUG
#include <cassert>
#include <string_view>
#include <gamehub/core/Game.hpp>
#include <gamehub/core/GameRegistry.hpp>

namespace {

class MockGame : public gamehub::core::Game {
public:
    void init() override { initialized = true; }
    void update(float dt) override { lastDt = dt; }
    void handleInput(gamehub::core::PlayerId id, const std::string& commandJson) override {
        lastPlayerId = id;
        lastCommand = commandJson;
    }
    std::string serializeState() const override { return "{\"mock\":true}"; }
    bool isFinished() const override { return finished; }
    int getWinner() const override { return 1; }

    bool initialized{false};
    float lastDt{0.0f};
    gamehub::core::PlayerId lastPlayerId{0};
    std::string lastCommand;
    bool finished{false};
};

std::unique_ptr<gamehub::core::Game> createMockGame() {
    return std::make_unique<MockGame>();
}

} // namespace

int main() {
    std::cout << "[TEST] Running GameRegistry & GameId unit tests..." << std::endl;

    using namespace gamehub::core;

    // 1. Verify GameId zero-heap value semantics
    static_assert(sizeof(GameId) == 24, "GameId must be exactly 24 bytes fixed stack array");
    GameId gid("test_game_id");
    assert(gid.view() == "test_game_id");
    assert(std::string_view(gid.c_str()) == "test_game_id");
    assert(gid == "test_game_id");
    assert(gid == GameId("test_game_id"));
    assert(gid != GameId("other_game"));

    // 2. Register mock game
    GameDescriptor desc{};
    desc.id = gid;
    desc.title = "Mock Adventure";
    desc.category = "SOLO";
    desc.minPlayers = 1;
    desc.maxPlayers = 1;
    desc.inputTemplate = InputFlag::DPAD | InputFlag::ACTION_BUTTONS;

    bool registered = GameRegistry::instance().registerGame(desc, &createMockGame);
    assert(registered);

    // 3. Find and inspect descriptor
    const auto* entry = GameRegistry::instance().find("test_game_id");
    assert(entry != nullptr);
    assert(entry->descriptor.id == "test_game_id");
    assert(std::string_view(entry->descriptor.title) == "Mock Adventure");
    assert(entry->descriptor.minPlayers == 1);
    assert(hasInputFlag(entry->descriptor.inputTemplate, InputFlag::DPAD));
    assert(hasInputFlag(entry->descriptor.inputTemplate, InputFlag::ACTION_BUTTONS));
    assert(!hasInputFlag(entry->descriptor.inputTemplate, InputFlag::GRID_POINTER));

    // 4. Create and exercise polymorphic instance
    auto game = GameRegistry::instance().create("test_game_id");
    assert(game != nullptr);
    game->init();
    game->update(0.05f);
    assert(game->serializeState() == "{\"mock\":true}");

    // 5. Query non-existent game
    assert(GameRegistry::instance().find("non_existent_game") == nullptr);
    assert(GameRegistry::instance().create("non_existent_game") == nullptr);

    std::cout << "[TEST] GameRegistry & GameId unit tests PASSED!" << std::endl;
    return 0;
}
