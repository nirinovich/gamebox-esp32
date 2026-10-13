#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <iomanip>
#include <cstdint>
#include <cassert>
#include <gamehub/util/GzipCompressor.hpp>

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
    std::vector<uint8_t> gzipBytes = gamehub::util::GzipCompressor::compress(rawBytes);
    size_t compressedSize = gzipBytes.size();
    float ratio = 100.0f * (1.0f - static_cast<float>(compressedSize) / static_cast<float>(uncompressedSize));

    // Verify round-trip integrity immediately at pack time
    std::vector<uint8_t> decompressed = gamehub::util::GzipCompressor::decompress(gzipBytes);
    if (decompressed != rawBytes) {
        std::cerr << "[ERROR] AssetPacker round-trip verification failed: decompressed payload does not match raw bytes!" << std::endl;
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

    std::cout << "[AssetPacker] Successfully packed: " << inputPath.string() << std::endl;
    std::cout << "              Uncompressed: " << std::dec << uncompressedSize << " bytes" << std::endl;
    std::cout << "              Gzip Payload: " << compressedSize << " bytes ("
              << std::fixed << std::setprecision(1) << ratio << "% flash reduction)" << std::endl;
    std::cout << "              Round-trip  : Verified byte-for-byte exact match" << std::endl;
    std::cout << "              Generated   : " << outputPath.string() << std::endl;

    return 0;
}
