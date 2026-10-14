#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <iomanip>
#include <cstdint>
#include <cassert>
#include <stdexcept>
#include <miniz.h>

namespace {

std::vector<uint8_t> compressToGzip(const std::vector<uint8_t>& input) {
    // 1. Raw Deflate compression using miniz
    mz_stream stream{};
    int status = mz_deflateInit2(&stream, MZ_BEST_COMPRESSION, MZ_DEFLATED, -MZ_DEFAULT_WINDOW_BITS, 9, MZ_DEFAULT_STRATEGY);
    if (status != MZ_OK) {
        throw std::runtime_error("mz_deflateInit2 failed with status " + std::to_string(status));
    }

    mz_ulong maxDeflateSize = mz_compressBound(static_cast<mz_ulong>(input.size()));
    std::vector<uint8_t> deflateBuf(maxDeflateSize);

    stream.next_in = input.data();
    stream.avail_in = static_cast<unsigned int>(input.size());
    stream.next_out = deflateBuf.data();
    stream.avail_out = static_cast<unsigned int>(deflateBuf.size());

    status = mz_deflate(&stream, MZ_FINISH);
    if (status != MZ_STREAM_END) {
        mz_deflateEnd(&stream);
        throw std::runtime_error("mz_deflate failed to finish, status " + std::to_string(status));
    }
    size_t deflateSize = stream.total_out;
    mz_deflateEnd(&stream);

    // 2. Compute CRC-32 and input size for RFC 1952 Gzip trailer
    mz_ulong crc = mz_crc32(0, input.data(), input.size());
    uint32_t isize = static_cast<uint32_t>(input.size());

    // 3. Assemble RFC 1952 Gzip container:
    // Header (10 bytes) + Deflate Payload + Trailer (8 bytes: CRC-32 + ISIZE)
    std::vector<uint8_t> gzip;
    gzip.reserve(10 + deflateSize + 8);

    // 10-byte Gzip Header
    gzip.push_back(0x1F); // ID1
    gzip.push_back(0x8B); // ID2
    gzip.push_back(0x08); // CM = Deflate
    gzip.push_back(0x00); // FLG = 0
    gzip.push_back(0x00); // MTIME byte 0
    gzip.push_back(0x00); // MTIME byte 1
    gzip.push_back(0x00); // MTIME byte 2
    gzip.push_back(0x00); // MTIME byte 3
    gzip.push_back(0x02); // XFL = 2 (maximum compression)
    gzip.push_back(0xFF); // OS = 255 (unknown / generic)

    // Deflate stream
    gzip.insert(gzip.end(), deflateBuf.data(), deflateBuf.data() + deflateSize);

    // Trailer: CRC-32 (4 bytes little-endian)
    gzip.push_back(static_cast<uint8_t>(crc & 0xFF));
    gzip.push_back(static_cast<uint8_t>((crc >> 8) & 0xFF));
    gzip.push_back(static_cast<uint8_t>((crc >> 16) & 0xFF));
    gzip.push_back(static_cast<uint8_t>((crc >> 24) & 0xFF));

    // Trailer: ISIZE (4 bytes little-endian)
    gzip.push_back(static_cast<uint8_t>(isize & 0xFF));
    gzip.push_back(static_cast<uint8_t>((isize >> 8) & 0xFF));
    gzip.push_back(static_cast<uint8_t>((isize >> 16) & 0xFF));
    gzip.push_back(static_cast<uint8_t>((isize >> 24) & 0xFF));

    return gzip;
}

std::vector<uint8_t> decompressGzip(const std::vector<uint8_t>& gzipBytes) {
    if (gzipBytes.size() < 18) {
        throw std::runtime_error("Gzip payload too short (minimum 18 bytes)");
    }
    if (gzipBytes[0] != 0x1F || gzipBytes[1] != 0x8B || gzipBytes[2] != 0x08) {
        throw std::runtime_error("Invalid Gzip header magic bytes");
    }

    size_t trailerOffset = gzipBytes.size() - 8;
    uint32_t expectedCrc = static_cast<uint32_t>(gzipBytes[trailerOffset]) |
                          (static_cast<uint32_t>(gzipBytes[trailerOffset + 1]) << 8) |
                          (static_cast<uint32_t>(gzipBytes[trailerOffset + 2]) << 16) |
                          (static_cast<uint32_t>(gzipBytes[trailerOffset + 3]) << 24);
    uint32_t expectedSize = static_cast<uint32_t>(gzipBytes[trailerOffset + 4]) |
                           (static_cast<uint32_t>(gzipBytes[trailerOffset + 5]) << 8) |
                           (static_cast<uint32_t>(gzipBytes[trailerOffset + 6]) << 16) |
                           (static_cast<uint32_t>(gzipBytes[trailerOffset + 7]) << 24);

    std::vector<uint8_t> decompressed(expectedSize);
    mz_stream stream{};
    int status = mz_inflateInit2(&stream, -MZ_DEFAULT_WINDOW_BITS);
    if (status != MZ_OK) {
        throw std::runtime_error("mz_inflateInit2 failed with status " + std::to_string(status));
    }

    stream.next_in = gzipBytes.data() + 10;
    stream.avail_in = static_cast<unsigned int>(gzipBytes.size() - 18);
    stream.next_out = decompressed.data();
    stream.avail_out = static_cast<unsigned int>(decompressed.size());

    status = mz_inflate(&stream, MZ_FINISH);
    if (status != MZ_STREAM_END) {
        mz_inflateEnd(&stream);
        throw std::runtime_error("mz_inflate failed, status " + std::to_string(status));
    }
    mz_inflateEnd(&stream);

    mz_ulong actualCrc = mz_crc32(0, decompressed.data(), decompressed.size());
    if (actualCrc != expectedCrc) {
        throw std::runtime_error("CRC-32 checksum mismatch: expected " + std::to_string(expectedCrc) +
                                 ", got " + std::to_string(actualCrc));
    }

    return decompressed;
}

} // namespace

int main(int argc, char* argv[]) {
    std::filesystem::path inputPath = "frontend/dist/index.html";
    std::filesystem::path outputPath = "include/gamehub/web/WebAssets.hpp";

    if (argc >= 2) {
        inputPath = argv[1];
    }
    if (argc >= 3) {
        outputPath = argv[2];
    }

    if (!std::filesystem::exists(inputPath)) {
        std::cerr << "[ERROR] Input file does not exist: " << inputPath.string() << std::endl;
        std::cerr << "        Please build the ClientShell first (e.g. npm run build in frontend/)." << std::endl;
        return 1;
    }

    std::ifstream in(inputPath, std::ios::binary);
    if (!in) {
        std::cerr << "[ERROR] Failed to open input file: " << inputPath.string() << std::endl;
        return 1;
    }

    std::vector<uint8_t> rawBytes(
        (std::istreambuf_iterator<char>(in)),
        std::istreambuf_iterator<char>()
    );
    in.close();

    size_t uncompressedSize = rawBytes.size();
    std::vector<uint8_t> gzipBytes;
    try {
        gzipBytes = compressToGzip(rawBytes);
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Compression with miniz failed: " << e.what() << std::endl;
        return 1;
    }

    size_t compressedSize = gzipBytes.size();
    float ratio = 100.0f * (1.0f - static_cast<float>(compressedSize) / static_cast<float>(uncompressedSize));

    // Verify round-trip integrity immediately at pack time using miniz inflate
    try {
        std::vector<uint8_t> decompressed = decompressGzip(gzipBytes);
        if (decompressed != rawBytes) {
            std::cerr << "[ERROR] AssetPacker round-trip verification failed: decompressed payload does not match raw bytes!" << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] AssetPacker round-trip verification failed: " << e.what() << std::endl;
        return 1;
    }

    std::filesystem::create_directories(outputPath.parent_path());
    std::ofstream out(outputPath);
    if (!out) {
        std::cerr << "[ERROR] Failed to open output file for writing: " << outputPath.string() << std::endl;
        return 1;
    }

    out << "#pragma once\n\n";
    out << "#ifndef PROGMEM\n#define PROGMEM\n#endif\n\n";
    out << "#include <cstddef>\n";
    out << "#include <cstdint>\n";
    out << "#include <string>\n";
    out << "#include <string_view>\n\n";
    out << "namespace gamehub {\n";
    out << "namespace web {\n\n";
    out << "/**\n";
    out << " * Embedded RFC 1952 Gzip-compressed ClientShell single-page application.\n";
    out << " * Compressed using industry-standard miniz (RFC 1951 Deflate).\n";
    out << " * Flash memory footprint: " << compressedSize << " bytes (compressed from " << uncompressedSize << " bytes).\n";
    out << " */\n";
    out << "class WebAssets {\n";
    out << "public:\n";
    out << "    static constexpr size_t getIndexHtmlSize() {\n";
    out << "        return INDEX_HTML_SIZE;\n";
    out << "    }\n\n";
    out << "    static constexpr size_t getIndexHtmlGzipSize() {\n";
    out << "        return INDEX_HTML_SIZE;\n";
    out << "    }\n\n";
    out << "    static constexpr size_t getUncompressedSize() {\n";
    out << "        return UNCOMPRESSED_SIZE;\n";
    out << "    }\n\n";
    out << "    static constexpr bool isGzipped() {\n";
    out << "        return true;\n";
    out << "    }\n\n";
    out << "    /// Zero-copy pointer to PROGMEM flash-resident RFC 1952 Gzip byte payload\n";
    out << "    static const uint8_t* getIndexHtmlData() {\n";
    out << "        return INDEX_HTML_DATA;\n";
    out << "    }\n\n";
    out << "    /// Zero-copy pointer to PROGMEM flash-resident RFC 1952 Gzip byte payload\n";
    out << "    static const uint8_t* getIndexHtmlGzipData() {\n";
    out << "        return INDEX_HTML_DATA;\n";
    out << "    }\n\n";
    out << "    /// String-view over the compressed Gzip payload for HTTP response assembly\n";
    out << "    static std::string_view getGzipPayloadView() {\n";
    out << "        return std::string_view(reinterpret_cast<const char*>(INDEX_HTML_DATA), INDEX_HTML_SIZE);\n";
    out << "    }\n\n";
    out << "    /// String copy of the compressed Gzip payload\n";
    out << "    static std::string getGzipPayload() {\n";
    out << "        return std::string(reinterpret_cast<const char*>(INDEX_HTML_DATA), INDEX_HTML_SIZE);\n";
    out << "    }\n\n";
    out << "    /// Backwards compatibility accessor: returns Gzip byte string for Content-Encoding: gzip streaming\n";
    out << "    static std::string_view getIndexHtmlView() {\n";
    out << "        return getGzipPayloadView();\n";
    out << "    }\n\n";
    out << "    /// Backwards compatibility accessor: returns Gzip byte string for Content-Encoding: gzip streaming\n";
    out << "    static std::string getIndexHtml() {\n";
    out << "        return getGzipPayload();\n";
    out << "    }\n";
    out << "private:\n";
    out << "    static constexpr size_t UNCOMPRESSED_SIZE = " << uncompressedSize << ";\n";
    out << "    static constexpr size_t INDEX_HTML_SIZE = " << compressedSize << ";\n";
    out << "    static const uint8_t INDEX_HTML_DATA[" << compressedSize << "];\n";
    out << "};\n\n";
    out << "inline const uint8_t WebAssets::INDEX_HTML_DATA[" << compressedSize << "] = {\n";

    for (size_t i = 0; i < compressedSize; ++i) {
        if (i % 20 == 0) out << "    ";
        out << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(gzipBytes[i]);
        if (i + 1 < compressedSize) out << ", ";
        if ((i + 1) % 20 == 0 || i + 1 == compressedSize) out << "\n";
    }

    out << "};\n\n";
    out << "} // namespace web\n";
    out << "} // namespace gamehub\n";
    out.close();

    std::cout << "[AssetPacker (miniz)] Successfully packed: " << inputPath.string() << std::endl;
    std::cout << "                      Uncompressed: " << std::dec << uncompressedSize << " bytes" << std::endl;
    std::cout << "                      Gzip Payload: " << compressedSize << " bytes ("
              << std::fixed << std::setprecision(1) << ratio << "% flash reduction)" << std::endl;
    std::cout << "                      Round-trip  : Verified byte-for-byte exact match (miniz inflate)" << std::endl;
    std::cout << "                      Generated   : " << outputPath.string() << std::endl;

    return 0;
}
