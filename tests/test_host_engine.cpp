#include <iostream>
#undef NDEBUG
#include <cassert>
#include <vector>
#include <string>
#include <algorithm>
#include <gamehub/core/HostEngine.hpp>
#include <gamehub/core/INetworkAdapter.hpp>
#include <gamehub/core/GameTypes.hpp>

namespace {

class MockNetworkAdapter : public gamehub::core::INetworkAdapter {
public:
    struct SentMessage {
        uint32_t sessionId;
        std::string payload;
    };

    std::vector<SentMessage> sentMessages;
    std::vector<std::string> broadcasts;

    void sendToClient(uint32_t sessionId, const std::string& message) override {
        sentMessages.push_back({sessionId, message});
    }

    void broadcast(const std::string& message) override {
        broadcasts.push_back(message);
    }

    void clear() {
        sentMessages.clear();
        broadcasts.clear();
    }

    std::string getLastMessageFor(uint32_t sessionId) const {
        for (auto it = sentMessages.rbegin(); it != sentMessages.rend(); ++it) {
            if (it->sessionId == sessionId) return it->payload;
        }
        return "";
    }

    bool hasMessageFor(uint32_t sessionId, const std::string& substring) const {
        for (const auto& msg : sentMessages) {
            if (msg.sessionId == sessionId && msg.payload.find(substring) != std::string::npos) {
                return true;
            }
        }
        return false;
    }
};

} // namespace

int main() {
    std::cout << "[TEST] Running HostEngine pure C++ test suite..." << std::endl;

    MockNetworkAdapter mockNet;
    gamehub::core::HostEngine engine(&mockNet, 4);

    // --- 1. Client Connect & Directory Broadcast ---
    {
        std::cout << "  - Testing onClientConnect..." << std::endl;
        engine.onClientConnect(101);
        assert(!mockNet.sentMessages.empty());
        assert(mockNet.sentMessages.back().sessionId == 101);
        assert(mockNet.sentMessages.back().payload.find("\"type\":\"room_directory\"") != std::string::npos);
        std::cout << "    Client connect received room directory." << std::endl;
    }

    // --- 2. Room Creation (Multiplayer) ---
    std::string roomCode;
    {
        std::cout << "  - Testing room creation..." << std::endl;
        mockNet.clear();
        engine.onClientMessage(101, "{\"cmd\":\"create\",\"game\":\"snake_duel\",\"host\":\"Alice\"}");

        assert(!mockNet.sentMessages.empty());
        std::string reply = mockNet.getLastMessageFor(101);
        assert(reply.find("\"type\":\"room_created\"") != std::string::npos);
        assert(reply.find("\"player\":1") != std::string::npos);
        assert(reply.find("\"host\":\"Alice\"") != std::string::npos);
        assert(reply.find("\"game\":\"snake_duel\"") != std::string::npos);
        assert(reply.find("\"max\":2") != std::string::npos);

        // Extract room code
        auto codePos = reply.find("\"code\":\"");
        assert(codePos != std::string::npos);
        roomCode = reply.substr(codePos + 8, 4);
        assert(roomCode.size() == 4);

        // Verify directory was broadcast
        assert(!mockNet.broadcasts.empty());
        assert(mockNet.broadcasts.back().find(roomCode) != std::string::npos);
        std::cout << "    Room created successfully with code: " << roomCode << std::endl;
    }

    // --- 3. Room Joining & Match Start ---
    {
        std::cout << "  - Testing room joining..." << std::endl;
        mockNet.clear();
        engine.onClientConnect(102);
        engine.onClientMessage(102, "{\"cmd\":\"join\",\"code\":\"" + roomCode + "\"}");

        // Player 2 should receive room_joined with game and max metadata
        assert(mockNet.hasMessageFor(102, "\"type\":\"room_joined\""));
        assert(mockNet.hasMessageFor(102, "\"game\":\"snake_duel\""));
        assert(mockNet.hasMessageFor(102, "\"max\":2"));

        // Both players should receive room_ready
        assert(mockNet.hasMessageFor(101, "\"type\":\"room_ready\""));
        assert(mockNet.hasMessageFor(102, "\"type\":\"room_ready\""));

        // Both players should receive the initial game state
        assert(mockNet.hasMessageFor(101, "\"game\":\"snake\""));
        assert(mockNet.hasMessageFor(102, "\"game\":\"snake\""));
        std::cout << "    Player 2 joined and room started." << std::endl;
    }

    // --- 4. In-Game Input Routing ---
    {
        std::cout << "  - Testing in-game input routing..." << std::endl;
        mockNet.clear();
        engine.onClientMessage(101, "{\"cmd\":\"dir\",\"val\":\"DOWN\"}");

        // State update should be broadcast to both room players
        assert(mockNet.hasMessageFor(101, "\"type\":\"state\""));
        assert(mockNet.hasMessageFor(102, "\"type\":\"state\""));
        std::cout << "    Multiplayer input successfully routed." << std::endl;
    }

    // --- 5. Real-Time Tick Loop & Game Over Broadcast ---
    {
        std::cout << "  - Testing real-time tick loop..." << std::endl;
        mockNet.clear();
        engine.tick(0.066f);

        // Snake Duel is a real-time game: tick should broadcast updated states
        assert(mockNet.hasMessageFor(101, "\"type\":\"state\""));
        assert(mockNet.hasMessageFor(102, "\"type\":\"state\""));

        // Tick repeatedly until snakes hit walls / match finishes
        bool sawFinishedBroadcast = false;
        for (int i = 0; i < 40; ++i) {
            mockNet.clear();
            engine.tick(0.066f);
            if (mockNet.hasMessageFor(101, "\"finished\":true")) {
                sawFinishedBroadcast = true;
                assert(mockNet.hasMessageFor(102, "\"finished\":true"));
                break;
            }
        }
        assert(sawFinishedBroadcast);
        std::cout << "    Real-time tick loop advanced match and broadcast final Game Over state." << std::endl;
    }

    // --- 6. Disconnect & Opponent Notification ---
    {
        std::cout << "  - Testing client disconnect handling..." << std::endl;
        mockNet.clear();
        engine.onClientDisconnect(101);

        // Player 1 left: Player 2 must receive opponent_left
        assert(mockNet.hasMessageFor(102, "\"type\":\"opponent_left\""));

        // Directory must be broadcast indicating room dissolved
        assert(!mockNet.broadcasts.empty());
        assert(mockNet.broadcasts.back().find(roomCode) == std::string::npos);
        std::cout << "    Disconnect handled and opponent notified." << std::endl;
    }

    // --- 7. Fog-of-War Masking Verification in Battleship ---
    {
        std::cout << "  - Testing Battleship Fog-of-War network security..." << std::endl;
        mockNet.clear();
        engine.onClientConnect(201);
        engine.onClientConnect(202);

        engine.onClientMessage(201, "{\"cmd\":\"create\",\"game\":\"battleship_pvp\",\"host\":\"Admiral1\"}");
        std::string bshipCode = mockNet.getLastMessageFor(201).substr(mockNet.getLastMessageFor(201).find("\"code\":\"") + 8, 4);

        engine.onClientMessage(202, "{\"cmd\":\"join\",\"code\":\"" + bshipCode + "\"}");

        // Both players auto-deploy fleets
        mockNet.clear();
        engine.onClientMessage(201, "{\"cmd\":\"auto_place\"}");
        engine.onClientMessage(202, "{\"cmd\":\"auto_place\"}");

        // Now battle phase starts
        std::string p1State = mockNet.getLastMessageFor(201);
        std::string p2State = mockNet.getLastMessageFor(202);

        assert(p1State.find("\"phase\":\"battle\"") != std::string::npos);
        assert(p2State.find("\"phase\":\"battle\"") != std::string::npos);

        // Fog-of-War check:
        // In p1's state, p2's grid ("p2":{"own":[...]}) must NOT have ship id 1 (unhit ship)
        // If p2's own ships were visible, "own":[...,1,...] would appear in p2 block.
        auto p1_p2Block = p1State.find("\"p2\":");
        assert(p1_p2Block != std::string::npos);
        std::string p2PerspectiveInP1 = p1State.substr(p1_p2Block);
        // Only 0 (fog/water), 2 (hit), 3 (miss) allowed; 1 (unhit ship) MUST NOT appear in p2.own!
        assert(p2PerspectiveInP1.find("\"own\":[") != std::string::npos);
        auto p2OwnArray = p2PerspectiveInP1.substr(p2PerspectiveInP1.find("\"own\":["));
        p2OwnArray = p2OwnArray.substr(0, p2OwnArray.find(']'));
        assert(p2OwnArray.find(",1,") == std::string::npos && p2OwnArray.find("[1,") == std::string::npos && p2OwnArray.find(",1]") == std::string::npos);

        std::cout << "    Fog-of-War verified: Enemy fleet hidden from client packets." << std::endl;
        engine.onClientDisconnect(201);
        engine.onClientDisconnect(202);
    }

    // --- 8. Solo Games via HostEngine ---
    {
        std::cout << "  - Testing Solo Game execution via HostEngine..." << std::endl;
        mockNet.clear();
        engine.onClientConnect(301);
        engine.onClientMessage(301, "{\"cmd\":\"start\",\"game\":\"snake_solo\"}");

        assert(mockNet.hasMessageFor(301, "\"game\":\"snake\""));
        assert(mockNet.hasMessageFor(301, "\"score\":0"));

        // Authoritative HostEngine tick loop MUST advance and send state for solo real-time games
        mockNet.clear();
        engine.tick(0.066f);
        assert(mockNet.hasMessageFor(301, "\"game\":\"snake\"") && "HostEngine::tick must tick and broadcast solo snake state to client");
        std::cout << "    Solo Snake real-time engine tick verified." << std::endl;

        // Solo Pong
        mockNet.clear();
        engine.onClientMessage(301, "{\"cmd\":\"start\",\"game\":\"pong_solo\"}");
        assert(mockNet.hasMessageFor(301, "\"game\":\"pong\""));
        mockNet.clear();
        engine.tick(0.066f);
        assert(mockNet.hasMessageFor(301, "\"game\":\"pong\"") && "HostEngine::tick must tick and broadcast solo pong");
        std::cout << "    Solo Pong game execution verified." << std::endl;

        // Solo Tron
        mockNet.clear();
        engine.onClientMessage(301, "{\"cmd\":\"start\",\"game\":\"tron_solo\"}");
        assert(mockNet.hasMessageFor(301, "\"game\":\"tron\""));
        mockNet.clear();
        engine.tick(0.1f);
        assert(mockNet.hasMessageFor(301, "\"game\":\"tron\"") && "HostEngine::tick must tick and broadcast solo tron");
        std::cout << "    Solo Tron game execution verified." << std::endl;

        // Solo Battleship
        mockNet.clear();
        engine.onClientMessage(301, "{\"cmd\":\"start\",\"game\":\"battleship_solo\"}");
        assert(mockNet.hasMessageFor(301, "\"game\":\"battleship\""));
        std::cout << "    Solo Battleship game execution verified." << std::endl;
    }

    // --- 9. Domino Fog-of-War Network Security ---
    {
        std::cout << "  - Testing Domino Fog-of-War hand security..." << std::endl;
        mockNet.clear();
        engine.onClientConnect(401);
        engine.onClientConnect(402);

        engine.onClientMessage(401, "{\"cmd\":\"create\",\"game\":\"domino_pvp\",\"max\":2,\"host\":\"BonePlayer1\"}");
        std::string domCode = mockNet.getLastMessageFor(401).substr(mockNet.getLastMessageFor(401).find("\"code\":\"") + 8, 4);

        engine.onClientMessage(402, "{\"cmd\":\"join\",\"code\":\"" + domCode + "\"}");

        std::string p1State = mockNet.getLastMessageFor(401);
        std::string p2State = mockNet.getLastMessageFor(402);

        assert(p1State.find("\"game\":\"domino\"") != std::string::npos);
        assert(p2State.find("\"game\":\"domino\"") != std::string::npos);

        // Security assertion: In P1's state, P1 hand must exist, but "hands":{"2": MUST NOT exist while match is ongoing!
        assert(p1State.find("\"hands\":{\"1\":") != std::string::npos);
        assert(p1State.find("\"2\":[") == std::string::npos);

        // In P2's state, P2 hand must exist, but "hands":{"1": MUST NOT exist!
        assert(p2State.find("\"hands\":{\"2\":") != std::string::npos);
        assert(p2State.find("\"1\":[") == std::string::npos);

        std::cout << "    Domino Fog-of-War verified: Opponent hands hidden." << std::endl;
        engine.onClientDisconnect(401);
        engine.onClientDisconnect(402);
    }

    // --- 10. Authoritative Rematch & Series Score ---
    {
        std::cout << "  - Testing Authoritative Rematch & Series Score..." << std::endl;
        mockNet.clear();
        engine.onClientConnect(501);
        engine.onClientConnect(502);

        engine.onClientMessage(501, "{\"cmd\":\"create\",\"game\":\"tictactoe_pvp\",\"host\":\"XPlayer\"}");
        std::string tttCode = mockNet.getLastMessageFor(501).substr(mockNet.getLastMessageFor(501).find("\"code\":\"") + 8, 4);
        engine.onClientMessage(502, "{\"cmd\":\"join\",\"code\":\"" + tttCode + "\"}");

        // Quick win for P1 (moves: 0, 3, 1, 4, 2 -> P1 wins top row)
        engine.onClientMessage(501, "{\"cmd\":\"move\",\"cell\":0}");
        engine.onClientMessage(502, "{\"cmd\":\"move\",\"cell\":3}");
        engine.onClientMessage(501, "{\"cmd\":\"move\",\"cell\":1}");
        engine.onClientMessage(502, "{\"cmd\":\"move\",\"cell\":4}");
        engine.onClientMessage(501, "{\"cmd\":\"move\",\"cell\":2}");

        std::string finishedState = mockNet.getLastMessageFor(501);
        assert(finishedState.find("\"finished\":true") != std::string::npos);
        assert(finishedState.find("\"winner\":1") != std::string::npos);

        // P1 requests rematch
        mockNet.clear();
        engine.onClientMessage(501, "{\"cmd\":\"rematch\"}");
        assert(mockNet.hasMessageFor(501, "\"type\":\"rematch_update\""));
        assert(mockNet.hasMessageFor(502, "\"type\":\"rematch_update\""));

        // P2 accepts rematch
        mockNet.clear();
        engine.onClientMessage(502, "{\"cmd\":\"rematch\"}");

        // Now room restarts! "room_ready" sent, state reset to finished=false, and series_score included
        assert(mockNet.hasMessageFor(501, "\"type\":\"room_ready\""));
        assert(mockNet.hasMessageFor(502, "\"type\":\"room_ready\""));

        std::string restartedState = mockNet.getLastMessageFor(501);
        assert(restartedState.find("\"finished\":false") != std::string::npos);
        assert(restartedState.find("\"series_p1\":1") != std::string::npos);

        std::cout << "    Rematch & Series Score verified." << std::endl;
        engine.onClientDisconnect(501);
        engine.onClientDisconnect(502);
    }

    std::cout << "[SUCCESS] All HostEngine tests passed!" << std::endl;
    return 0;
}

