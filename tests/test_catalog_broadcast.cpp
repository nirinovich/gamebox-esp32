#include <iostream>
#undef NDEBUG
#include <cassert>
#include <string>
#include <vector>
#include <gamehub/core/HostEngine.hpp>
#include <gamehub/core/INetworkAdapter.hpp>
#include <gamehub/core/GameRegistry.hpp>
#include <gamehub/core/Game.hpp>

namespace {

class DummyCatalogGame : public gamehub::core::Game {
public:
    void init() override { inited = true; }
    void update(float) override {}
    void handleInput(gamehub::core::PlayerId, const std::string&) override {}
    std::string serializeState() const override { return "{\"game\":\"laser_duel\",\"active\":true}"; }
    bool isFinished() const override { return false; }
    int getWinner() const override { return 0; }

    bool inited{false};
};

std::unique_ptr<gamehub::core::Game> createDummyCatalogGame() {
    return std::make_unique<DummyCatalogGame>();
}

class TestNetAdapter : public gamehub::core::INetworkAdapter {
public:
    struct Msg {
        uint32_t session;
        std::string payload;
    };
    std::vector<Msg> sent;
    std::vector<std::string> broadcasts;

    void sendToClient(uint32_t session, const std::string& message) override {
        sent.push_back({session, message});
    }

    void broadcast(const std::string& message) override {
        broadcasts.push_back(message);
    }

    void clear() {
        sent.clear();
        broadcasts.clear();
    }
};

} // namespace

int main() {
    std::cout << "[TEST] Running Host Directory Catalog & InputTemplate Broadcast tests..." << std::endl;

    using namespace gamehub::core;

    // 1. Register test game in GameRegistry
    GameDescriptor desc{};
    desc.id = GameId("laser_duel");
    desc.title = "Laser Duel";
    desc.category = "DUEL";
    desc.minPlayers = 2;
    desc.maxPlayers = 2;
    desc.inputTemplate = InputFlag::DPAD | InputFlag::ACTION_BUTTONS;
    desc.isDuel = true;
    GameRegistry::instance().registerGame(desc, &createDummyCatalogGame);

    TestNetAdapter net;
    HostEngine engine(&net, 4);

    // 2. Client connects: should receive directory containing catalog games
    engine.onClientConnect(101);
    assert(!net.sent.empty());
    std::string connectMsg = net.sent.back().payload;
    assert(connectMsg.find("\"type\":\"room_directory\"") != std::string::npos);
    assert(connectMsg.find("\"games\":") != std::string::npos);
    assert(connectMsg.find("\"id\":\"laser_duel\"") != std::string::npos);
    assert(connectMsg.find("\"title\":\"Laser Duel\"") != std::string::npos);
    assert(connectMsg.find("\"dpad\"") != std::string::npos);
    assert(connectMsg.find("\"action_buttons\"") != std::string::npos);

    // 3. Client creates multiplayer room using string GameId
    net.clear();
    std::string createResp = engine.onClientMessage(101, "{\"cmd\":\"create\",\"game\":\"laser_duel\",\"host\":\"Alice\"}");
    assert(createResp.find("\"type\":\"room_created\"") != std::string::npos);
    assert(createResp.find("\"game\":\"laser_duel\"") != std::string::npos);

    // 4. Verify room exists in RoomManager
    Room* r = engine.getRoomManager().findRoomByPlayer(101);
    assert(r != nullptr);
    assert(r->getGameId() == "laser_duel");
    assert(r->getGame() != nullptr);

    // 5. Client starts solo room with GameId
    net.clear();
    std::string startResp = engine.onClientMessage(102, "{\"cmd\":\"start\",\"game\":\"laser_duel\"}");
    assert(startResp.find("\"game\":\"laser_duel\"") != std::string::npos);
    Room* soloRoom = engine.getRoomManager().findRoomByPlayer(102);
    assert(soloRoom != nullptr);
    assert(soloRoom->getGameId() == "laser_duel");

    std::cout << "[TEST] Host Directory Catalog & InputTemplate Broadcast tests PASSED!" << std::endl;
    return 0;
}
