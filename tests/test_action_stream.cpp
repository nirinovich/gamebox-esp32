#include <iostream>
#undef NDEBUG
#include <cassert>
#include <string>
#ifdef _WIN32
#include <crtdbg.h>
#endif
#include <domino/host/DominoGame.hpp>

int main() {
#ifdef _WIN32
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
#endif
    std::cout << "[TEST] Running Authoritative ActionStream and State Prompt Protocol tests..." << std::endl;

    using namespace gamehub::core;

    // 1. Verify Domino action stream
    DominoGame game(true, 2, 100);
    game.init();

    std::string s1 = game.serializeStateForPlayer(1);
    assert(s1.find("\"available_actions\":[") != std::string::npos);
    assert(s1.find("\"id\":\"play\"") != std::string::npos);

    // Play tile through public handleInput seam
    game.handleInput(1, "{\"cmd\":\"play\",\"index\":0,\"end\":\"left\"}");

    std::cout << "[TEST] Authoritative ActionStream and State Prompt Protocol tests PASSED!" << std::endl;
    return 0;
}
