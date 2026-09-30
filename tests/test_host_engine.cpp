#include <iostream>
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

        // Player 2 should receive room_joined
        assert(mockNet.hasMessageFor(102, "\"type\":\"room_joined\""));

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

    // --- 5. Real-Time Tick Loop ---
    {
        std::cout << "  - Testing real-time tick loop..." << std::endl;
        mockNet.clear();
        engine.tick(0.066f);

        // Snake Duel is a real-time game: tick should broadcast updated states
        assert(mockNet.hasMessageFor(101, "\"type\":\"state\""));
        assert(mockNet.hasMessageFor(102, "\"type\":\"state\""));
        std::cout << "    Real-time tick loop advanced match." << std::endl;
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

        // Tick solo game
        mockNet.clear();
        engine.onClientMessage(301, "{\"cmd\":\"tick\"}");
        assert(mockNet.hasMessageFor(301, "\"game\":\"snake\""));
        std::cout << "    Solo Snake game execution verified." << std::endl;

        // Solo Pong
        mockNet.clear();
        engine.onClientMessage(301, "{\"cmd\":\"start\",\"game\":\"pong_solo\"}");
        assert(mockNet.hasMessageFor(301, "\"game\":\"pong\""));
        std::cout << "    Solo Pong game execution verified." << std::endl;

        // Solo Tron
        mockNet.clear();
        engine.onClientMessage(301, "{\"cmd\":\"start\",\"game\":\"tron_solo\"}");
        assert(mockNet.hasMessageFor(301, "\"game\":\"tron\""));
        std::cout << "    Solo Tron game execution verified." << std::endl;

        // Solo Battleship
        mockNet.clear();
        engine.onClientMessage(301, "{\"cmd\":\"start\",\"game\":\"battleship_solo\"}");
        assert(mockNet.hasMessageFor(301, "\"game\":\"battleship\""));
        std::cout << "    Solo Battleship game execution verified." << std::endl;
    }

    std::cout << "[SUCCESS] All HostEngine tests passed!" << std::endl;
    return 0;
}
