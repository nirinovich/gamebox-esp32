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
        return std::string(INDEX_HTML_PART1) + INDEX_HTML_PART2 + INDEX_HTML_PART3 + 
               INDEX_HTML_PART4 + INDEX_HTML_PART5 + INDEX_HTML_PART6;
    }

private:
    static constexpr const char* INDEX_HTML_PART1 = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>Gamebox</title>
    <style>
        :root {
            --bg: #090d16;
            --surface: #111827;
            --surface-hover: #1f293d;
            --surface-active: #27354f;
            --border: #1e293b;
            --border-highlight: #334155;
            --p1-cyan: #38bdf8;
            --p2-pink: #ec4899;
            --bot-amber: #f59e0b;
            --danger: #ef4444;
            --success: #22c55e;
            --text: #f1f5f9;
            --text-secondary: #94a3b8;
            --text-tertiary: #64748b;
            --radius-lg: 12px;
            --radius-md: 8px;
            --radius-sm: 6px;
            --ease: cubic-bezier(0.16, 1, 0.3, 1);
        }
        * { box-sizing: border-box; margin: 0; padding: 0; user-select: none; -webkit-user-select: none; -webkit-tap-highlight-color: transparent; }
        body {
            background: var(--bg);
            color: var(--text);
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
            display: flex;
            flex-direction: column;
            align-items: center;
            min-height: 100vh;
            padding: 12px;
            overflow-x: hidden;
        }
        header {
            width: 100%;
            max-width: 440px;
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding: 10px 14px;
            background: var(--surface);
            border: 1px solid var(--border);
            border-radius: var(--radius-lg);
            margin-bottom: 12px;
        }
        .logo-wrap { display: flex; align-items: center; gap: 8px; font-weight: 800; font-size: 1.1rem; }
        .logo-icon { color: var(--p1-cyan); display: flex; align-items: center; }
        .header-controls { display: flex; align-items: center; gap: 8px; }
        .icon-btn {
            background: transparent;
            border: 1px solid var(--border);
            color: var(--text-secondary);
            width: 32px; height: 32px; border-radius: var(--radius-sm);
            display: flex; align-items: center; justify-content: center;
            cursor: pointer; transition: color 140ms var(--ease), border-color 140ms var(--ease), background 140ms var(--ease);
        }
        .icon-btn:hover { color: var(--text); border-color: var(--border-highlight); background: var(--surface-hover); }
        .icon-btn:active { transform: scale(0.94); }
        .badge {
            font-size: 0.72rem; padding: 5px 9px; border-radius: 9999px;
            background: #1e293b; color: var(--text-tertiary);
            display: flex; align-items: center; gap: 6px; font-weight: 600;
        }
        .badge.connected { background: rgba(34, 197, 94, 0.12); color: #4ade80; }
        .dot { width: 6px; height: 6px; border-radius: 50%; background: currentColor; }
        .badge.connected .dot { box-shadow: 0 0 6px currentColor; }

        #main-container { width: 100%; max-width: 440px; display: flex; flex-direction: column; align-items: center; }
        .view { display: none; width: 100%; flex-direction: column; gap: 12px; }
        .view.active { display: flex; animation: viewFade 160ms var(--ease); }
        @keyframes viewFade { from { opacity: 0; transform: translateY(4px); } to { opacity: 1; transform: translateY(0); } }

        /* Profile bar */
        .profile-bar {
            display: flex; align-items: center; gap: 10px;
            background: var(--surface); padding: 8px 12px;
            border-radius: var(--radius-md); border: 1px solid var(--border);
        }
        .profile-label { font-size: 0.8rem; font-weight: 600; color: var(--text-secondary); }
        .profile-input {
            flex: 1; background: #0b111e; border: 1px solid var(--border);
            color: var(--p1-cyan); padding: 6px 10px; border-radius: var(--radius-sm);
            font-weight: 700; font-size: 0.9rem; outline: none; transition: border-color 140ms;
        }
        .profile-input:focus { border-color: var(--p1-cyan); }

        /* Mode Selector Tabs */
        .tab-bar {
            display: flex; width: 100%; background: var(--surface);
            border: 1px solid var(--border); border-radius: var(--radius-md); padding: 3px; gap: 4px;
        }
        .tab-btn {
            flex: 1; padding: 7px 0; border: none; background: transparent;
            color: var(--text-secondary); font-size: 0.82rem; font-weight: 700;
            border-radius: var(--radius-sm); cursor: pointer; transition: all 140ms var(--ease);
            text-align: center;
        }
        .tab-btn.active {
            background: var(--surface-active); color: var(--text); box-shadow: 0 1px 3px rgba(0,0,0,0.3);
        }

        /* Game List Items */
        .game-list { display: flex; flex-direction: column; gap: 8px; width: 100%; }
        .game-item {
            background: var(--surface); border: 1px solid var(--border);
            border-radius: var(--radius-md); padding: 12px 14px;
            display: flex; align-items: center; justify-content: space-between;
            cursor: pointer; transition: border-color 140ms, background 140ms, transform 100ms;
        }
        .game-item:hover { border-color: var(--border-highlight); background: var(--surface-hover); }
        .game-item:active { transform: scale(0.985); }
        .game-info { display: flex; align-items: center; gap: 12px; }
        .game-icon-box {
            width: 36px; height: 36px; border-radius: var(--radius-sm);
            background: #182234; border: 1px solid var(--border);
            display: flex; align-items: center; justify-content: center;
            color: var(--p1-cyan); flex-shrink: 0;
        }
        .game-icon-box.duel { color: var(--p2-pink); }
        .game-text { display: flex; flex-direction: column; gap: 2px; }
        .game-title { font-size: 0.92rem; font-weight: 700; }
        .game-desc { font-size: 0.74rem; color: var(--text-secondary); }
        .game-tag {
            font-size: 0.68rem; font-weight: 700; padding: 4px 8px;
            border-radius: var(--radius-sm); background: #1e293b; color: var(--text-secondary);
            flex-shrink: 0;
        }

        /* Section Headings */
        .section-title {
            font-size: 0.85rem; font-weight: 700; color: var(--text-secondary);
            margin: 4px 0 0 2px; display: flex; justify-content: space-between; align-items: center;
        }

        /* Room Directory Feed */
        .directory-container {
            background: var(--surface); border: 1px solid var(--border);
            border-radius: var(--radius-md); padding: 10px;
            display: flex; flex-direction: column; gap: 6px;
        }
        .room-item {
            display: flex; justify-content: space-between; align-items: center;
            background: #0d1524; border: 1px solid var(--border);
            padding: 9px 12px; border-radius: var(--radius-sm);
        }
        .room-meta { display: flex; flex-direction: column; gap: 2px; }
        .room-name { font-weight: 700; font-size: 0.88rem; }
        .room-sub { font-size: 0.72rem; color: var(--text-tertiary); }
        .btn-solid {
            background: var(--p1-cyan); color: #090d16; border: none;
            font-weight: 800; font-size: 0.78rem; padding: 7px 13px;
            border-radius: var(--radius-sm); cursor: pointer; transition: opacity 140ms;
        }
        .btn-solid:hover { opacity: 0.92; }
        .btn-solid:active { transform: scale(0.95); }
        .empty-directory {
            text-align: center; font-size: 0.78rem; color: var(--text-tertiary);
            padding: 14px 8px;
        }

        /* Waiting View */
        .waiting-box {
            background: var(--surface); border: 1px solid var(--border);
            border-radius: var(--radius-lg); padding: 32px 20px;
            display: flex; flex-direction: column; align-items: center; text-align: center; gap: 14px;
        }
        .radar-pulse {
            width: 58px; height: 58px; border-radius: 50%;
            background: rgba(56, 189, 248, 0.08); border: 1px solid var(--p1-cyan);
            position: relative; animation: pulseRadar 1.8s infinite ease-out;
            display: flex; align-items: center; justify-content: center;
        }
        @keyframes pulseRadar {
            0% { transform: scale(0.92); box-shadow: 0 0 0 0 rgba(56, 189, 248, 0.35); }
            70% { transform: scale(1.04); box-shadow: 0 0 0 16px rgba(56, 189, 248, 0); }
            100% { transform: scale(0.92); box-shadow: 0 0 0 0 rgba(56, 189, 248, 0); }
        }
        .btn-danger {
            background: rgba(239, 68, 68, 0.12); color: #f87171;
            border: 1px solid rgba(239, 68, 68, 0.25); font-weight: 700;
            padding: 8px 18px; border-radius: var(--radius-sm); cursor: pointer; font-size: 0.82rem;
            transition: background 140ms;
        }
        .btn-danger:hover { background: rgba(239, 68, 68, 0.2); }
        .btn-danger:active { transform: scale(0.95); }

        /* Game HUD */
        .game-hud {
            width: 100%; display: flex; justify-content: space-between; align-items: center;
            background: var(--surface); border: 1px solid var(--border);
            border-radius: var(--radius-md); padding: 8px 12px; font-size: 0.85rem; font-weight: 700;
        }
        .hud-player { display: flex; align-items: center; gap: 6px; }
        .p1-dot { width: 8px; height: 8px; border-radius: 50%; background: var(--p1-cyan); }
        .p2-dot { width: 8px; height: 8px; border-radius: 50%; background: var(--p2-pink); }
        .bot-dot { width: 8px; height: 8px; border-radius: 50%; background: var(--bot-amber); }

        #canvas-wrap {
            position: relative; width: 100%; max-width: 400px; aspect-ratio: 1/1;
            border-radius: var(--radius-lg); overflow: hidden;
            border: 1px solid var(--border); background: #070b14;
        }
        #game-canvas { width: 100%; height: 100%; display: block; }

        /* Virtual D-pad */
        .dpad-container {
            display: grid; grid-template-columns: repeat(3, 52px); grid-template-rows: repeat(3, 52px);
            gap: 6px; justify-content: center; margin-top: 4px;
        }
        .dpad-btn {
            background: var(--surface); color: var(--text-secondary); border: 1px solid var(--border);
            border-radius: var(--radius-md); display: flex; align-items: center; justify-content: center;
            cursor: pointer; touch-action: manipulation; transition: background 100ms, color 100ms;
        }
        .dpad-btn:active { background: var(--p1-cyan); color: #090d16; }
        .dpad-up { grid-column: 2; grid-row: 1; }
        .dpad-left { grid-column: 1; grid-row: 2; }
        .dpad-down { grid-column: 2; grid-row: 2; }
        .dpad-right { grid-column: 3; grid-row: 2; }

        @media (hover: hover) and (pointer: fine) {
            #touch-controls { display: none !important; }
        }

        /* Modal Dialog */
        .modal-overlay {
            display: none; position: absolute; inset: 0;
            background: rgba(9, 13, 22, 0.9); backdrop-filter: blur(4px);
            flex-direction: column; align-items: center; justify-content: center;
            padding: 20px; text-align: center; gap: 12px; z-index: 10;
        }
        .modal-overlay.active { display: flex; }
        .modal-icon-wrap { margin-bottom: 2px; }
        .modal-title { font-size: 1.6rem; font-weight: 800; }
        .modal-sub { font-size: 0.9rem; color: var(--text-secondary); }
        .modal-actions { display: flex; gap: 8px; margin-top: 6px; }
        .btn-action {
            background: var(--p1-cyan); color: #090d16; border: none;
            font-weight: 800; font-size: 0.85rem; padding: 9px 16px;
            border-radius: var(--radius-sm); cursor: pointer;
        }
        .btn-secondary {
            background: #1e293b; color: var(--text); border: 1px solid var(--border);
            font-weight: 700; font-size: 0.85rem; padding: 9px 16px;
            border-radius: var(--radius-sm); cursor: pointer;
        }
    </style>
</head>
)rawliteral";
    static constexpr const char* INDEX_HTML_PART2 = R"rawliteral(
<body>
    <header>
        <div class="logo-wrap">
            <span class="logo-icon">
                <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2" stroke-linecap="round" stroke-linejoin="round">
                    <rect x="2" y="6" width="20" height="12" rx="3"></rect>
                    <path d="M6 12h4m-2-2v4m10-2h.01m-3-2h.01"></path>
                </svg>
            </span>
            <span>Gamebox</span>
        </div>
        <div class="header-controls">
            <button id="btn-sound" class="icon-btn" title="Toggle Sound">
                <svg id="svg-sound-on" width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                    <polygon points="11 5 6 9 2 9 2 15 6 15 11 19 11 5"></polygon>
                    <path d="M15.54 8.46a5 5 0 0 1 0 7.07"></path>
                    <path d="M19.07 4.93a10 10 0 0 1 0 14.14"></path>
                </svg>
                <svg id="svg-sound-off" style="display:none;" width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                    <polygon points="11 5 6 9 2 9 2 15 6 15 11 19 11 5"></polygon>
                    <line x1="23" y1="9" x2="17" y2="15"></line>
                    <line x1="17" y1="9" x2="23" y2="15"></line>
                    <line x1="1" y1="1" x2="23" y2="23"></line>
                </svg>
            </button>
            <div id="status-badge" class="badge">
                <span class="dot"></span>
                <span id="status-text">Connecting</span>
            </div>
        </div>
    </header>

    <div id="main-container">
        <!-- 1. LOBBY VIEW -->
        <div id="view-lobby" class="view active">
            <div class="profile-bar">
                <span class="profile-label">Player:</span>
                <input id="player-nickname" class="profile-input" maxlength="12" spellcheck="false" placeholder="Your Name">
            </div>

            <div class="tab-bar">
                <button class="tab-btn active" id="tab-btn-solo">Solo Play</button>
                <button class="tab-btn" id="tab-btn-mp">Multiplayer</button>
            </div>

            <!-- Solo Games List -->
            <div id="list-solo" class="game-list">
                <div class="game-item" id="card-solo-snake">
                    <div class="game-info">
                        <div class="game-icon-box">
                            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 2a4 4 0 0 0-4 4v3a3 3 0 0 1-6 0V8"/><path d="M12 2a4 4 0 0 1 4 4v8a4 4 0 0 1-8 0v-2a2 2 0 0 1 4 0"/><circle cx="10" cy="5" r="1" fill="currentColor"/></svg>
                        </div>
                        <div class="game-text">
                            <span class="game-title">Snake Survival</span>
                            <span class="game-desc">Grow tail, collect apples, beat records</span>
                        </div>
                    </div>
                    <span class="game-tag">1 Player</span>
                </div>

                <div class="game-item" id="card-solo-ttt">
                    <div class="game-info">
                        <div class="game-icon-box">
                            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="8" y1="3" x2="8" y2="21"/><line x1="16" y1="3" x2="16" y2="21"/><line x1="3" y1="8" x2="21" y2="8"/><line x1="3" y1="16" x2="21" y2="16"/></svg>
                        </div>
                        <div class="game-text">
                            <span class="game-title">Tic-Tac-Toe</span>
                            <span class="game-desc">Duel against the unbeatable Minimax Bot</span>
                        </div>
                    </div>
                    <span class="game-tag">vs Bot</span>
                </div>

                <div class="game-item" id="card-solo-c4">
                    <div class="game-info">
                        <div class="game-icon-box">
                            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="3" width="18" height="18" rx="3"/><circle cx="8" cy="8" r="2"/><circle cx="16" cy="8" r="2"/><circle cx="8" cy="16" r="2"/><circle cx="16" cy="16" r="2"/></svg>
                        </div>
                        <div class="game-text">
                            <span class="game-title">Connect Four</span>
                            <span class="game-desc">Drop discs to form four-in-a-row</span>
                        </div>
                    </div>
                    <span class="game-tag">vs Bot</span>
                </div>
            </div>

            <!-- Multiplayer Games List -->
            <div id="list-mp" class="game-list" style="display:none;">
                <div class="game-item" id="card-mp-snake">
                    <div class="game-info">
                        <div class="game-icon-box duel">
                            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 2a4 4 0 0 0-4 4v3a3 3 0 0 1-6 0V8"/><path d="M12 2a4 4 0 0 1 4 4v8a4 4 0 0 1-8 0v-2a2 2 0 0 1 4 0"/><circle cx="10" cy="5" r="1" fill="currentColor"/></svg>
                        </div>
                        <div class="game-text">
                            <span class="game-title">Snake Arena</span>
                            <span class="game-desc">Real-time head-to-head collision duel</span>
                        </div>
                    </div>
                    <span class="game-tag">2 Players</span>
                </div>

                <div class="game-item" id="card-mp-ttt">
                    <div class="game-info">
                        <div class="game-icon-box duel">
                            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="8" y1="3" x2="8" y2="21"/><line x1="16" y1="3" x2="16" y2="21"/><line x1="3" y1="8" x2="21" y2="8"/><line x1="3" y1="16" x2="21" y2="16"/></svg>
                        </div>
                        <div class="game-text">
                            <span class="game-title">2P Tic-Tac-Toe</span>
                            <span class="game-desc">Turn-based 1v1 tactical match</span>
                        </div>
                    </div>
                    <span class="game-tag">2 Players</span>
                </div>

                <div class="game-item" id="card-mp-c4">
                    <div class="game-info">
                        <div class="game-icon-box duel">
                            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="3" width="18" height="18" rx="3"/><circle cx="8" cy="8" r="2"/><circle cx="16" cy="8" r="2"/><circle cx="8" cy="16" r="2"/><circle cx="16" cy="16" r="2"/></svg>
                        </div>
                        <div class="game-text">
                            <span class="game-title">2P Connect Four</span>
                            <span class="game-desc">Vertical drop grid duel</span>
                        </div>
                    </div>
                    <span class="game-tag">2 Players</span>
                </div>

                <div class="game-item" id="card-mp-pong">
                    <div class="game-info">
                        <div class="game-icon-box duel">
                            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="4" y1="7" x2="4" y2="17"/><line x1="20" y1="7" x2="20" y2="17"/><line x1="12" y1="2" x2="12" y2="22" stroke-dasharray="3 3"/><rect x="9" y="11" width="2" height="2" fill="currentColor"/></svg>
                        </div>
                        <div class="game-text">
                            <span class="game-title">Pong Duel</span>
                            <span class="game-desc">Fast-paced dual paddle bounce battle</span>
                        </div>
                    </div>
                    <span class="game-tag">2 Players</span>
                </div>

                <div class="game-item" id="card-mp-tron">
                    <div class="game-info">
                        <div class="game-icon-box duel">
                            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M3 19h8a2 2 0 0 0 2-2V7a2 2 0 0 1 2-2h6"/><circle cx="3" cy="19" r="2" fill="currentColor"/><circle cx="21" cy="5" r="2" fill="currentColor"/></svg>
                        </div>
                        <div class="game-text">
                            <span class="game-title">Tron Light Cycles</span>
                            <span class="game-desc">90-degree light trail wall trapper</span>
                        </div>
                    </div>
                    <span class="game-tag">2 Players</span>
                </div>

                <div class="game-item" id="card-mp-battleship">
                    <div class="game-info">
                        <div class="game-icon-box duel">
                            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M2 14l3 5h14l3-5H2z"/><path d="M7 14V9h3v5m4 0V6h3v8"/><line x1="2" y1="19" x2="22" y2="19"/></svg>
                        </div>
                        <div class="game-text">
                            <span class="game-title">Battleship Fleet</span>
                            <span class="game-desc">Secret naval deployment & tactical firing</span>
                        </div>
                    </div>
                    <span class="game-tag">2 Players</span>
                </div>
            </div>

            <div class="section-title">
                <span>Active Matches</span>
                <span id="latency-val" style="font-size:0.72rem; color:var(--text-tertiary);">Ping: -- ms</span>
            </div>

            <div class="directory-container" id="room-directory-list">
                <div class="empty-directory">No open matches. Select a multiplayer game to host!</div>
            </div>
        </div>

        <!-- 2. ROOM WAITING VIEW -->
        <div id="view-waiting" class="view">
            <div class="waiting-box">
                <div class="radar-pulse">
                    <svg width="28" height="28" viewBox="0 0 24 24" fill="none" stroke="var(--p1-cyan)" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                        <circle cx="12" cy="12" r="2"></circle>
                        <path d="M16.24 7.76a6 6 0 0 1 0 8.49m-8.48-.01a6 6 0 0 1 0-8.49m11.31-2.82a10 10 0 0 1 0 14.14m-14.14 0a10 10 0 0 1 0-14.14"></path>
                    </svg>
                </div>
                <h2 id="waiting-game-title" style="font-size:1.15rem; font-weight:800;">Waiting for Challenger</h2>
                <p id="waiting-host-msg" style="color:var(--text-secondary); font-size:0.82rem;">Visible in the match feed. Anyone can join directly.</p>
                <button id="btn-cancel-room" class="btn-danger">Cancel Match</button>
            </div>
        </div>

        <!-- 3. GAMEPLAY VIEW -->
        <div id="view-game" class="view">
            <div class="game-hud">
                <div class="hud-player">
                    <span class="p1-dot"></span>
                    <span id="hud-p1-name">P1</span>: <span id="hud-p1-score">0</span>
                </div>
                <div id="hud-status" style="color:var(--text-secondary); font-size:0.8rem; font-weight:600;">Match Live</div>
                <div class="hud-player">
                    <span id="hud-p2-marker" class="p2-dot"></span>
                    <span id="hud-p2-name">P2</span>: <span id="hud-p2-score">0</span>
                </div>
            </div>

            <!-- Battleship Deployment Controls -->
            <div id="battleship-controls" style="display:none; width:100%; justify-content:center; gap:8px;">
                <button id="btn-deploy-fleet" class="btn-solid" style="width:100%; padding:10px;">Deploy Fleet (Auto-Arrange)</button>
            </div>

            <div id="canvas-wrap">
                <canvas id="game-canvas" width="400" height="400"></canvas>
                <!-- Game Over Modal Overlay -->
                <div id="modal-gameover" class="modal-overlay">
                    <div id="modal-icon-container" class="modal-icon-wrap"></div>
                    <div id="modal-title" class="modal-title">VICTORY</div>
                    <div id="modal-sub" class="modal-sub">Match finished.</div>
                    <div class="modal-actions">
                        <button id="btn-modal-replay" class="btn-action">Play Again</button>
                        <button id="btn-modal-lobby" class="btn-secondary">Lobby</button>
                    </div>
                </div>
            </div>

            <div class="dpad-container" id="touch-controls">
                <button class="dpad-btn dpad-up" data-dir="UP">
                    <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round"><polyline points="18 15 12 9 6 15"/></svg>
                </button>
                <button class="dpad-btn dpad-left" data-dir="LEFT">
                    <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round"><polyline points="15 18 9 12 15 6"/></svg>
                </button>
                <button class="dpad-btn dpad-down" data-dir="DOWN">
                    <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round"><polyline points="6 9 12 15 18 9"/></svg>
                </button>
                <button class="dpad-btn dpad-right" data-dir="RIGHT">
                    <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round"><polyline points="9 18 15 12 9 6"/></svg>
                </button>
            </div>

            <div style="display:flex; justify-content:center; margin-top:2px;">
                <button id="btn-forfeit" class="btn-secondary" style="font-size:0.78rem; padding:6px 14px;">Leave Match</button>
            </div>
        </div>
    </div>
)rawliteral";
    static constexpr const char* INDEX_HTML_PART3 = R"rawliteral(
    <script>
        // --- Web Audio Procedural Chiptune Synthesizer ---
        class SoundEngine {
            constructor() {
                this.ctx = null;
                this.muted = localStorage.getItem('gamebox_muted') === 'true';
            }
            init() {
                if (!this.ctx) {
                    const AudioCtx = window.AudioContext || window.webkitAudioContext;
                    if (AudioCtx) this.ctx = new AudioCtx();
                }
                if (this.ctx && this.ctx.state === 'suspended') {
                    this.ctx.resume();
                }
            }
            toggleMute() {
                this.muted = !this.muted;
                localStorage.setItem('gamebox_muted', this.muted);
                return this.muted;
            }
            playTone(freq, type, duration, vol = 0.1) {
                if (this.muted) return;
                this.init();
                if (!this.ctx) return;
                try {
                    const osc = this.ctx.createOscillator();
                    const gain = this.ctx.createGain();
                    osc.type = type;
                    osc.frequency.setValueAtTime(freq, this.ctx.currentTime);
                    gain.gain.setValueAtTime(vol, this.ctx.currentTime);
                    gain.gain.exponentialRampToValueAtTime(0.0001, this.ctx.currentTime + duration);
                    osc.connect(gain);
                    gain.connect(this.ctx.destination);
                    osc.start();
                    osc.stop(this.ctx.currentTime + duration);
                } catch(e) {}
            }
            click() { this.playTone(600, 'square', 0.04, 0.05); }
            move() { this.playTone(220, 'triangle', 0.05, 0.06); }
            bounce() { this.playTone(440, 'sine', 0.05, 0.08); }
            eat() {
                this.playTone(523.25, 'sine', 0.08, 0.12);
                setTimeout(() => this.playTone(659.25, 'sine', 0.08, 0.12), 60);
                setTimeout(() => this.playTone(783.99, 'sine', 0.12, 0.12), 120);
            }
            defeat() {
                this.playTone(293.66, 'sawtooth', 0.15, 0.15);
                setTimeout(() => this.playTone(220.00, 'sawtooth', 0.2, 0.15), 100);
                setTimeout(() => this.playTone(146.83, 'sawtooth', 0.35, 0.18), 220);
            }
            victory() {
                const notes = [523.25, 659.25, 783.99, 1046.50];
                notes.forEach((f, i) => {
                    setTimeout(() => this.playTone(f, 'square', 0.15, 0.1), i * 90);
                });
            }
        }

        const sound = new SoundEngine();
        function haptic(ms = 15) {
            if (navigator.vibrate) try { navigator.vibrate(ms); } catch(e) {}
        }

        // --- DOM Elements & Global State ---
        const viewLobby = document.getElementById('view-lobby');
        const viewWaiting = document.getElementById('view-waiting');
        const viewGame = document.getElementById('view-game');
        const tabBtnSolo = document.getElementById('tab-btn-solo');
        const tabBtnMp = document.getElementById('tab-btn-mp');
        const listSolo = document.getElementById('list-solo');
        const listMp = document.getElementById('list-mp');
        const statusBadge = document.getElementById('status-badge');
        const statusText = document.getElementById('status-text');
        const latencyVal = document.getElementById('latency-val');
        const btnSound = document.getElementById('btn-sound');
        const svgSoundOn = document.getElementById('svg-sound-on');
        const svgSoundOff = document.getElementById('svg-sound-off');
        const nicknameInput = document.getElementById('player-nickname');
        const roomDirectoryList = document.getElementById('room-directory-list');
        const waitingGameTitle = document.getElementById('waiting-game-title');
        const btnCancelRoom = document.getElementById('btn-cancel-room');
        const btnForfeit = document.getElementById('btn-forfeit');
        const battleshipControls = document.getElementById('battleship-controls');
        const btnDeployFleet = document.getElementById('btn-deploy-fleet');

        const canvas = document.getElementById('game-canvas');
        const ctx = canvas.getContext('2d');
        const modalGameOver = document.getElementById('modal-gameover');
        const modalTitle = document.getElementById('modal-title');
        const modalSub = document.getElementById('modal-sub');
        const modalIconContainer = document.getElementById('modal-icon-container');
        const btnModalReplay = document.getElementById('btn-modal-replay');
        const btnModalLobby = document.getElementById('btn-modal-lobby');
        const touchControls = document.getElementById('touch-controls');

        const hudP1Name = document.getElementById('hud-p1-name');
        const hudP1Score = document.getElementById('hud-p1-score');
        const hudP2Name = document.getElementById('hud-p2-name');
        const hudP2Score = document.getElementById('hud-p2-score');
        const hudP2Marker = document.getElementById('hud-p2-marker');
        const hudStatus = document.getElementById('hud-status');

        let ws = null;
        let pingTimer = null;
        let gameLoop = null;
        let activeGameType = null;
        let isHost = false;
        let myPlayerNum = 1;
        let opponentName = "Opponent";
        let activeRoomCode = null;
        let lastState = null;
        let particles = [];
        let shakeFrames = 0;

        // Restore Nickname
        const defaultNames = ["Ace", "Pilot", "Striker", "Cyber", "Viper", "Echo", "Nova", "Blaze"];
        const savedNick = localStorage.getItem('gamebox_nick') || defaultNames[Math.floor(Math.random() * defaultNames.length)];
        nicknameInput.value = savedNick;
        nicknameInput.addEventListener('change', () => {
            const v = nicknameInput.value.trim() || "Player";
            nicknameInput.value = v;
            localStorage.setItem('gamebox_nick', v);
        });

        // Sound Toggle
        function updateSoundIcons() {
            if (sound.muted) {
                svgSoundOn.style.display = 'none';
                svgSoundOff.style.display = 'block';
            } else {
                svgSoundOn.style.display = 'block';
                svgSoundOff.style.display = 'none';
            }
        }
        updateSoundIcons();
        btnSound.addEventListener('click', () => {
            const isMuted = sound.toggleMute();
            updateSoundIcons();
            if (!isMuted) sound.click();
        });

        // Tabs
        tabBtnSolo.addEventListener('click', () => {
            sound.click();
            tabBtnSolo.classList.add('active');
            tabBtnMp.classList.remove('active');
            listSolo.style.display = 'flex';
            listMp.style.display = 'none';
        });
        tabBtnMp.addEventListener('click', () => {
            sound.click();
            tabBtnMp.classList.add('active');
            tabBtnSolo.classList.remove('active');
            listMp.style.display = 'flex';
            listSolo.style.display = 'none';
        });

        function switchView(name) {
            [viewLobby, viewWaiting, viewGame].forEach(v => v.classList.remove('active'));
            modalGameOver.classList.remove('active');
            battleshipControls.style.display = 'none';
            if (name === 'lobby') viewLobby.classList.add('active');
            if (name === 'waiting') viewWaiting.classList.add('active');
            if (name === 'game') viewGame.classList.add('active');
        }

        // --- WebSocket Networking ---
        function connect() {
            const proto = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
            const host = window.location.host || 'localhost:8080';
            ws = new WebSocket(`${proto}//${host}/ws`);

            ws.onopen = () => {
                statusBadge.classList.add('connected');
                statusText.textContent = 'Connected';
                startPing();
                ws.send(JSON.stringify({ cmd: 'get_rooms' }));
            };

            ws.onmessage = (event) => {
                try {
                    const data = JSON.parse(event.data);
                    handleServerMessage(data);
                } catch (e) {
                    console.error("Message parse error", e);
                }
            };

            ws.onclose = () => {
                statusBadge.classList.remove('connected');
                statusText.textContent = 'Offline';
                clearInterval(pingTimer);
                setTimeout(connect, 1500);
            };
        }

        function startPing() {
            clearInterval(pingTimer);
            pingTimer = setInterval(() => {
                if (ws && ws.readyState === WebSocket.OPEN) {
                    ws.send(JSON.stringify({ type: 'ping', ts: Date.now() }));
                }
            }, 1000);
        }
)rawliteral";
    static constexpr const char* INDEX_HTML_PART4 = R"rawliteral(
        function getGameTitle(gameKey) {
            switch(gameKey) {
                case 'snake_duel': return 'Snake Arena';
                case 'ttt_pvp': return '2P Tic-Tac-Toe';
                case 'connect4_pvp': return 'Connect Four';
                case 'pong_duel': return 'Pong Duel';
                case 'tron_duel': return 'Tron Light Cycles';
                case 'battleship_pvp': return 'Battleship Fleet';
                default: return 'Custom Game';
            }
        }

        function handleServerMessage(data) {
            if (data.type === 'pong') {
                const rtt = Date.now() - data.ts;
                latencyVal.textContent = `Ping: ${rtt} ms`;
            } else if (data.type === 'room_directory') {
                renderRoomDirectory(data.rooms || []);
            } else if (data.type === 'room_created') {
                activeRoomCode = data.code;
                isHost = true;
                myPlayerNum = 1;
                switchView('waiting');
                waitingGameTitle.textContent = `Hosting ${getGameTitle(activeGameType)}`;
            } else if (data.type === 'room_joined') {
                activeRoomCode = data.code;
                isHost = false;
                myPlayerNum = 2;
                opponentName = data.host || "Host";
                hudP2Name.textContent = nicknameInput.value;
                hudP1Name.textContent = opponentName;
                setupGameView(activeGameType);
                switchView('game');
                sound.click();
            } else if (data.type === 'room_ready') {
                setupGameView(activeGameType);
                switchView('game');
                sound.click();
            } else if (data.type === 'opponent_left') {
                sound.defeat();
                stopGameLoop();
                switchView('lobby');
                if (ws && ws.readyState === WebSocket.OPEN) ws.send(JSON.stringify({ cmd: 'get_rooms' }));
            } else if (data.type === 'state') {
                lastState = data;
                renderGameState(data);
            }
        }

        function renderRoomDirectory(rooms) {
            roomDirectoryList.innerHTML = '';
            if (rooms.length === 0) {
                roomDirectoryList.innerHTML = '<div class="empty-directory">No open matches. Select a multiplayer game to host!</div>';
                return;
            }
            rooms.forEach(r => {
                const item = document.createElement('div');
                item.className = 'room-item';
                item.innerHTML = `
                    <div class="room-meta">
                        <span class="room-name">${escapeHtml(r.host)} · ${getGameTitle(r.game)}</span>
                        <span class="room-sub">Waiting for challenger</span>
                    </div>
                    <button class="btn-solid" data-code="${r.code}">Join Match</button>
                `;
                item.querySelector('.btn-solid').addEventListener('click', () => {
                    sound.click();
                    haptic(20);
                    if (ws && ws.readyState === WebSocket.OPEN) {
                        activeGameType = r.game;
                        ws.send(JSON.stringify({ cmd: 'join', code: r.code }));
                    }
                });
                roomDirectoryList.appendChild(item);
            });
        }

        function escapeHtml(str) {
            return String(str).replace(/[&<>"']/g, s => ({ '&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":"&#39;" })[s]);
        }

        function setupGameView(gType) {
            touchControls.style.display = 'none';
            battleshipControls.style.display = 'none';

            if (gType === 'snake_solo' || gType === 'snake_duel' || gType === 'tron_duel') {
                touchControls.style.display = 'grid';
            } else if (gType === 'pong_duel') {
                touchControls.style.display = 'grid';
            } else if (gType === 'battleship_pvp') {
                battleshipControls.style.display = 'flex';
            }
        }

        // --- Solo Game Triggers ---
        document.getElementById('card-solo-snake').addEventListener('click', () => {
            sound.click();
            activeGameType = 'snake_solo';
            myPlayerNum = 1;
            hudP1Name.textContent = nicknameInput.value;
            hudP2Name.textContent = "High";
            hudP2Marker.style.display = 'none';
            setupGameView('snake_solo');
            switchView('game');
            ws.send(JSON.stringify({ cmd: 'start', game: 'snake_solo' }));
            startGameLoop();
        });

        document.getElementById('card-solo-ttt').addEventListener('click', () => {
            sound.click();
            activeGameType = 'ttt_solo';
            myPlayerNum = 1;
            hudP1Name.textContent = nicknameInput.value + " (X)";
            hudP2Name.textContent = "Minimax (O)";
            hudP2Marker.className = 'bot-dot';
            hudP2Marker.style.display = 'inline-block';
            setupGameView('ttt_solo');
            switchView('game');
            ws.send(JSON.stringify({ cmd: 'start', game: 'ttt_solo' }));
        });

        document.getElementById('card-solo-c4').addEventListener('click', () => {
            sound.click();
            activeGameType = 'connect4_solo';
            myPlayerNum = 1;
            hudP1Name.textContent = nicknameInput.value;
            hudP2Name.textContent = "Bot";
            hudP2Marker.className = 'bot-dot';
            hudP2Marker.style.display = 'inline-block';
            setupGameView('connect4_solo');
            switchView('game');
            ws.send(JSON.stringify({ cmd: 'start', game: 'connect4_solo' }));
        });

        // --- Multiplayer Game Triggers ---
        function hostMultiplayer(gameType) {
            sound.click();
            activeGameType = gameType;
            ws.send(JSON.stringify({ cmd: 'create', game: gameType, host: nicknameInput.value }));
        }

        document.getElementById('card-mp-snake').addEventListener('click', () => hostMultiplayer('snake_duel'));
        document.getElementById('card-mp-ttt').addEventListener('click', () => hostMultiplayer('ttt_pvp'));
        document.getElementById('card-mp-c4').addEventListener('click', () => hostMultiplayer('connect4_pvp'));
        document.getElementById('card-mp-pong').addEventListener('click', () => hostMultiplayer('pong_duel'));
        document.getElementById('card-mp-tron').addEventListener('click', () => hostMultiplayer('tron_duel'));
        document.getElementById('card-mp-battleship').addEventListener('click', () => hostMultiplayer('battleship_pvp'));

        btnCancelRoom.addEventListener('click', () => {
            sound.click();
            if (ws && ws.readyState === WebSocket.OPEN) ws.send(JSON.stringify({ cmd: 'leave' }));
            switchView('lobby');
        });

        btnForfeit.addEventListener('click', () => {
            sound.click();
            stopGameLoop();
            if (ws && ws.readyState === WebSocket.OPEN) ws.send(JSON.stringify({ cmd: 'leave' }));
            switchView('lobby');
        });

        btnModalLobby.addEventListener('click', () => {
            sound.click();
            switchView('lobby');
            if (ws && ws.readyState === WebSocket.OPEN) ws.send(JSON.stringify({ cmd: 'get_rooms' }));
        });

        btnModalReplay.addEventListener('click', () => {
            sound.click();
            modalGameOver.classList.remove('active');
            if (activeGameType === 'snake_solo') {
                ws.send(JSON.stringify({ cmd: 'start', game: 'snake_solo' }));
                startGameLoop();
            } else if (activeGameType === 'ttt_solo') {
                ws.send(JSON.stringify({ cmd: 'start', game: 'ttt_solo' }));
            } else if (activeGameType === 'connect4_solo') {
                ws.send(JSON.stringify({ cmd: 'start', game: 'connect4_solo' }));
            } else {
                switchView('lobby');
            }
        });

        function startGameLoop() {
            clearInterval(gameLoop);
            gameLoop = setInterval(() => {
                if (ws && ws.readyState === WebSocket.OPEN && activeGameType === 'snake_solo') {
                    ws.send(JSON.stringify({ cmd: 'tick' }));
                }
            }, 66);
        }
        function stopGameLoop() { clearInterval(gameLoop); }

        function sendDir(dir) {
            haptic(15);
            sound.move();
            if (ws && ws.readyState === WebSocket.OPEN) {
                ws.send(JSON.stringify({ cmd: 'dir', val: dir }));
            }
        }
)rawliteral";
    static constexpr const char* INDEX_HTML_PART5 = R"rawliteral(
        // D-Pad and Keyboard Listeners
        document.querySelectorAll('.dpad-btn').forEach(btn => {
            const dir = btn.getAttribute('data-dir');
            btn.addEventListener('pointerdown', (e) => {
                e.preventDefault();
                sendDir(dir);
            });
        });

        window.addEventListener('keydown', (e) => {
            let dir = null;
            if (e.key === 'ArrowUp' || e.key === 'w' || e.key === 'W') dir = 'UP';
            else if (e.key === 'ArrowDown' || e.key === 's' || e.key === 'S') dir = 'DOWN';
            else if (e.key === 'ArrowLeft' || e.key === 'a' || e.key === 'A') dir = 'LEFT';
            else if (e.key === 'ArrowRight' || e.key === 'd' || e.key === 'D') dir = 'RIGHT';
            if (dir) {
                e.preventDefault();
                sendDir(dir);
            }
        });

        // Battleship Auto-Deploy button
        btnDeployFleet.addEventListener('click', () => {
            sound.click();
            haptic(20);
            if (ws && ws.readyState === WebSocket.OPEN) {
                ws.send(JSON.stringify({ cmd: 'auto_place' }));
            }
        });

        // Canvas Click handler for grid-based games
        canvas.addEventListener('click', (e) => {
            if (!lastState || lastState.finished) return;
            const rect = canvas.getBoundingClientRect();
            const scaleX = canvas.width / rect.width;
            const scaleY = canvas.height / rect.height;
            const x = (e.clientX - rect.left) * scaleX;
            const y = (e.clientY - rect.top) * scaleY;

            // 1. Tic-Tac-Toe
            if (lastState.game === 'tictactoe') {
                const cellSize = canvas.width / 3;
                const col = Math.floor(x / cellSize);
                const row = Math.floor(y / cellSize);
                if (col >= 0 && col < 3 && row >= 0 && row < 3) {
                    const cell = row * 3 + col;
                    sound.click();
                    haptic(20);
                    ws.send(JSON.stringify({ cmd: 'place', cell: cell }));
                }
            }
            // 2. Connect Four
            else if (lastState.game === 'connect4') {
                const colWidth = canvas.width / 7;
                const col = Math.floor(x / colWidth);
                if (col >= 0 && col < 7) {
                    sound.click();
                    haptic(20);
                    ws.send(JSON.stringify({ cmd: 'drop', col: col }));
                }
            }
            // 3. Battleship (firing on enemy grid)
            else if (lastState.game === 'battleship' && lastState.phase === 'battle') {
                if (lastState.turn === myPlayerNum) {
                    const cellSize = canvas.width / 10;
                    const col = Math.floor(x / cellSize);
                    const row = Math.floor(y / cellSize);
                    if (col >= 0 && col < 10 && row >= 0 && row < 10) {
                        sound.click();
                        haptic(25);
                        ws.send(JSON.stringify({ cmd: 'fire', row: row, col: col }));
                    }
                }
            }
        });

        // --- Particles & Canvas Juice ---
        function spawnParticles(x, y, color, count = 12) {
            for (let i = 0; i < count; i++) {
                const angle = Math.random() * Math.PI * 2;
                const speed = 1 + Math.random() * 4;
                particles.push({
                    x, y,
                    vx: Math.cos(angle) * speed,
                    vy: Math.sin(angle) * speed,
                    life: 1.0,
                    decay: 0.03 + Math.random() * 0.04,
                    color
                });
            }
        }

        let prevScore = 0;
        let prevFinished = false;

        function renderGameState(state) {
            ctx.save();
            if (shakeFrames > 0) {
                ctx.translate((Math.random() - 0.5) * 6, (Math.random() - 0.5) * 6);
                shakeFrames--;
            }

            if (state.game === 'snake') {
                renderSnakeGame(state);
            } else if (state.game === 'tictactoe') {
                renderTicTacToeGame(state);
            } else if (state.game === 'connect4') {
                renderConnectFourGame(state);
            } else if (state.game === 'pong') {
                renderPongGame(state);
            } else if (state.game === 'tron') {
                renderTronGame(state);
            } else if (state.game === 'battleship') {
                renderBattleshipGame(state);
            }
            ctx.restore();

            // Audio cues
            if (state.score > prevScore) {
                sound.eat();
                haptic(25);
            }
            prevScore = state.score || 0;

            if (state.finished && !prevFinished) {
                shakeFrames = 6;
                haptic(40);
                if (state.winner === myPlayerNum || (state.game === 'snake' && !state.duel)) {
                    sound.victory();
                } else if (state.winner === -1) {
                    sound.bounce();
                } else {
                    sound.defeat();
                }
            }
            prevFinished = state.finished;
        }
)rawliteral";
    static constexpr const char* INDEX_HTML_PART6 = R"rawliteral(
        // --- 1. Snake Renderer ---
        function renderSnakeGame(state) {
            const grid = 20;
            const sz = canvas.width / grid;
            ctx.fillStyle = '#070b14';
            ctx.fillRect(0, 0, canvas.width, canvas.height);

            // Subtle grid lines
            ctx.strokeStyle = '#111827';
            ctx.lineWidth = 1;
            for (let i = 0; i <= grid; i++) {
                ctx.beginPath(); ctx.moveTo(i * sz, 0); ctx.lineTo(i * sz, canvas.height); ctx.stroke();
                ctx.beginPath(); ctx.moveTo(0, i * sz); ctx.lineTo(canvas.width, i * sz); ctx.stroke();
            }

            // Food with pulse
            if (state.food) {
                const fx = state.food[0] * sz + sz / 2;
                const fy = state.food[1] * sz + sz / 2;
                const pulse = 1 + Math.sin(Date.now() / 150) * 0.12;
                ctx.fillStyle = '#ef4444';
                ctx.beginPath();
                ctx.arc(fx, fy, (sz / 2 - 2) * pulse, 0, Math.PI * 2);
                ctx.fill();
            }

            if (state.body && state.body.length > 0) renderSnakeBody(state.body, '#38bdf8', '#0284c7', sz);
            if (state.body2 && state.body2.length > 0) renderSnakeBody(state.body2, '#ec4899', '#be185d', sz);

            // Particles
            particles.forEach((p, idx) => {
                p.x += p.vx; p.y += p.vy; p.life -= p.decay;
                if (p.life <= 0) particles.splice(idx, 1);
                else {
                    ctx.fillStyle = p.color; ctx.globalAlpha = p.life;
                    ctx.beginPath(); ctx.arc(p.x, p.y, 2, 0, Math.PI * 2); ctx.fill();
                    ctx.globalAlpha = 1.0;
                }
            });

            hudP1Score.textContent = state.score || 0;
            if (state.duel) {
                hudP2Score.textContent = state.score2 || 0;
                hudStatus.textContent = "Arena Duel";
            } else {
                hudStatus.textContent = "Solo Survival";
            }
            if (state.finished) showModal(state);
        }

        function renderSnakeBody(body, headColor, bodyColor, sz) {
            body.forEach((seg, idx) => {
                const rx = seg[0] * sz + 1;
                const ry = seg[1] * sz + 1;
                const rsz = sz - 2;
                ctx.fillStyle = idx === 0 ? headColor : bodyColor;
                ctx.beginPath();
                ctx.roundRect ? ctx.roundRect(rx, ry, rsz, rsz, 4) : ctx.rect(rx, ry, rsz, rsz);
                ctx.fill();
            });
        }

        // --- 2. Tic-Tac-Toe Renderer ---
        function renderTicTacToeGame(state) {
            const sz = canvas.width / 3;
            ctx.fillStyle = '#070b14';
            ctx.fillRect(0, 0, canvas.width, canvas.height);

            ctx.strokeStyle = '#1e293b';
            ctx.lineWidth = 3;
            for (let i = 1; i < 3; i++) {
                ctx.beginPath(); ctx.moveTo(i * sz, 24); ctx.lineTo(i * sz, canvas.height - 24); ctx.stroke();
                ctx.beginPath(); ctx.moveTo(24, i * sz); ctx.lineTo(canvas.width - 24, i * sz); ctx.stroke();
            }

            if (state.board) {
                state.board.forEach((val, idx) => {
                    const row = Math.floor(idx / 3);
                    const col = idx % 3;
                    const cx = col * sz + sz / 2;
                    const cy = row * sz + sz / 2;
                    if (val === 1) {
                        ctx.strokeStyle = '#38bdf8';
                        ctx.lineWidth = 6;
                        ctx.beginPath();
                        ctx.moveTo(cx - 28, cy - 28); ctx.lineTo(cx + 28, cy + 28);
                        ctx.moveTo(cx + 28, cy - 28); ctx.lineTo(cx - 28, cy + 28);
                        ctx.stroke();
                    } else if (val === 2) {
                        ctx.strokeStyle = '#ec4899';
                        ctx.lineWidth = 6;
                        ctx.beginPath();
                        ctx.arc(cx, cy, 30, 0, Math.PI * 2);
                        ctx.stroke();
                    }
                });
            }

            if (!state.finished) {
                hudStatus.textContent = state.turn === myPlayerNum ? "Your Turn" : "Opponent Turn";
            } else {
                showModal(state);
            }
        }

        // --- 3. Connect Four Renderer ---
        function renderConnectFourGame(state) {
            const cols = 7;
            const rows = 6;
            const cw = canvas.width / cols;
            const ch = canvas.height / rows;

            ctx.fillStyle = '#0b1329';
            ctx.fillRect(0, 0, canvas.width, canvas.height);

            if (state.board) {
                for (let r = 0; r < rows; r++) {
                    for (let c = 0; c < cols; c++) {
                        const idx = r * cols + c;
                        const val = state.board[idx];
                        const cx = c * cw + cw / 2;
                        const cy = r * ch + ch / 2;
                        const radius = Math.min(cw, ch) * 0.38;

                        ctx.fillStyle = '#070b14';
                        ctx.beginPath();
                        ctx.arc(cx, cy, radius, 0, Math.PI * 2);
                        ctx.fill();

                        if (val === 1) {
                            ctx.fillStyle = '#38bdf8';
                            ctx.beginPath();
                            ctx.arc(cx, cy, radius - 2, 0, Math.PI * 2);
                            ctx.fill();
                        } else if (val === 2) {
                            ctx.fillStyle = '#ec4899';
                            ctx.beginPath();
                            ctx.arc(cx, cy, radius - 2, 0, Math.PI * 2);
                            ctx.fill();
                        }
                    }
                }
            }

            if (!state.finished) {
                hudStatus.textContent = state.turn === myPlayerNum ? "Your Turn (Click Column)" : "Opponent Turn";
            } else {
                showModal(state);
            }
        }

        // --- 4. Pong Renderer ---
        function renderPongGame(state) {
            ctx.fillStyle = '#070b14';
            ctx.fillRect(0, 0, canvas.width, canvas.height);

            // Center dashed court line
            ctx.strokeStyle = '#1e293b';
            ctx.lineWidth = 2;
            ctx.setLineDash([6, 6]);
            ctx.beginPath(); ctx.moveTo(canvas.width / 2, 0); ctx.lineTo(canvas.width / 2, canvas.height); ctx.stroke();
            ctx.setLineDash([]);

            // Paddles
            const padW = 10;
            const padH = 60;
            ctx.fillStyle = '#38bdf8';
            ctx.fillRect(padW, (state.p1y || 200) - padH / 2, padW, padH);

            ctx.fillStyle = '#ec4899';
            ctx.fillRect(canvas.width - padW * 2, (state.p2y || 200) - padH / 2, padW, padH);

            // Ball
            ctx.fillStyle = '#f1f5f9';
            ctx.beginPath();
            ctx.arc(state.bx || 200, state.by || 200, 8, 0, Math.PI * 2);
            ctx.fill();

            hudP1Score.textContent = state.score1 || 0;
            hudP2Score.textContent = state.score2 || 0;
            hudStatus.textContent = "First to 5 Wins";

            if (state.finished) showModal(state);
        }

        // --- 5. Tron Light Cycles Renderer ---
        function renderTronGame(state) {
            const grid = 30;
            const sz = canvas.width / grid;

            ctx.fillStyle = '#070b14';
            ctx.fillRect(0, 0, canvas.width, canvas.height);

            if (state.grid) {
                for (let y = 0; y < grid; y++) {
                    for (let x = 0; x < grid; x++) {
                        const val = state.grid[y * grid + x];
                        if (val === 1) {
                            ctx.fillStyle = '#0284c7';
                            ctx.fillRect(x * sz, y * sz, sz, sz);
                        } else if (val === 2) {
                            ctx.fillStyle = '#be185d';
                            ctx.fillRect(x * sz, y * sz, sz, sz);
                        }
                    }
                }
            }

            // Draw player heads with brighter tint
            if (state.p1 && state.p1.alive) {
                ctx.fillStyle = '#38bdf8';
                ctx.fillRect(state.p1.x * sz, state.p1.y * sz, sz, sz);
            }
            if (state.p2 && state.p2.alive) {
                ctx.fillStyle = '#ec4899';
                ctx.fillRect(state.p2.x * sz, state.p2.y * sz, sz, sz);
            }

            hudStatus.textContent = "Avoid Wall Collision";
            if (state.finished) showModal(state);
        }

        // --- 6. Battleship Renderer ---
        function renderBattleshipGame(state) {
            const sz = canvas.width / 10;
            ctx.fillStyle = '#070b14';
            ctx.fillRect(0, 0, canvas.width, canvas.height);

            // Sonar grid lines
            ctx.strokeStyle = '#111e33';
            ctx.lineWidth = 1;
            for (let i = 0; i <= 10; i++) {
                ctx.beginPath(); ctx.moveTo(i * sz, 0); ctx.lineTo(i * sz, canvas.height); ctx.stroke();
                ctx.beginPath(); ctx.moveTo(0, i * sz); ctx.lineTo(canvas.width, i * sz); ctx.stroke();
            }

            const me = myPlayerNum === 1 ? state.p1 : state.p2;

            if (state.phase === 'placement') {
                battleshipControls.style.display = 'flex';
                hudStatus.textContent = (myPlayerNum === 1 ? state.p1ready : state.p2ready) ?
                    "Fleet Deployed! Waiting for opponent..." : "Tap 'Deploy Fleet' to Arrange";

                // Draw own ships
                if (me && me.own) {
                    me.own.forEach((cell, idx) => {
                        const r = Math.floor(idx / 10);
                        const c = idx % 10;
                        if (cell >= 1) {
                            ctx.fillStyle = '#38bdf8';
                            ctx.fillRect(c * sz + 2, r * sz + 2, sz - 4, sz - 4);
                        }
                    });
                }
            } else {
                battleshipControls.style.display = 'none';
                hudStatus.textContent = state.turn === myPlayerNum ? "Your Turn: Tap Sector to Fire" : "Opponent Aiming...";

                // Draw enemy grid targeting view
                if (me && me.enemy) {
                    me.enemy.forEach((cell, idx) => {
                        const r = Math.floor(idx / 10);
                        const c = idx % 10;
                        const cx = c * sz + sz / 2;
                        const cy = r * sz + sz / 2;
                        if (cell === 2) { // Hit
                            ctx.fillStyle = '#ef4444';
                            ctx.fillRect(c * sz + 4, r * sz + 4, sz - 8, sz - 8);
                        } else if (cell === 3) { // Miss
                            ctx.fillStyle = '#475569';
                            ctx.beginPath();
                            ctx.arc(cx, cy, 3, 0, Math.PI * 2);
                            ctx.fill();
                        }
                    });
                }
            }

            hudP1Score.textContent = state.ships_left_p1 !== undefined ? state.ships_left_p1 : 0;
            hudP2Score.textContent = state.ships_left_p2 !== undefined ? state.ships_left_p2 : 0;

            if (state.finished) showModal(state);
        }

        // --- Modal Dialog (No Emojis, Pure SVG) ---
        function showModal(state) {
            modalGameOver.classList.add('active');
            let isWin = (state.winner === myPlayerNum) || (state.game === 'snake' && !state.duel);
            let isDraw = state.winner === -1;
            let title = isWin ? "VICTORY" : (isDraw ? "DRAW" : "DEFEAT");
            let sub = "";

            if (state.game === 'snake') {
                sub = state.duel ? (isWin ? "Dominated the Snake Arena!" : (isDraw ? "Mutual collision." : "Opponent outmaneuvered you.")) : `Final Score: ${state.score || 0}`;
            } else if (state.game === 'tictactoe') {
                sub = isWin ? "Three in a row achieved!" : (isDraw ? "Grid deadlock." : "Opponent made three in a row.");
            } else if (state.game === 'connect4') {
                sub = isWin ? "Four-in-a-row connection!" : (isDraw ? "Board completely full." : "Opponent connected four.");
            } else if (state.game === 'pong') {
                sub = isWin ? "Paddle victory!" : "Opponent scored 5 first.";
            } else if (state.game === 'tron') {
                sub = isWin ? "Light cycle survival!" : (isDraw ? "Mutual crash." : "Crashed into light trail.");
            } else if (state.game === 'battleship') {
                sub = isWin ? "Enemy naval fleet sunk!" : "Your naval fleet was sunk.";
            }

            modalTitle.textContent = title;
            modalTitle.style.color = isWin ? '#4ade80' : (isDraw ? '#f59e0b' : '#f87171');
            modalSub.textContent = sub;

            // Render SVG Icon
            if (isWin) {
                modalIconContainer.innerHTML = `
                    <svg width="40" height="40" viewBox="0 0 24 24" fill="none" stroke="#4ade80" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                        <polygon points="12 2 15.09 8.26 22 9.27 17 14.14 18.18 21.02 12 17.77 5.82 21.02 7 14.14 2 9.27 8.91 8.26 12 2"></polygon>
                    </svg>`;
            } else if (isDraw) {
                modalIconContainer.innerHTML = `
                    <svg width="40" height="40" viewBox="0 0 24 24" fill="none" stroke="#f59e0b" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                        <circle cx="12" cy="12" r="9"></circle>
                        <line x1="8" y1="12" x2="16" y2="12"></line>
                    </svg>`;
            } else {
                modalIconContainer.innerHTML = `
                    <svg width="40" height="40" viewBox="0 0 24 24" fill="none" stroke="#f87171" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                        <path d="M12 22s8-4 8-10V5l-8-3-8 3v7c0 6 8 10 8 10z"></path>
                        <line x1="12" y1="8" x2="12" y2="12"></line>
                        <line x1="12" y1="16" x2="12.01" y2="16"></line>
                    </svg>`;
            }
        }

        connect();
    </script>
</body>
</html>
)rawliteral";

};

} // namespace web
} // namespace gamehub
