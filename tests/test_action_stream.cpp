#include <iostream>
#undef NDEBUG
#include <cassert>
#include <string>
#include <domino/host/DominoGame.hpp>

int main() {
    std::cout << "[TEST] Running Authoritative ActionStream and State Prompt Protocol tests..." << std::endl;

    using namespace gamehub::core;

    // 1. Verify Domino action stream
    DominoGame game(true, 2, false); // Solo against 1 bot
    game.init();

    std::string s1 = game.serializeStateForPlayer(1);
    assert(s1.find("\"available_actions\":[") != std::string::npos);
    assert(s1.find("\"id\":\"draw\"") != std::string::npos);
    assert(s1.find("\"id\":\"pass\"") != std::string::npos);

    // Play initial tile through public handleInput seam
    game.handleInput(1, "{\"cmd\":\"play\",\"index\":0,\"end\":\"left\"}");
    
    // Test choice clearing via play command
    game.handleInput(1, "{\"cmd\":\"play\",\"index\":0,\"end\":\"left\"}");
    std::string sAfter = game.serializeStateForPlayer(1);
    assert(sAfter.find("\"prompt\":{") == std::string::npos);

    std::cout << "[TEST] Authoritative ActionStream and State Prompt Protocol tests PASSED!" << std::endl;
    return 0;
}
