#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <iomanip>
#include <cstdint>
#include <array>
#include <algorithm>

namespace {

// CRC-32 table generator (RFC 1952)
std::array<uint32_t, 256> generateCrcTable() {
    std::array<uint32_t, 256> table{};
    for (uint32_t i = 0; i < 256; ++i) {
        uint32_t c = i;
        for (int j = 0; j < 8; ++j) {
            if (c & 1) {
                c = 0xEDB88320U ^ (c >> 1);
            } else {
                c >>= 1;
            }
        }
        table[i] = c;
    }
    return table;
}

uint32_t calculateCrc32(const std::vector<uint8_t>& data) {
    static const auto crcTable = generateCrcTable();
    uint32_t c = 0xFFFFFFFFU;
    for (uint8_t byte : data) {
        c = crcTable[(c ^ byte) & 0xFF] ^ (c >> 8);
    }
    return c ^ 0xFFFFFFFFU;
}

// RFC 1951 Deflate Length & Distance Symbol Tables
struct CodeSymbol {
    uint16_t base;
    uint16_t code;
    uint8_t extraBits;
};

const CodeSymbol LENGTH_CODES[29] = {
    {3, 257, 0}, {4, 258, 0}, {5, 259, 0}, {6, 260, 0}, {7, 261, 0}, {8, 262, 0}, {9, 263, 0}, {10, 264, 0},
    {11, 265, 1}, {13, 266, 1}, {15, 267, 1}, {17, 268, 1},
    {19, 269, 2}, {23, 270, 2}, {27, 271, 2}, {31, 272, 2},
    {35, 273, 3}, {43, 274, 3}, {51, 275, 3}, {59, 276, 3},
    {67, 277, 4}, {83, 278, 4}, {99, 279, 4}, {115, 280, 4},
    {131, 281, 5}, {163, 282, 5}, {195, 283, 5}, {227, 284, 5},
    {258, 285, 0}
};

const CodeSymbol DIST_CODES[30] = {
    {1, 0, 0}, {2, 1, 0}, {3, 2, 0}, {4, 3, 0},
    {5, 4, 1}, {7, 5, 1},
    {9, 6, 2}, {13, 7, 2},
    {17, 8, 3}, {25, 9, 3},
    {33, 10, 4}, {49, 11, 4},
    {65, 12, 5}, {97, 13, 5},
    {129, 14, 6}, {193, 15, 6},
    {257, 16, 7}, {385, 17, 7},
    {513, 18, 8}, {769, 19, 8},
    {1025, 20, 9}, {1537, 21, 9},
    {2049, 22, 10}, {3073, 23, 10},
    {4097, 24, 11}, {6145, 25, 11},
    {8193, 26, 12}, {12289, 27, 12},
    {16385, 28, 13}, {24577, 29, 13}
};

inline uint32_t reverseBits(uint32_t code, int bits) {
    uint32_t r = 0;
    for (int i = 0; i < bits; ++i) {
        r = (r << 1) | (code & 1);
        code >>= 1;
    }
    return r;
}

class BitStream {
public:
    std::vector<uint8_t> buffer;
    uint32_t bitBuf{0};
    int bitCount{0};

    void write(uint32_t val, int bits) {
        bitBuf |= (val << bitCount);
        bitCount += bits;
        while (bitCount >= 8) {
            buffer.push_back(static_cast<uint8_t>(bitBuf & 0xFF));
            bitBuf >>= 8;
            bitCount -= 8;
        }
    }

    void flush() {
        if (bitCount > 0) {
            buffer.push_back(static_cast<uint8_t>(bitBuf & 0xFF));
            bitBuf = 0;
            bitCount = 0;
        }
    }

    void writeLiteral(uint32_t val) {
        if (val < 144) {
            write(reverseBits(0x30 + val, 8), 8);
        } else if (val < 256) {
            write(reverseBits(0x190 + (val - 144), 9), 9);
        } else if (val == 256) {
            write(reverseBits(0x00, 7), 7); // End of block
        } else if (val <= 279) {
            write(reverseBits(val - 256, 7), 7);
        } else {
            write(reverseBits(0xC0 + (val - 280), 8), 8);
        }
    }

    void writeDistance(uint32_t distCode) {
        write(reverseBits(distCode, 5), 5);
    }
};

std::vector<uint8_t> compressGzip(const std::vector<uint8_t>& data) {
    if (data.empty()) {
        return {};
    }

    BitStream bs;
    // BFINAL=1, BTYPE=01 (Fixed Huffman block: 3 bits with value 3 = 0b011)
    bs.write(3, 3);

    size_t n = data.size();
    size_t pos = 0;
    std::vector<int> head(65536, -1);
    std::vector<int> prev(n, -1);

    while (pos < n) {
        int bestLen = 0;
        int bestDist = 0;

        if (pos + 2 < n) {
            uint32_t h = ((data[pos] << 8) ^ (data[pos + 1] << 4) ^ data[pos + 2]) & 0xFFFF;
            int m = head[h];
            int chainLen = 0;
            while (m != -1 && (pos - m) <= 32768 && chainLen < 64) {
                int dist = static_cast<int>(pos - m);
                int curLen = 0;
                while (pos + curLen < n && curLen < 258 && data[pos + curLen] == data[m + curLen]) {
                    curLen++;
                }
                if (curLen > bestLen && curLen >= 3) {
                    bestLen = curLen;
                    bestDist = dist;
                    if (curLen == 258) break;
                }
                m = prev[m];
                chainLen++;
            }
            prev[pos] = head[h];
            head[h] = static_cast<int>(pos);
        }

        if (bestLen >= 3) {
            // Find length code
            uint16_t lCode = 285;
            uint16_t lExtra = 0;
            uint8_t lExtraBits = 0;
            for (int i = 28; i >= 0; --i) {
                if (bestLen >= LENGTH_CODES[i].base) {
                    lCode = LENGTH_CODES[i].code;
                    lExtra = static_cast<uint16_t>(bestLen - LENGTH_CODES[i].base);
                    lExtraBits = LENGTH_CODES[i].extraBits;
                    break;
                }
            }
            bs.writeLiteral(lCode);
            if (lExtraBits > 0) {
                bs.write(lExtra, lExtraBits);
            }

            // Find distance code
            uint16_t dCode = 0;
            uint16_t dExtra = 0;
            uint8_t dExtraBits = 0;
            for (int i = 29; i >= 0; --i) {
                if (bestDist >= DIST_CODES[i].base) {
                    dCode = DIST_CODES[i].code;
                    dExtra = static_cast<uint16_t>(bestDist - DIST_CODES[i].base);
                    dExtraBits = DIST_CODES[i].extraBits;
                    break;
                }
            }
            bs.writeDistance(dCode);
            if (dExtraBits > 0) {
                bs.write(dExtra, dExtraBits);
            }

            for (size_t p = pos + 1; p < std::min(pos + bestLen, n - 2); ++p) {
                uint32_t h = ((data[p] << 8) ^ (data[p + 1] << 4) ^ data[p + 2]) & 0xFFFF;
                prev[p] = head[h];
                head[h] = static_cast<int>(p);
            }
            pos += bestLen;
        } else {
            bs.writeLiteral(data[pos]);
            pos++;
        }
    }

    // End of block marker (code 256)
    bs.writeLiteral(256);
    bs.flush();

    // Assemble Gzip output: 10-byte header + deflate stream + 8-byte trailer
    std::vector<uint8_t> gzip;
    gzip.reserve(10 + bs.buffer.size() + 8);

    // Gzip Header
    gzip.push_back(0x1F); // ID1
    gzip.push_back(0x8B); // ID2
    gzip.push_back(0x08); // Deflate
    gzip.push_back(0x00); // Flags
    gzip.push_back(0x00); gzip.push_back(0x00); gzip.push_back(0x00); gzip.push_back(0x00); // MTIME
    gzip.push_back(0x02); // Maximum compression flag
    gzip.push_back(0x03); // Unix OS

    // Deflate stream
    gzip.insert(gzip.end(), bs.buffer.begin(), bs.buffer.end());

    // Trailer: CRC-32 (LE) + ISIZE (LE)
    uint32_t crc = calculateCrc32(data);
    uint32_t isize = static_cast<uint32_t>(data.size());

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
        std::cerr << "        Please build the frontend first (e.g. npm run build in frontend/)." << std::endl;
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
    std::vector<uint8_t> gzipBytes = compressGzip(rawBytes);
    size_t compressedSize = gzipBytes.size();
    float ratio = 100.0f * (1.0f - static_cast<float>(compressedSize) / static_cast<float>(uncompressedSize));

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
    out << "#include <string_view>\n";
    out << "#include <vector>\n\n";
    out << "namespace gamehub {\n";
    out << "namespace web {\n\n";
    out << "class WebAssets {\n";
    out << "public:\n";
    out << "    static constexpr size_t getIndexHtmlSize() {\n";
    out << "        return INDEX_HTML_SIZE;\n";
    out << "    }\n\n";
    out << "    static constexpr size_t getUncompressedSize() {\n";
    out << "        return UNCOMPRESSED_SIZE;\n";
    out << "    }\n\n";
    out << "    static constexpr bool isGzipped() {\n";
    out << "        return true;\n";
    out << "    }\n\n";
    out << "    static const uint8_t* getIndexHtmlData() {\n";
    out << "        return INDEX_HTML_DATA;\n";
    out << "    }\n\n";
    out << "    static std::string_view getIndexHtmlView() {\n";
    out << "        return std::string_view(reinterpret_cast<const char*>(INDEX_HTML_DATA), INDEX_HTML_SIZE);\n";
    out << "    }\n\n";
    out << "    static std::string getIndexHtml() {\n";
    out << "        return std::string(reinterpret_cast<const char*>(INDEX_HTML_DATA), INDEX_HTML_SIZE);\n";
    out << "    }\n\n";
    out << "    static const std::vector<const char*>& getParts() {\n";
    out << "        static const std::vector<const char*> parts;\n";
    out << "        return parts;\n";
    out << "    }\n\n";
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
    std::cout << "              Generated   : " << outputPath.string() << std::endl;

    return 0;
}
