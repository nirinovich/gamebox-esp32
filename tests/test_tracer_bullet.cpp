#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <gamehub/web/WebAssets.hpp>
#include <gamehub/core/Session.hpp>
#include <gamehub/util/GzipCompressor.hpp>

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

    // Test 2: In-memory round-trip decompression verification
    std::vector<uint8_t> decompressed = gamehub::util::GzipCompressor::decompress(data, compressedSize);
    assert(decompressed.size() == uncompressedSize && "Decompressed size must exactly match uncompressed size");
    std::string decompressedHtml(decompressed.begin(), decompressed.end());
    assert(decompressedHtml.find("<!DOCTYPE html>") != std::string::npos && "Decompressed content must contain HTML doctype");
    assert(decompressedHtml.find("canvas") != std::string::npos && "Decompressed content must contain canvas reference");
    assert(decompressedHtml.find("WebSocket") != std::string::npos && "Decompressed content must contain WebSocket reference");

    // If source index.html exists, verify exact byte-for-byte fidelity
    std::filesystem::path sourceHtml = "frontend/dist/index.html";
    if (std::filesystem::exists(sourceHtml)) {
        std::ifstream in(sourceHtml, std::ios::binary);
        std::vector<uint8_t> expectedBytes(
            (std::istreambuf_iterator<char>(in)),
            std::istreambuf_iterator<char>()
        );
        in.close();
        assert(decompressed == expectedBytes && "Decompressed stream must match frontend/dist/index.html byte-for-byte");
        std::cout << "  [PASS] Byte-for-byte identity verified against frontend/dist/index.html." << std::endl;
    } else {
        std::cout << "  [PASS] In-memory decompression integrity verified (HTML tags intact)." << std::endl;
    }

    // Test 3: Synthetic round-trip test on arbitrary sample data
    std::string sample = "Hello Gamebox! Test compression with repetitive patterns 1234567890 1234567890 1234567890.";
    std::vector<uint8_t> sampleCompressed = gamehub::util::GzipCompressor::compress(
        reinterpret_cast<const uint8_t*>(sample.data()), sample.size()
    );
    std::vector<uint8_t> sampleDecompressed = gamehub::util::GzipCompressor::decompress(sampleCompressed);
    std::string sampleRestored(sampleDecompressed.begin(), sampleDecompressed.end());
    assert(sampleRestored == sample && "Synthetic sample round-trip must reproduce original string exactly");
    std::cout << "  [PASS] Synthetic payload round-trip compression and decompression verified." << std::endl;

    // Test 4: Verify Session ping-pong message handling
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
