import { defineConfig } from 'vite';
import preact from '@preact/preset-vite';
import { viteSingleFile } from 'vite-plugin-singlefile';
import path from 'path';
import fs from 'fs';

const version = fs.readFileSync(path.resolve(__dirname, '../VERSION'), 'utf-8').trim();

export default defineConfig({
  plugins: [
    preact(),
    viteSingleFile()
  ],
  define: {
    __GAMEBOX_VERSION__: JSON.stringify(version)
  },
  resolve: {
    alias: {
      '@games': path.resolve(__dirname, '../games')
    }
  },
  build: {
    target: 'es2020',
    cssCodeSplit: false,
    assetsInlineLimit: 100000000,
    chunkSizeWarningLimit: 1000,
    minify: 'esbuild'
  },
  server: {
    port: 5173,
    fs: {
      allow: ['..']
    },
    proxy: {
      '/ws': {
        target: 'ws://localhost:8080',
        ws: true,
        changeOrigin: true
      }
    }
  },
  test: {
    include: ['src/**/*.{test,spec}.ts', '../games/*/client/**/*.{test,spec}.ts']
  }
});
