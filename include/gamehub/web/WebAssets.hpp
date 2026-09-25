#pragma once

#ifndef PROGMEM
#define PROGMEM
#endif

#include <string>

namespace gamehub {
namespace web {

class WebAssets {
public:
    static std::string getIndexHtml() {
        return std::string(INDEX_HTML_PART1) + INDEX_HTML_PART2 + INDEX_HTML_PART3;
    }

private:
    static constexpr const char* INDEX_HTML_PART1 = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>ESP32 Game Hub</title>
    <style>
        :root {
            --bg-color: #0f172a;
            --panel-bg: #1e293b;
            --accent: #38bdf8;
            --accent-hover: #0284c7;
            --text-color: #f8fafc;
            --text-muted: #94a3b8;
            --success: #22c55e;
            --danger: #ef4444;
        }
        * { box-sizing: border-box; margin: 0; padding: 0; user-select: none; -webkit-user-select: none; }
        body {
            background-color: var(--bg-color);
            color: var(--text-color);
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
            display: flex;
            flex-direction: column;
            align-items: center;
            min-height: 100vh;
            padding: 12px;
        }
        header {
            width: 100%;
            max-width: 480px;
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding: 8px 12px;
            background: var(--panel-bg);
            border-radius: 12px;
            margin-bottom: 12px;
            box-shadow: 0 4px 6px -1px rgba(0, 0, 0, 0.1);
        }
        .logo { font-weight: 700; font-size: 1.1rem; color: var(--accent); }
        .badge {
            font-size: 0.75rem;
            padding: 4px 8px;
            border-radius: 9999px;
            background: #334155;
            color: var(--text-muted);
            display: flex;
            align-items: center;
            gap: 6px;
        }
        .badge.connected { background: rgba(34, 197, 94, 0.2); color: var(--success); }
        .dot { width: 6px; height: 6px; border-radius: 50%; background: currentColor; }
        #app-container {
            width: 100%;
            max-width: 480px;
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: 12px;
        }
        #game-canvas {
            background: #000;
            border: 2px solid #334155;
            border-radius: 12px;
            width: 100%;
            max-width: 400px;
            aspect-ratio: 1 / 1;
            box-shadow: 0 10px 15px -3px rgba(0, 0, 0, 0.3);
        }
        .menu-card {
            background: var(--panel-bg);
            border-radius: 12px;
            padding: 16px;
            width: 100%;
            display: flex;
            flex-direction: column;
            gap: 10px;
        }
        .btn {
            background: var(--accent);
            color: #0f172a;
            border: none;
            border-radius: 8px;
            padding: 12px 16px;
            font-weight: 600;
            font-size: 1rem;
            cursor: pointer;
            transition: all 0.15s ease;
            text-align: center;
        }
        .btn:active { transform: scale(0.98); background: var(--accent-hover); }
        .btn-secondary { background: #334155; color: var(--text-color); }
        .btn-secondary:active { background: #475569; }
        .dpad-container {
            display: grid;
            grid-template-columns: repeat(3, 60px);
            grid-template-rows: repeat(3, 60px);
            gap: 8px;
            justify-content: center;
            margin-top: 8px;
        }
        .dpad-btn {
            background: #334155;
            color: var(--text-color);
            border: none;
            border-radius: 10px;
            font-size: 1.5rem;
            display: flex;
            align-items: center;
            justify-content: center;
            cursor: pointer;
            touch-action: manipulation;
        }
        .dpad-btn:active { background: var(--accent); color: #0f172a; }
        .dpad-up { grid-column: 2; grid-row: 1; }
        .dpad-left { grid-column: 1; grid-row: 2; }
        .dpad-down { grid-column: 2; grid-row: 2; }
        .dpad-right { grid-column: 3; grid-row: 2; }
        .input-group { display: flex; gap: 8px; width: 100%; margin-top: 4px; }
        .input-code {
            flex: 1;
            padding: 10px 14px;
            border-radius: 8px;
            border: 1px solid #475569;
            background: #0f172a;
            color: #f8fafc;
            font-size: 1rem;
            text-transform: uppercase;
            letter-spacing: 2px;
            font-weight: 700;
            text-align: center;
        }
        .section-title { font-size: 0.75rem; font-weight: 700; color: var(--text-muted); text-transform: uppercase; letter-spacing: 1px; margin-top: 6px; }
        #latency-display { font-size: 0.75rem; color: var(--text-muted); text-align: center; }
    </style>
</head>
<body>
    <header>
        <div class="logo">ESP32 Game Hub</div>
        <div id="status-badge" class="badge">
            <span class="dot"></span>
            <span id="status-text">Connecting...</span>
        </div>
    </header>

    <div id="app-container">
        <canvas id="game-canvas" width="400" height="400"></canvas>
        <div id="latency-display">Ping: -- ms</div>

        <div id="menu-view" class="menu-card">
            <div class="section-title">Solo Modes</div>
            <button id="btn-solo-snake" class="btn">Play Solo Snake</button>
            <button id="btn-solo-ttt" class="btn btn-secondary">Play Solo Tic-Tac-Toe</button>

            <div class="section-title">Multiplayer 1v1 (Room Codes)</div>
            <button id="btn-mp-ttt" class="btn" style="background:#818cf8; color:#0f172a;">Host 2P Tic-Tac-Toe</button>
            <button id="btn-mp-snake" class="btn" style="background:#a855f7; color:#0f172a;">Host 2P Snake Duel</button>
            <div class="input-group">
                <input id="room-code-input" class="input-code" placeholder="CODE" maxlength="4">
                <button id="btn-join-room" class="btn btn-secondary" style="white-space:nowrap;">Join Room</button>
            </div>
            <div id="room-info" style="display:none; text-align:center; padding:10px; background:#334155; border-radius:8px; font-weight:bold; color:var(--accent);"></div>
        </div>

        <div class="dpad-container" id="touch-controls">
            <button class="dpad-btn dpad-up" data-dir="UP">&#9650;</button>
            <button class="dpad-btn dpad-left" data-dir="LEFT">&#9664;</button>
            <button class="dpad-btn dpad-down" data-dir="DOWN">&#9660;</button>
            <button class="dpad-btn dpad-right" data-dir="RIGHT">&#9654;</button>
        </div>
    </div>
)rawliteral";

    static constexpr const char* INDEX_HTML_PART2 = R"rawliteral(
    <script>
        const canvas = document.getElementById('game-canvas');
        const ctx = canvas.getContext('2d');
        const statusBadge = document.getElementById('status-badge');
        const statusText = document.getElementById('status-text');
        const latencyDisplay = document.getElementById('latency-display');
        const btnSoloSnake = document.getElementById('btn-solo-snake');
        const btnSoloTtt = document.getElementById('btn-solo-ttt');
        const btnMpTtt = document.getElementById('btn-mp-ttt');
        const btnMpSnake = document.getElementById('btn-mp-snake');
        const roomCodeInput = document.getElementById('room-code-input');
        const btnJoinRoom = document.getElementById('btn-join-room');
        const roomInfo = document.getElementById('room-info');

        let ws = null;
        let pingInterval = null;
        let lastPingSent = 0;
        let currentGame = null;
        let myPlayerNum = 1;
        let currentRoomCode = null;

        function connectWebSocket() {
            const proto = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
            const host = window.location.host || 'localhost:8080';
            ws = new WebSocket(`${proto}//${host}/ws`);

            ws.onopen = () => {
                statusBadge.classList.add('connected');
                statusText.textContent = 'Connected';
                renderSplash("READY TO PLAY", "Select a solo or multiplayer mode below");
                startPing();
            };

            ws.onmessage = (event) => {
                try {
                    const data = JSON.parse(event.data);
                    if (data.type === 'pong') {
                        const rtt = Date.now() - data.ts;
                        latencyDisplay.textContent = `Ping: ${rtt} ms`;
                    } else if (data.type === 'room_created') {
                        currentRoomCode = data.code;
                        myPlayerNum = data.player;
                        roomInfo.style.display = 'block';
                        roomInfo.textContent = `Room Code: ${data.code} (Waiting for Player 2...)`;
                        renderSplash(`ROOM ${data.code}`, "Share code with Player 2 to begin");
                    } else if (data.type === 'room_joined') {
                        currentRoomCode = data.code;
                        myPlayerNum = data.player;
                        roomInfo.style.display = 'block';
                        roomInfo.textContent = `Joined Room ${data.code}! Starting...`;
                    } else if (data.type === 'room_ready') {
                        roomInfo.textContent = `Room ${currentRoomCode || ''} - In Progress!`;
                    } else if (data.type === 'opponent_left') {
                        roomInfo.textContent = 'Opponent disconnected.';
                        renderSplash("OPPONENT LEFT", "Your opponent disconnected. Room closed.");
                        currentGame = null;
                        clearInterval(gameLoop);
                    } else if (data.type === 'error') {
                        alert(data.msg);
                    } else if (data.type === 'state') {
                        if (data.game === 'snake') {
                            currentGame = 'snake';
                            renderSnake(data);
                        } else if (data.game === 'tictactoe') {
                            currentGame = 'ttt';
                            renderTicTacToe(data);
                        }
                    }
                } catch (e) {
                    console.error("Failed to parse server message", e);
                }
            };

            ws.onclose = () => {
                statusBadge.classList.remove('connected');
                statusText.textContent = 'Disconnected';
                renderSplash("DISCONNECTED", "Reconnecting to Host...");
                clearInterval(pingInterval);
                setTimeout(connectWebSocket, 2000);
            };

            ws.onerror = (err) => {
                console.error("WebSocket error:", err);
            };
        }

        function startPing() {
            clearInterval(pingInterval);
            pingInterval = setInterval(() => {
                if (ws && ws.readyState === WebSocket.OPEN) {
                    lastPingSent = Date.now();
                    ws.send(JSON.stringify({ type: 'ping', ts: lastPingSent }));
                }
            }, 1000);
        }

        function sendDirection(dir) {
            if (ws && ws.readyState === WebSocket.OPEN) {
                ws.send(JSON.stringify({ cmd: 'dir', val: dir }));
            }
        }

        function renderSplash(title, subtitle) {
            ctx.fillStyle = '#0f172a';
            ctx.fillRect(0, 0, canvas.width, canvas.height);
            ctx.fillStyle = '#38bdf8';
            ctx.font = 'bold 22px sans-serif';
            ctx.textAlign = 'center';
            ctx.fillText(title, canvas.width / 2, canvas.height / 2 - 10);
            ctx.fillStyle = '#94a3b8';
            ctx.font = '14px sans-serif';
            ctx.fillText(subtitle, canvas.width / 2, canvas.height / 2 + 20);
        }

        function renderSnake(state) {
            const gridCount = 20;
            const cellSize = canvas.width / gridCount;

            // Clear background
            ctx.fillStyle = '#090d16';
            ctx.fillRect(0, 0, canvas.width, canvas.height);

            // Draw subtle grid lines
            ctx.strokeStyle = '#1e293b';
            ctx.lineWidth = 0.5;
            for (let i = 0; i <= gridCount; i++) {
                ctx.beginPath();
                ctx.moveTo(i * cellSize, 0);
                ctx.lineTo(i * cellSize, canvas.height);
                ctx.stroke();
                ctx.beginPath();
                ctx.moveTo(0, i * cellSize);
                ctx.lineTo(canvas.width, i * cellSize);
                ctx.stroke();
            }

            // Draw Apple / Food
            if (state.food) {
                ctx.fillStyle = '#ef4444';
                ctx.beginPath();
                const fx = state.food[0] * cellSize + cellSize / 2;
                const fy = state.food[1] * cellSize + cellSize / 2;
                ctx.arc(fx, fy, cellSize / 2 - 2, 0, Math.PI * 2);
                ctx.fill();
            }

            // Draw Snake 1 Body (Cyan / Green)
            if (state.body && state.body.length > 0) {
                state.body.forEach((seg, idx) => {
                    ctx.fillStyle = idx === 0 ? '#38bdf8' : '#22c55e';
                    const rx = seg[0] * cellSize + 1;
                    const ry = seg[1] * cellSize + 1;
                    const rw = cellSize - 2;
                    const rh = cellSize - 2;
                    ctx.fillRect(rx, ry, rw, rh);
                });
            }

            // Draw Snake 2 Body (Magenta / Purple)
            if (state.body2 && state.body2.length > 0) {
                state.body2.forEach((seg, idx) => {
                    ctx.fillStyle = idx === 0 ? '#ec4899' : '#a855f7';
                    const rx = seg[0] * cellSize + 1;
                    const ry = seg[1] * cellSize + 1;
                    const rw = cellSize - 2;
                    const rh = cellSize - 2;
                    ctx.fillRect(rx, ry, rw, rh);
                });
            }

            // HUD Score Overlay
            ctx.fillStyle = '#ffffff';
            ctx.font = 'bold 13px sans-serif';
            ctx.textAlign = 'left';
            if (state.duel) {
                const p1Label = myPlayerNum === 1 ? 'P1 (You)' : 'P1';
                const p2Label = myPlayerNum === 2 ? 'P2 (You)' : 'P2';
                ctx.fillText(`${p1Label}: ${state.score || 0}  |  ${p2Label}: ${state.score2 || 0}`, 10, 20);
            } else {
                ctx.fillText(`Score: ${state.score || 0}`, 10, 20);
            }

            // Game Over Overlay
            if (state.finished) {
                ctx.fillStyle = 'rgba(15, 23, 42, 0.85)';
                ctx.fillRect(0, 0, canvas.width, canvas.height);

                let title = "GAME OVER";
                let titleColor = "#ef4444";
                let subMsg = `Final Score: ${state.score || 0}`;

                if (state.duel) {
                    if (state.winner === myPlayerNum) {
                        title = "VICTORY!";
                        titleColor = "#22c55e";
                        subMsg = "You won the Snake duel!";
                    } else if (state.winner === -1) {
                        title = "DRAW GAME!";
                        titleColor = "#eab308";
                        subMsg = "Both snakes collided simultaneously.";
                    } else {
                        title = "DEFEAT!";
                        titleColor = "#ef4444";
                        subMsg = `Player ${state.winner} won the duel.`;
                    }
                }

                ctx.fillStyle = titleColor;
                ctx.font = 'bold 28px sans-serif';
                ctx.textAlign = 'center';
                ctx.fillText(title, canvas.width / 2, canvas.height / 2 - 15);

                ctx.fillStyle = '#f8fafc';
                ctx.font = '15px sans-serif';
                ctx.fillText(subMsg, canvas.width / 2, canvas.height / 2 + 15);

                ctx.fillStyle = '#94a3b8';
                ctx.font = '13px sans-serif';
                ctx.fillText("Click Restart below to play again", canvas.width / 2, canvas.height / 2 + 45);

                btnSoloSnake.textContent = "Restart Solo Snake";
                clearInterval(gameLoop);
                currentGame = null;
            } else {
                btnSoloSnake.textContent = state.duel ? "Playing Duel..." : "Playing Snake...";
            }
        }

        function renderTicTacToe(state) {
            ctx.fillStyle = '#090d16';
            ctx.fillRect(0, 0, canvas.width, canvas.height);

            const cellSize = canvas.width / 3;

            // Draw 3x3 grid lines
            ctx.strokeStyle = '#38bdf8';
            ctx.lineWidth = 4;
            for (let i = 1; i < 3; i++) {
                ctx.beginPath();
                ctx.moveTo(i * cellSize, 20);
                ctx.lineTo(i * cellSize, canvas.height - 20);
                ctx.stroke();

                ctx.beginPath();
                ctx.moveTo(20, i * cellSize);
                ctx.lineTo(canvas.width - 20, i * cellSize);
                ctx.stroke();
            }

            // Draw Markers
            if (state.board) {
                state.board.forEach((val, idx) => {
                    const row = Math.floor(idx / 3);
                    const col = idx % 3;
                    const cx = col * cellSize + cellSize / 2;
                    const cy = row * cellSize + cellSize / 2;

                    if (val === 1) { // X
                        ctx.strokeStyle = '#38bdf8';
                        ctx.lineWidth = 6;
                        ctx.beginPath();
                        ctx.moveTo(cx - 30, cy - 30);
                        ctx.lineTo(cx + 30, cy + 30);
                        ctx.moveTo(cx + 30, cy - 30);
                        ctx.lineTo(cx - 30, cy + 30);
                        ctx.stroke();
                    } else if (val === 2) { // O
                        ctx.strokeStyle = '#ef4444';
                        ctx.lineWidth = 6;
                        ctx.beginPath();
                        ctx.arc(cx, cy, 32, 0, Math.PI * 2);
                        ctx.stroke();
                    }
                });
            }

            // Status / Turn Text
            ctx.fillStyle = '#f8fafc';
            ctx.font = '14px sans-serif';
            ctx.textAlign = 'left';
            if (!state.finished) {
                ctx.fillText(`Turn: ${state.turn === 1 ? 'Player (X)' : 'Bot (O)'}`, 12, 18);
                btnSoloTtt.textContent = "Playing Tic-Tac-Toe...";
            }

            if (state.finished) {
                ctx.fillStyle = 'rgba(15, 23, 42, 0.85)';
                ctx.fillRect(0, 0, canvas.width, canvas.height);

                ctx.fillStyle = state.winner === 1 ? '#22c55e' : (state.winner === 2 ? '#ef4444' : '#eab308');
                ctx.font = 'bold 28px sans-serif';
                ctx.textAlign = 'center';
                const msg = state.winner === 1 ? 'YOU WIN!' : (state.winner === 2 ? 'BOT WINS!' : 'DRAW GAME!');
                ctx.fillText(msg, canvas.width / 2, canvas.height / 2 - 10);

                ctx.fillStyle = '#94a3b8';
                ctx.font = '14px sans-serif';
                ctx.fillText("Click Restart below to play again", canvas.width / 2, canvas.height / 2 + 30);
                btnSoloTtt.textContent = "Restart Solo Tic-Tac-Toe";
                currentGame = null;
            }
        }
)rawliteral";

    static constexpr const char* INDEX_HTML_PART3 = R"rawliteral(
        let gameLoop = null;
        function startGameLoop() {
            clearInterval(gameLoop);
            gameLoop = setInterval(() => {
                if (ws && ws.readyState === WebSocket.OPEN && currentGame === 'snake') {
                    ws.send(JSON.stringify({ cmd: 'tick' }));
                }
            }, 66); // 15 Hz
        }

        // Button Listeners
        btnSoloSnake.addEventListener('click', () => {
            if (ws && ws.readyState === WebSocket.OPEN) {
                currentGame = 'snake';
                ws.send(JSON.stringify({ cmd: 'start', game: 'snake_solo' }));
                btnSoloSnake.textContent = "Restarting...";
                startGameLoop();
            }
        });

        btnSoloTtt.addEventListener('click', () => {
            if (ws && ws.readyState === WebSocket.OPEN) {
                clearInterval(gameLoop);
                currentGame = 'ttt';
                ws.send(JSON.stringify({ cmd: 'start', game: 'ttt_solo' }));
                btnSoloTtt.textContent = "Starting...";
            }
        });

        btnMpTtt.addEventListener('click', () => {
            if (ws && ws.readyState === WebSocket.OPEN) {
                clearInterval(gameLoop);
                currentGame = 'ttt';
                ws.send(JSON.stringify({ cmd: 'create', game: 'ttt_pvp' }));
            }
        });

        btnMpSnake.addEventListener('click', () => {
            if (ws && ws.readyState === WebSocket.OPEN) {
                clearInterval(gameLoop);
                currentGame = 'snake';
                ws.send(JSON.stringify({ cmd: 'create', game: 'snake_duel' }));
            }
        });

        btnJoinRoom.addEventListener('click', () => {
            const code = roomCodeInput.value.trim().toUpperCase();
            if (code && ws && ws.readyState === WebSocket.OPEN) {
                clearInterval(gameLoop);
                ws.send(JSON.stringify({ cmd: 'join', code: code }));
            }
        });

        // Canvas Click for Tic-Tac-Toe
        canvas.addEventListener('click', (e) => {
            if (currentGame === 'ttt' && ws && ws.readyState === WebSocket.OPEN) {
                const rect = canvas.getBoundingClientRect();
                const x = e.clientX - rect.left;
                const y = e.clientY - rect.top;
                const scaleX = canvas.width / rect.width;
                const scaleY = canvas.height / rect.height;
                const canvasX = x * scaleX;
                const canvasY = y * scaleY;
                const cellSize = canvas.width / 3;
                const col = Math.floor(canvasX / cellSize);
                const row = Math.floor(canvasY / cellSize);
                if (col >= 0 && col < 3 && row >= 0 && row < 3) {
                    const cell = row * 3 + col;
                    ws.send(JSON.stringify({ cmd: 'place', cell: cell }));
                }
            }
        });

        // Touch D-Pad Listeners
        document.querySelectorAll('.dpad-btn').forEach(btn => {
            const dir = btn.getAttribute('data-dir');
            btn.addEventListener('pointerdown', (e) => {
                e.preventDefault();
                sendDirection(dir);
            });
        });

        // Keyboard Controls
        window.addEventListener('keydown', (e) => {
            let dir = null;
            if (e.key === 'ArrowUp' || e.key === 'w' || e.key === 'W') dir = 'UP';
            else if (e.key === 'ArrowDown' || e.key === 's' || e.key === 'S') dir = 'DOWN';
            else if (e.key === 'ArrowLeft' || e.key === 'a' || e.key === 'A') dir = 'LEFT';
            else if (e.key === 'ArrowRight' || e.key === 'd' || e.key === 'D') dir = 'RIGHT';

            if (dir) {
                e.preventDefault();
                sendDirection(dir);
            }
        });

        renderSplash("ESP32 GAME HUB", "Connecting to server...");
        connectWebSocket();
    </script>
</body>
</html>
)rawliteral";
};

} // namespace web
} // namespace gamehub
