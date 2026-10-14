#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#ifdef _WIN32
#include <crtdbg.h>
#endif
#include <gamehub/core/HostEngine.hpp>
#include <gamehub/core/INetworkAdapter.hpp>
#include <gamehub/core/Room.hpp>

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

    // Check who has opening lead
    int leadSid = (res2.find("\"turn\":2") != std::string::npos) ? 2 : 1;
    net.clear();
    std::cout << "Player " << leadSid << " plays tile 0..." << std::endl;
    engine.onClientMessage(leadSid, "{\"cmd\":\"play\",\"index\":0,\"end\":\"right\"}");
    std::string stateAfterPlay = net.lastFor(leadSid);
    std::cout << "State after play: " << stateAfterPlay.substr(0, std::min<size_t>(stateAfterPlay.size(), 120)) << std::endl;
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

    // Whoever has opening lead plays
    std::string p10State = net.lastFor(10);
    int leadSid2 = 10;
    if (p10State.find("\"turn\":2") != std::string::npos) leadSid2 = 20;
    else if (p10State.find("\"turn\":3") != std::string::npos) leadSid2 = 30;

    net.clear();
    engine.onClientMessage(leadSid2, "{\"cmd\":\"play\",\"index\":0,\"end\":\"right\"}");
    std::cout << "Lead played. Messages sent: " << net.sent.size() << std::endl;

    // Scenario 3: Explicit 2-Player Domino Duel ('domino_duel')
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

    // Whoever has lead plays
    std::string p40State = net.lastFor(40);
    int leadSid3 = (p40State.find("\"turn\":2") != std::string::npos) ? 50 : 40;
    net.clear();
    engine.onClientMessage(leadSid3, "{\"cmd\":\"play\",\"index\":0,\"end\":\"right\"}");
    std::string stateDuelAfterPlay = net.lastFor(leadSid3);
    EXPECT(stateDuelAfterPlay.find("\"left_end\":-1") == std::string::npos, "Board left_end should be set");

    // Scenario 4: Domino Solo vs Bot Execution
    std::cout << "\n--- Test 4: Domino Solo vs Bot ('domino_solo') ---" << std::endl;
    engine.onClientConnect(60);
    net.clear();
    engine.onClientMessage(60, "{\"cmd\":\"start\",\"game\":\"domino_solo\"}");
    std::string soloInit = net.lastFor(60);
    EXPECT(!soloInit.empty(), "Client should receive initial state for solo domino");

    gamehub::core::Room* soloRoom = engine.getRoomManager().findRoomByPlayer(60);
    EXPECT(soloRoom != nullptr, "Solo room must exist");
    EXPECT(soloRoom->getGame() != nullptr, "Solo room game must exist");
    EXPECT(soloRoom->getGame()->isRealTime(), "DominoGame must be real-time so HostEngine can tick bots");

    // Advance engine ticks. If Bot has opening lead, Bot should play autonomously!
    net.clear();
    for (int t = 0; t < 20; ++t) {
        engine.tick(0.066f);
    }

    std::string stateAfterTicks = net.lastFor(60);
    if (soloInit.find("\"turn\":2") != std::string::npos) {
        // Bot had opening turn: Bot must have played!
        EXPECT(!stateAfterTicks.empty(), "State must have been broadcast after Bot move");
        EXPECT(stateAfterTicks.find("\"board\":[]") == std::string::npos, "Board must not be empty after Bot opening move");
        EXPECT(stateAfterTicks.find("\"turn\":1") != std::string::npos, "Turn must advance to Player 1 after Bot move");
    } else {
        // Player 1 had opening turn: Player 1 plays, then Bot ticks and plays
        net.clear();
        engine.onClientMessage(60, "{\"cmd\":\"play\",\"index\":0,\"end\":\"right\"}");
        std::string p1PlayState = net.lastFor(60);
        EXPECT(p1PlayState.find("\"turn\":2") != std::string::npos, "Turn must advance to Bot (2) after P1 play");
        net.clear();
        for (int t = 0; t < 20; ++t) {
            engine.tick(0.066f);
        }
        std::string botPlayState = net.lastFor(60);
        EXPECT(!botPlayState.empty(), "State must be sent to client after Bot ticks and moves");
        EXPECT(botPlayState.find("\"turn\":1") != std::string::npos, "Turn must return to Player 1 after Bot move");
    }

    // Scenario 5: Catalog Check (No Poker, No Duplicate Games)
    std::cout << "\n--- Test 5: Catalog Check (No Poker, No Duplicates) ---" << std::endl;
    std::string cat = engine.getRoomManager().serializeCatalog();
    EXPECT(cat.find("poker") == std::string::npos, "Catalog must not contain poker");
    EXPECT(cat.find("Texas Hold") == std::string::npos, "Catalog must not contain Texas Hold'em");

    // Check duplicate titles per category
    std::vector<std::string> ids;
    size_t pos = 0;
    while ((pos = cat.find("\"id\":\"", pos)) != std::string::npos) {
        size_t endPos = cat.find("\"", pos + 6);
        std::string id = cat.substr(pos + 6, endPos - (pos + 6));
        for (const auto& existing : ids) {
            EXPECT(existing != id, "Duplicate game ID in catalog: " + id);
        }
        ids.push_back(id);
        pos = endPos;
    }

    std::vector<std::pair<std::string, std::string>> titleCats; // {title, category}
    pos = 0;
    while ((pos = cat.find("{\"id\":\"", pos)) != std::string::npos) {
        size_t titlePos = cat.find("\"title\":\"", pos);
        size_t titleEnd = cat.find("\"", titlePos + 9);
        std::string title = cat.substr(titlePos + 9, titleEnd - (titlePos + 9));

        size_t catPos = cat.find("\"category\":\"", pos);
        size_t catEnd = cat.find("\"", catPos + 12);
        std::string category = cat.substr(catPos + 12, catEnd - (catPos + 12));

        for (const auto& tc : titleCats) {
            if (tc.first == title && tc.second == category) {
                EXPECT(false, "Duplicate title in category " + category + ": " + title);
            }
        }
        titleCats.push_back({title, category});
        pos = catEnd;
    }

    std::cout << "[DIAGNOSIS] Completed all scenarios without crashing!" << std::endl;
    return 0;
}
