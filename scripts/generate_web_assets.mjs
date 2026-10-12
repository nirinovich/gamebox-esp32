import fs from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

const CHUNK_SIZE = 8000;

function generateHeader(htmlContent) {
  const chunks = [];
  for (let i = 0; i < htmlContent.length; i += CHUNK_SIZE) {
    chunks.push(htmlContent.slice(i, i + CHUNK_SIZE));
  }
  if (chunks.length === 0) {
    chunks.push('');
  }

  const partDeclarations = [];
  const partNames = [];

  chunks.forEach((chunk, idx) => {
    const varName = `INDEX_HTML_PART${idx}`;
    partNames.push(varName);
    partDeclarations.push(
      `    static constexpr const char* ${varName} = R"rawliteral(${chunk})rawliteral";`
    );
  });

  const partsListStr = partNames.join(', ');
  const partsDeclStr = partDeclarations.join('\n');

  return `#pragma once

#ifndef PROGMEM
#define PROGMEM
#endif

#include <string>
#include <vector>

namespace gamehub {
namespace web {

class WebAssets {
public:
    static const std::vector<const char*>& getParts() {
        static const std::vector<const char*> parts = {
            ${partsListStr}
        };
        return parts;
    }

    static std::string getIndexHtml() {
        std::string res;
        for (const char* p : getParts()) {
            res += p;
        }
        return res;
    }

private:
${partsDeclStr}
};

} // namespace web
} // namespace gamehub
`;
}

function main() {
  const repoRoot = path.resolve(__dirname, '..');
  const distHtml = path.join(repoRoot, 'frontend', 'dist', 'index.html');
  const outputHeader = path.join(repoRoot, 'include', 'gamehub', 'web', 'WebAssets.hpp');

  if (!fs.existsSync(distHtml)) {
    console.error(`Error: ${distHtml} not found. Run 'npm run build' in frontend/ first.`);
    process.exit(1);
  }

  const content = fs.readFileSync(distHtml, 'utf-8');
  const totalBytes = Buffer.byteLength(content, 'utf-8');
  console.log(`Read ${totalBytes} bytes from ${distHtml}`);

  const headerCode = generateHeader(content);

  fs.mkdirSync(path.dirname(outputHeader), { recursive: true });
  fs.writeFileSync(outputHeader, headerCode, 'utf-8');

  const chunkCount = Math.ceil(totalBytes / CHUNK_SIZE);
  console.log(`Successfully generated ${outputHeader} with ${chunkCount} chunks.`);
}

main();
