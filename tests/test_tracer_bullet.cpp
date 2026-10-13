#include <cassert>
#include <iostream>
#include <string>
#include <gamehub/web/WebAssets.hpp>
#include <gamehub/core/Session.hpp>

int main() {
    std::cout << "[TEST] Running Tracer Bullet Verification..." << std::endl;

    // Test 1: Verify embedded web assets are present, compressed with RFC 1952 Gzip
    assert(gamehub::web::WebAssets::isGzipped() && "WebAssets must report isGzipped() true");
    const uint8_t* data = gamehub::web::WebAssets::getIndexHtmlData();
    size_t compressedSize = gamehub::web::WebAssets::getIndexHtmlSize();
    size_t uncompressedSize = gamehub::web::WebAssets::getUncompressedSize();

    assert(compressedSize > 18 && "Gzip stream must have at least 10-byte header and 8-byte trailer");
    assert(data[0] == 0x1F && data[1] == 0x8B && "Gzip stream must begin with RFC 1952 magic bytes 0x1F, 0x8B");
    assert(data[2] == 0x08 && "Gzip stream must use Deflate compression method (0x08)");
    assert(compressedSize < uncompressedSize && "Compressed size must be strictly smaller than uncompressed size");

    // Verify RFC 1952 trailer ISIZE (uncompressed size modulo 2^32, little-endian)
    uint32_t trailerISize = static_cast<uint32_t>(data[compressedSize - 4]) |
                           (static_cast<uint32_t>(data[compressedSize - 3]) << 8) |
                           (static_cast<uint32_t>(data[compressedSize - 2]) << 16) |
                           (static_cast<uint32_t>(data[compressedSize - 1]) << 24);
    assert(trailerISize == static_cast<uint32_t>(uncompressedSize) && "Gzip trailer ISIZE must match uncompressed size");
    std::cout << "  [PASS] WebAssets contains valid RFC 1952 Gzip stream ("
              << compressedSize << " bytes compressed vs " << uncompressedSize << " bytes original)." << std::endl;

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
