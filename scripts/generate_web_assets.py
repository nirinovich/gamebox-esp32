#!/usr/bin/env python3
"""
generate_web_assets.py

Reads frontend/dist/index.html (produced by vite-plugin-singlefile)
and automatically formats it into chunked C++ raw string literals in
include/gamehub/web/WebAssets.hpp.

Chunk sizes are capped at 8000 bytes to strictly guarantee compatibility
with MSVC, GCC, and Xtensa-GCC without exceeding compiler string limits.
"""

import os
import sys

CHUNK_SIZE = 8000

def generate_header(html_content: str) -> str:
    # Split html_content into chunks
    chunks = [html_content[i:i + CHUNK_SIZE] for i in range(0, len(html_content), CHUNK_SIZE)]
    if not chunks:
        chunks = [""]

    part_declarations = []
    part_names = []

    for idx, chunk in enumerate(chunks):
        var_name = f"INDEX_HTML_PART{idx}"
        part_names.append(var_name)
        part_declarations.append(
            f'    static constexpr const char* {var_name} = R"rawliteral({chunk})rawliteral";'
        )

    parts_list_str = ", ".join(part_names)
    parts_decl_str = "\n".join(part_declarations)

    header_code = f"""#pragma once

#ifndef PROGMEM
#define PROGMEM
#endif

#include <string>
#include <vector>

namespace gamehub {{
namespace web {{

class WebAssets {{
public:
    static const std::vector<const char*>& getParts() {{
        static const std::vector<const char*> parts = {{
            {parts_list_str}
        }};
        return parts;
    }}

    static std::string getIndexHtml() {{
        std::string res;
        for (const char* p : getParts()) {{
            res += p;
        }}
        return res;
    }}

private:
{parts_decl_str}
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

    with open(dist_html, "r", encoding="utf-8") as f:
        content = f.read()

    total_bytes = len(content.encode("utf-8"))
    print(f"Read {total_bytes} bytes from {dist_html}")

    header_code = generate_header(content)

    os.makedirs(os.path.dirname(output_header), exist_ok=True)
    with open(output_header, "w", encoding="utf-8") as f:
        f.write(header_code)

    print(f"Successfully generated {output_header} with {(total_bytes + CHUNK_SIZE - 1) // CHUNK_SIZE} chunks.")

if __name__ == "__main__":
    main()
