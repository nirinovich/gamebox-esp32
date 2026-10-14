#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <gamehub/web/WebAssets.hpp>
#include <gamehub/core/Session.hpp>
#include <miniz.h>

namespace {

std::vector<uint8_t> decompressGzip(const uint8_t* data, size_t size) {
    assert(size >= 18 && "Gzip payload must have at least 10-byte header and 8-byte trailer");
    assert(data[0] == 0x1F && data[1] == 0x8B && "Gzip stream must begin with RFC 1952 magic bytes 0x1F, 0x8B");
    assert(data[2] == 0x08 && "Gzip stream must use Deflate compression method (0x08)");

    size_t trailerOffset = size - 8;
    uint32_t expectedCrc = static_cast<uint32_t>(data[trailerOffset]) |
                          (static_cast<uint32_t>(data[trailerOffset + 1]) << 8) |
                          (static_cast<uint32_t>(data[trailerOffset + 2]) << 16) |
                          (static_cast<uint32_t>(data[trailerOffset + 3]) << 24);
    uint32_t expectedSize = static_cast<uint32_t>(data[trailerOffset + 4]) |
                           (static_cast<uint32_t>(data[trailerOffset + 5]) << 8) |
                           (static_cast<uint32_t>(data[trailerOffset + 6]) << 16) |
                           (static_cast<uint32_t>(data[trailerOffset + 7]) << 24);

    std::vector<uint8_t> decompressed(expectedSize);
    mz_stream stream{};
    int status = mz_inflateInit2(&stream, -MZ_DEFAULT_WINDOW_BITS);
    assert(status == MZ_OK && "mz_inflateInit2 failed");

    stream.next_in = data + 10;
    stream.avail_in = static_cast<unsigned int>(size - 18);
    stream.next_out = decompressed.data();
    stream.avail_out = static_cast<unsigned int>(decompressed.size());

    status = mz_inflate(&stream, MZ_FINISH);
    assert(status == MZ_STREAM_END && "mz_inflate failed to complete");
    mz_inflateEnd(&stream);

    mz_ulong actualCrc = mz_crc32(0, decompressed.data(), decompressed.size());
    assert(actualCrc == expectedCrc && "CRC-32 checksum mismatch in Gzip payload");

    return decompressed;
}

std::vector<uint8_t> compressGzip(const uint8_t* inData, size_t inSize) {
    mz_stream stream{};
    int status = mz_deflateInit2(&stream, MZ_BEST_COMPRESSION, MZ_DEFLATED, -MZ_DEFAULT_WINDOW_BITS, 9, MZ_DEFAULT_STRATEGY);
    assert(status == MZ_OK && "mz_deflateInit2 failed");

    mz_ulong maxDeflateSize = mz_compressBound(static_cast<mz_ulong>(inSize));
    std::vector<uint8_t> deflateBuf(maxDeflateSize);

    stream.next_in = inData;
    stream.avail_in = static_cast<unsigned int>(inSize);
    stream.next_out = deflateBuf.data();
    stream.avail_out = static_cast<unsigned int>(deflateBuf.size());

    status = mz_deflate(&stream, MZ_FINISH);
    assert(status == MZ_STREAM_END && "mz_deflate failed");
    size_t deflateSize = stream.total_out;
    mz_deflateEnd(&stream);

    mz_ulong crc = mz_crc32(0, inData, inSize);
    uint32_t isize = static_cast<uint32_t>(inSize);

    std::vector<uint8_t> gzip;
    gzip.reserve(10 + deflateSize + 8);
    // 10-byte RFC 1952 header
    gzip.push_back(0x1F); gzip.push_back(0x8B); gzip.push_back(0x08); gzip.push_back(0x00);
    gzip.push_back(0x00); gzip.push_back(0x00); gzip.push_back(0x00); gzip.push_back(0x00);
    gzip.push_back(0x02); gzip.push_back(0xFF);

    gzip.insert(gzip.end(), deflateBuf.data(), deflateBuf.data() + deflateSize);

    // 8-byte trailer
    gzip.push_back(static_cast<uint8_t>(crc & 0xFF));
    gzip.push_back(static_cast<uint8_t>((crc >> 8) & 0xFF));
    gzip.push_back(static_cast<uint8_t>((crc >> 16) & 0xFF));
    gzip.push_back(static_cast<uint8_t>((crc >> 24) & 0xFF));
    gzip.push_back(static_cast<uint8_t>(isize & 0xFF));
    gzip.push_back(static_cast<uint8_t>((isize >> 8) & 0xFF));
    gzip.push_back(static_cast<uint8_t>((isize >> 16) & 0xFF));
    gzip.push_back(static_cast<uint8_t>((isize >> 24) & 0xFF));

    return gzip;
}

} // namespace

int main() {
    std::cout << "[TEST] Running Tracer Bullet Verification (miniz)..." << std::endl;

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

    // Test 2: In-memory round-trip decompression verification using miniz
    std::vector<uint8_t> decompressed = decompressGzip(data, compressedSize);
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

    // Test 3: Synthetic round-trip test on arbitrary sample data using miniz
    std::string sample = "Hello Gamebox! Test compression with repetitive patterns 1234567890 1234567890 1234567890.";
    std::vector<uint8_t> sampleCompressed = compressGzip(
        reinterpret_cast<const uint8_t*>(sample.data()), sample.size()
    );
    std::vector<uint8_t> sampleDecompressed = decompressGzip(sampleCompressed.data(), sampleCompressed.size());
    std::string sampleRestored(sampleDecompressed.begin(), sampleDecompressed.end());
    assert(sampleRestored == sample && "Synthetic sample round-trip must reproduce original string exactly");
    std::cout << "  [PASS] Synthetic payload round-trip compression and decompression verified (miniz)." << std::endl;

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
