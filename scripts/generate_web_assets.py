#!/usr/bin/env python3
"""
generate_web_assets.py

Reads frontend/dist/index.html (produced by vite-plugin-singlefile)
and automatically formats it into a contiguous binary byte array
in include/gamehub/web/WebAssets.hpp.

This avoids C++ compiler string literal length limits (MSVC C2026),
enables zero-copy streaming via ESPAsyncWebServer request->send(),
and requires no string chunking loops.
"""

import os
import sys

def generate_header(raw_bytes: bytes) -> str:
    lines = []
    line_bytes = []
    for b in raw_bytes:
        line_bytes.append(f"0x{b:02x}")
        if len(line_bytes) == 20:
            lines.append("    " + ", ".join(line_bytes))
            line_bytes = []
    if line_bytes:
        lines.append("    " + ", ".join(line_bytes))
    bytes_str = ",\n".join(lines)
    total_len = len(raw_bytes)

    header_code = f"""#pragma once

#ifndef PROGMEM
#define PROGMEM
#endif

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace gamehub {{
namespace web {{

class WebAssets {{
public:
    static constexpr size_t getIndexHtmlSize() {{
        return INDEX_HTML_SIZE;
    }}

    static const uint8_t* getIndexHtmlData() {{
        return INDEX_HTML_DATA;
    }}

    static std::string_view getIndexHtmlView() {{
        return std::string_view(reinterpret_cast<const char*>(INDEX_HTML_DATA), INDEX_HTML_SIZE);
    }}

    static std::string getIndexHtml() {{
        return std::string(reinterpret_cast<const char*>(INDEX_HTML_DATA), INDEX_HTML_SIZE);
    }}

    static const std::vector<const char*>& getParts() {{
        static const std::vector<const char*> parts;
        return parts;
    }}

private:
    static constexpr size_t INDEX_HTML_SIZE = {total_len};
    static const uint8_t INDEX_HTML_DATA[{total_len}];
}};

inline const uint8_t WebAssets::INDEX_HTML_DATA[{total_len}] = {{
{bytes_str}
}};

}} // namespace web
}} // namespace gamehub
"""
    return header_code

def main():
    repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
    dist_html = os.path.join(repo_root, "frontend", "dist", "index.html")
    output_header = os.path.join(repo_root, "include", "gamehub", "web", "WebAssets.hpp")

    if not os.path.exists(dist_html):
        print(f"Error: {dist_html} not found. Run 'npm run build' in frontend/ first.", file=sys.stderr)
        sys.exit(1)

    with open(dist_html, "rb") as f:
        raw_bytes = f.read()

    total_bytes = len(raw_bytes)
    print(f"Read {total_bytes} bytes from {dist_html}")

    header_code = generate_header(raw_bytes)

    os.makedirs(os.path.dirname(output_header), exist_ok=True)
    with open(output_header, "w", encoding="utf-8") as f:
        f.write(header_code)

    print(f"Successfully generated contiguous binary array in {output_header} ({total_bytes} bytes).")

if __name__ == "__main__":
    main()
