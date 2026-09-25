#include <cassert>
#include <iostream>
#include <string>
#include <gamehub/web/WebAssets.hpp>
#include <gamehub/core/Session.hpp>

int main() {
    std::cout << "[TEST] Running Tracer Bullet Verification..." << std::endl;

    // Test 1: Verify embedded web assets are present and valid HTML
    std::string html = gamehub::web::WebAssets::getIndexHtml();
    assert(!html.empty() && "Embedded HTML must not be empty");
    assert(html.find("<canvas") != std::string::npos && "HTML must contain canvas element");
    assert(html.find("WebSocket") != std::string::npos && "HTML must contain WebSocket client code");
    std::cout << "  [PASS] WebAssets contains valid HTML and WebSocket client." << std::endl;

    // Test 2: Verify Session ping-pong message handling
    gamehub::core::Session session(1);
    assert(session.getId() == 1);
    
    std::string pingMsg = R"({"type":"ping","ts":12345})";
    std::string response = session.handleMessage(pingMsg);
    assert(!response.empty() && "Response to ping must not be empty");
    assert(response.find("\"type\":\"pong\"") != std::string::npos && "Response must be a pong");
    assert(response.find("12345") != std::string::npos && "Pong must reflect timestamp");
    std::cout << "  [PASS] Session responds to ping with pong." << std::endl;

    std::cout << "[TEST] All Tracer Bullet Tests Passed!" << std::endl;
    return 0;
}
