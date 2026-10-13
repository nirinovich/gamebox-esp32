#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#ifdef _WIN32
#include <crtdbg.h>
#endif
#include <gamehub/core/HostEngine.hpp>
#include <gamehub/core/INetworkAdapter.hpp>

class MockNetwork : public gamehub::core::INetworkAdapter {
public:
    struct Msg {
        uint32_t sid;
        std::string text;
    };
    std::vector<Msg> sent;
    std::vector<std::string> bcast;

    void sendToClient(uint32_t sid, const std::string& message) override {
        sent.push_back({sid, message});
    }
    void broadcast(const std::string& message) override {
        bcast.push_back(message);
    }
    void clear() {
        sent.clear();
        bcast.clear();
    }
    std::string lastFor(uint32_t sid) const {
        for (auto it = sent.rbegin(); it != sent.rend(); ++it) {
            if (it->sid == sid) return it->text;
        }
        return "";
    }
};

#define EXPECT(cond, msg) \
    if (!(cond)) { \
        std::cerr << "[FAIL] Line " << __LINE__ << ": " << msg << std::endl; \
        return 1; \
    }

int main() {
#ifdef _WIN32
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
#endif
    std::cout << "[DIAGNOSIS] Running Domino 2P and 3P test harness..." << std::endl;
    MockNetwork net;
    gamehub::core::HostEngine engine(&net, 4);

    // Scenario 1: Standard frontend creation (game="domino_pvp", no max specified)
    std::cout << "--- Test 1: Frontend create room ('domino_pvp' without max) ---" << std::endl;
    engine.onClientConnect(1);
    engine.onClientConnect(2);
    net.clear();

    engine.onClientMessage(1, "{\"cmd\":\"create\",\"game\":\"domino_pvp\",\"host\":\"Alice\"}");
    std::string res1 = net.lastFor(1);
    std::cout << "Create response for P1: " << res1 << std::endl;
    auto codePos = res1.find("\"code\":\"");
    EXPECT(codePos != std::string::npos, "codePos found");
    std::string code = res1.substr(codePos + 8, 4);

    net.clear();
    engine.onClientMessage(2, "{\"cmd\":\"join\",\"code\":\"" + code + "\"}");
    std::string res2 = net.lastFor(2);
    std::cout << "Join response for P2: " << res2 << std::endl;

    // Check if room started for 2 players
    bool p1GotReady = false, p2GotReady = false;
    for (const auto& m : net.sent) {
        if (m.sid == 1 && m.text.find("room_ready") != std::string::npos) p1GotReady = true;
        if (m.sid == 2 && m.text.find("room_ready") != std::string::npos) p2GotReady = true;
    }
    std::cout << "Did 2P room start automatically? P1 ready: " << p1GotReady << ", P2 ready: " << p2GotReady << std::endl;

    // Now Alice presses "Start Match Now (2 Players)"
    net.clear();
    std::cout << "Alice sends start_early..." << std::endl;
    engine.onClientMessage(1, "{\"cmd\":\"start_early\"}");
    for (const auto& m : net.sent) {
        std::cout << "Sent to " << m.sid << ": " << m.text.substr(0, std::min<size_t>(m.text.size(), 80)) << std::endl;
    }

    // Now Alice tries to play a tile
    net.clear();
    std::cout << "Alice plays tile 0..." << std::endl;
    engine.onClientMessage(1, "{\"cmd\":\"play\",\"index\":0,\"end\":\"right\"}");
    std::string stateAfterPlay = net.lastFor(1);
    std::cout << "State after play for P1: " << stateAfterPlay.substr(0, std::min<size_t>(stateAfterPlay.size(), 120)) << std::endl;
    EXPECT(stateAfterPlay.find("\"turn\":2") != std::string::npos, "Alice's play should advance turn to 2");
    EXPECT(stateAfterPlay.find("\"left_end\":-1") == std::string::npos, "Board should have left_end set after play");

    // Scenario 2: 3-Player Domino Game
    std::cout << "\n--- Test 2: 3-Player Domino Game ---" << std::endl;
    engine.onClientConnect(10);
    engine.onClientConnect(20);
    engine.onClientConnect(30);
    net.clear();

    engine.onClientMessage(10, "{\"cmd\":\"create\",\"game\":\"domino_pvp\",\"host\":\"P10\"}");
    std::string res10 = net.lastFor(10);
    std::string code3 = res10.substr(res10.find("\"code\":\"") + 8, 4);

    engine.onClientMessage(20, "{\"cmd\":\"join\",\"code\":\"" + code3 + "\"}");
    engine.onClientMessage(30, "{\"cmd\":\"join\",\"code\":\"" + code3 + "\"}");

    std::cout << "Total messages sent on P3 join: " << net.sent.size() << std::endl;
    for (const auto& m : net.sent) {
        std::cout << "Msg to " << m.sid << ": " << m.text.substr(0, std::min<size_t>(m.text.size(), 100)) << std::endl;
    }

    // P1 plays
    net.clear();
    engine.onClientMessage(10, "{\"cmd\":\"play\",\"index\":0,\"end\":\"right\"}");
    std::cout << "P10 played. Messages sent: " << net.sent.size() << std::endl;
    for (const auto& m : net.sent) {
        std::cout << "P10 play msg to " << m.sid << ": " << m.text.substr(0, std::min<size_t>(m.text.size(), 80)) << std::endl;
    }

    // Scenario 3: Explicit 2-Player Domino Duel
    std::cout << "\n--- Test 3: Explicit 2-Player Domino Duel ('domino_duel') ---" << std::endl;
    engine.onClientConnect(40);
    engine.onClientConnect(50);
    net.clear();

    engine.onClientMessage(40, "{\"cmd\":\"create\",\"game\":\"domino_duel\",\"host\":\"Duelist1\"}");
    std::string res40 = net.lastFor(40);
    EXPECT(res40.find("\"max\":2") != std::string::npos, "Duel room should have max 2");
    std::string codeDuel = res40.substr(res40.find("\"code\":\"") + 8, 4);

    net.clear();
    engine.onClientMessage(50, "{\"cmd\":\"join\",\"code\":\"" + codeDuel + "\"}");
    
    // In 2P duel, joining should immediately start the room!
    bool duelReady40 = false, duelReady50 = false;
    for (const auto& m : net.sent) {
        if (m.sid == 40 && m.text.find("room_ready") != std::string::npos) duelReady40 = true;
        if (m.sid == 50 && m.text.find("room_ready") != std::string::npos) duelReady50 = true;
    }
    EXPECT(duelReady40, "P40 should receive room_ready automatically on 2nd join");
    EXPECT(duelReady50, "P50 should receive room_ready automatically on 2nd join");

    // P40 plays immediately
    net.clear();
    engine.onClientMessage(40, "{\"cmd\":\"play\",\"index\":0,\"end\":\"right\"}");
    std::string stateDuelAfterPlay = net.lastFor(40);
    EXPECT(stateDuelAfterPlay.find("\"turn\":2") != std::string::npos, "P40 play should advance turn to 2");
    EXPECT(stateDuelAfterPlay.find("\"left_end\":-1") == std::string::npos, "Board left_end should be set");

    std::cout << "[DIAGNOSIS] Completed all scenarios without crashing!" << std::endl;
    return 0;
}
