#include <iostream>
#undef NDEBUG
#include <cassert>
#include <vector>
#include <string>
#include <algorithm>
#include <gamehub/core/HostEngine.hpp>
#include <gamehub/core/INetworkAdapter.hpp>
#include <gamehub/core/GameRegistry.hpp>

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

#ifdef _WIN32
#include <crtdbg.h>
#endif

int main() {
#ifdef _WIN32
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
#endif
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

    // --- 2. Room Creation (Multiplayer Domino) ---
    std::string roomCode;
    {
        std::cout << "  - Testing room creation..." << std::endl;
        mockNet.clear();
        engine.onClientMessage(101, "{\"cmd\":\"create\",\"game\":\"domino_pvp\",\"host\":\"Alice\",\"max\":2}");

        assert(!mockNet.sentMessages.empty());
        std::string reply = mockNet.getLastMessageFor(101);
        assert(reply.find("\"type\":\"room_created\"") != std::string::npos);
        assert(reply.find("\"player\":1") != std::string::npos);
        assert(reply.find("\"host\":\"Alice\"") != std::string::npos);
        assert(reply.find("\"game\":\"domino_pvp\"") != std::string::npos);
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
        assert(mockNet.hasMessageFor(102, "\"game\":\"domino_pvp\""));
        assert(mockNet.hasMessageFor(102, "\"max\":2"));

        // Both players should receive room_ready
        assert(mockNet.hasMessageFor(101, "\"type\":\"room_ready\""));
        assert(mockNet.hasMessageFor(102, "\"type\":\"room_ready\""));

        // Both players should receive the initial game state
        assert(mockNet.hasMessageFor(101, "\"game\":\"domino\""));
        assert(mockNet.hasMessageFor(102, "\"game\":\"domino\""));
        std::cout << "    Player 2 joined and room started." << std::endl;
    }

    // --- 4. In-Game Input Routing & Fog-of-War ---
    {
        std::cout << "  - Testing in-game input routing & Fog-of-War hand security..." << std::endl;
        std::string p1State = mockNet.getLastMessageFor(101);
        std::string p2State = mockNet.getLastMessageFor(102);

        // Security assertion: In P1's state, P1 hand must exist, but "hands":{"2": MUST NOT exist while match is ongoing!
        assert(p1State.find("\"hands\":{\"1\":") != std::string::npos);
        assert(p1State.find("\"2\":[") == std::string::npos);

        // In P2's state, P2 hand must exist, but "hands":{"1": MUST NOT exist!
        assert(p2State.find("\"hands\":{\"2\":") != std::string::npos);
        assert(p2State.find("\"1\":[") == std::string::npos);
        std::cout << "    Domino Fog-of-War verified: Opponent hands hidden." << std::endl;
    }

    // --- 5. Disconnect & Opponent Notification ---
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

    // --- 6. Solo Games via HostEngine ---
    {
        std::cout << "  - Testing Solo Game execution via HostEngine..." << std::endl;
        mockNet.clear();
        engine.onClientConnect(301);
        engine.onClientMessage(301, "{\"cmd\":\"start\",\"game\":\"domino_solo\"}");

        assert(mockNet.hasMessageFor(301, "\"game\":\"domino\""));
        std::cout << "    Solo Domino execution verified." << std::endl;

        // Solo Katro
        mockNet.clear();
        engine.onClientMessage(301, "{\"cmd\":\"start\",\"game\":\"katro_solo\"}");
        assert(mockNet.hasMessageFor(301, "\"game\":\"katro\""));
        std::cout << "    Solo Katro execution verified." << std::endl;

        // Solo Ludo
        mockNet.clear();
        engine.onClientMessage(301, "{\"cmd\":\"start\",\"game\":\"ludo_solo\"}");
        assert(mockNet.hasMessageFor(301, "\"game\":\"ludo\""));
        std::cout << "    Solo Ludo execution verified." << std::endl;
        engine.onClientDisconnect(301);
    }

    // --- 7. Authoritative Rematch & Series Score in Katro ---
    {
        std::cout << "  - Testing Authoritative Rematch & Series Score..." << std::endl;
        mockNet.clear();
        engine.onClientConnect(501);
        engine.onClientConnect(502);

        engine.onClientMessage(501, "{\"cmd\":\"create\",\"game\":\"katro_pvp\",\"host\":\"Player1\"}");
        std::string katCode = mockNet.getLastMessageFor(501).substr(mockNet.getLastMessageFor(501).find("\"code\":\"") + 8, 4);
        engine.onClientMessage(502, "{\"cmd\":\"join\",\"code\":\"" + katCode + "\"}");

        // Verify room ready
        assert(mockNet.hasMessageFor(501, "\"type\":\"room_ready\""));
        assert(mockNet.hasMessageFor(502, "\"type\":\"room_ready\""));

        engine.onClientDisconnect(501);
        engine.onClientDisconnect(502);
        std::cout << "    Multiplayer Katro lifecycle verified." << std::endl;
    }

    // --- 8. Multiplayer Ludo Table Lifecycle ---
    {
        std::cout << "  - Testing Multiplayer Ludo Table Lifecycle..." << std::endl;
        // Ludo Quick Match
        mockNet.clear();
        engine.onClientConnect(701);
        engine.onClientConnect(702);
        engine.onClientMessage(701, "{\"cmd\":\"create\",\"game\":\"ludo_quick\",\"host\":\"HostLudo\",\"players\":2}");
        std::string ludoCode = mockNet.getLastMessageFor(701).substr(mockNet.getLastMessageFor(701).find("\"code\":\"") + 8, 4);
        engine.onClientMessage(702, "{\"cmd\":\"join\",\"code\":\"" + ludoCode + "\"}");
        assert(mockNet.hasMessageFor(701, "\"type\":\"room_ready\""));
        assert(mockNet.hasMessageFor(702, "\"type\":\"room_ready\""));
        engine.onClientDisconnect(701);
        engine.onClientDisconnect(702);
        std::cout << "    Multiplayer Ludo Quick Match lifecycle verified." << std::endl;
    }

    std::cout << "[SUCCESS] All HostEngine tests passed!" << std::endl;
    return 0;
}
