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
        return std::string(INDEX_HTML_PART1) + INDEX_HTML_PART2 + INDEX_HTML_PART2B + INDEX_HTML_PART3 + 
               INDEX_HTML_PART3B + INDEX_HTML_PART4 + INDEX_HTML_PART4B + INDEX_HTML_PART5 + INDEX_HTML_PART5B + 
               INDEX_HTML_PART5C + INDEX_HTML_PART6 + INDEX_HTML_PART6B + INDEX_HTML_PART7 + INDEX_HTML_PART8 + 
               INDEX_HTML_PART9 + INDEX_HTML_PART10;
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
            --surface: rgba(17, 24, 39, 0.82);
            --surface-hover: rgba(31, 41, 61, 0.85);
            --surface-active: rgba(39, 53, 79, 0.9);
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
            --radius-lg: 14px;
            --radius-md: 10px;
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
            backdrop-filter: blur(16px);
            -webkit-backdrop-filter: blur(16px);
            border: 1px solid rgba(255, 255, 255, 0.08);
            border-radius: var(--radius-lg);
            margin-bottom: 12px;
            box-shadow: 0 4px 20px rgba(0, 0, 0, 0.35);
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
)rawliteral";

    static constexpr const char* INDEX_HTML_PART2 = R"rawliteral(
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
        .room-name { font-weight: 700; font-size: 0.88rem; display: flex; align-items: center; }
        .room-sub { font-size: 0.72rem; color: var(--text-tertiary); }
        .btn-solid {
            background: var(--p1-cyan); color: #090d16; border: none;
            font-weight: 800; font-size: 0.78rem; padding: 7px 13px;
            border-radius: var(--radius-sm); cursor: pointer; transition: opacity 140ms;
        }
        .btn-solid:hover { opacity: 0.92; }

        /* Live Radar Beacon (ADR 0013) */
        .radar-beacon {
            width: 8px; height: 8px; border-radius: 50%;
            background: #22c55e;
            box-shadow: 0 0 0 0 rgba(34, 197, 94, 0.7);
            animation: radarPulse 1.8s infinite;
            display: inline-block; margin-right: 6px; flex-shrink: 0;
        }
        @keyframes radarPulse {
            0% { transform: scale(0.95); box-shadow: 0 0 0 0 rgba(34, 197, 94, 0.7); }
            70% { transform: scale(1); box-shadow: 0 0 0 7px rgba(34, 197, 94, 0); }
            100% { transform: scale(0.95); box-shadow: 0 0 0 0 rgba(34, 197, 94, 0); }
        }

        /* Room Waiting Card */
        .waiting-box {
            background: var(--surface); border: 1px solid var(--border);
            border-radius: var(--radius-lg); padding: 24px;
            display: flex; flex-direction: column; align-items: center; text-align: center; gap: 14px;
        }
        .radar-pulse {
            width: 64px; height: 64px; border-radius: 50%;
            background: rgba(56, 189, 248, 0.08); border: 1px solid var(--p1-cyan);
            display: flex; align-items: center; justify-content: center;
        }
        .room-code-tag {
            font-size: 1.5rem; font-weight: 900; letter-spacing: 2px;
            color: var(--p1-cyan); font-family: monospace;
            background: #0b111e; padding: 6px 14px; border-radius: var(--radius-sm);
        }
        .btn-cancel {
            background: transparent; color: var(--danger); border: 1px solid rgba(239, 68, 68, 0.3);
            padding: 8px 16px; border-radius: var(--radius-sm); font-weight: 700; font-size: 0.8rem; cursor: pointer;
        }

        /* Game HUD Header */
        .game-hud {
            width: 100%; display: flex; justify-content: space-between; align-items: center;
            background: var(--surface); border: 1px solid var(--border);
            border-radius: var(--radius-md); padding: 8px 12px;
        }
        .hud-player { display: flex; align-items: center; gap: 6px; font-weight: 700; font-size: 0.85rem; }
        .hud-p1 { color: var(--p1-cyan); }
        .hud-p2 { color: var(--p2-pink); }
        .hud-score {
            font-size: 0.95rem; font-weight: 800; font-family: monospace;
            background: #0b111e; padding: 2px 7px; border-radius: 4px;
        }
        .bot-dot { width: 6px; height: 6px; border-radius: 50%; background: var(--bot-amber); display: inline-block; }

        /* Canvas & Viewport */
        #canvas-wrap {
            position: relative; width: 100%; max-width: 400px; aspect-ratio: 1/1;
            background: #070b14; border: 1px solid var(--border-highlight);
            border-radius: var(--radius-lg); overflow: hidden;
            box-shadow: 0 8px 24px rgba(0, 0, 0, 0.4);
            touch-action: none;
        }
        canvas#game-canvas { width: 100%; height: 100%; display: block; }

        /* Virtual D-pad (60px Buttons & Proper Row 3 - Issue 01) */
        .dpad-container {
            display: grid; grid-template-columns: repeat(3, 60px); grid-template-rows: repeat(3, 60px);
            gap: 8px; justify-content: center; margin-top: 8px;
        }
        .dpad-btn {
            width: 60px; height: 60px;
            background: rgba(17, 24, 39, 0.85); color: var(--text-secondary); border: 1px solid var(--border-highlight);
            border-radius: var(--radius-md); display: flex; align-items: center; justify-content: center;
            cursor: pointer; touch-action: manipulation; transition: background 80ms, color 80ms, transform 80ms, box-shadow 80ms;
            box-shadow: 0 4px 12px rgba(0, 0, 0, 0.3);
        }
        .dpad-btn:active { background: var(--p1-cyan); color: #090d16; transform: scale(0.92); box-shadow: 0 0 16px var(--p1-cyan); }
        .dpad-up { grid-column: 2; grid-row: 1; }
        .dpad-left { grid-column: 1; grid-row: 2; }
        .dpad-down { grid-column: 2; grid-row: 3; }
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
        .p3-dot { width: 8px; height: 8px; border-radius: 50%; background: #a855f7; }
        .domino-controls-bar {
            display: none; width: 100%; justify-content: center; gap: 8px; margin-top: 8px; margin-bottom: 4px;
        }
        .domino-end-choice {
            display: none; width: 100%; justify-content: center; gap: 8px; margin-top: 8px; margin-bottom: 4px;
        }
        .domino-btn {
            flex: 1; min-height: 40px; padding: 9px 14px; font-weight: 700; font-size: 0.88rem; border-radius: var(--radius-sm); cursor: pointer; transition: all 0.15s ease;
        }

        /* Bottom-Sheet Hall of Fame Drawer (Issue 08) */
        .drawer-overlay {
            position: fixed; inset: 0; background: rgba(0, 0, 0, 0.72);
            backdrop-filter: blur(8px); -webkit-backdrop-filter: blur(8px);
            z-index: 1000; display: none; align-items: flex-end; justify-content: center;
            opacity: 0; transition: opacity 200ms ease;
        }
        .drawer-overlay.active { display: flex; opacity: 1; }
        .drawer-card {
            background: rgba(17, 24, 39, 0.96); border: 1px solid var(--border-highlight);
            border-radius: 20px 20px 0 0; width: 100%; max-width: 440px;
            max-height: 85vh; overflow-y: auto; padding: 14px 18px 24px;
            box-shadow: 0 -8px 32px rgba(0, 0, 0, 0.8); transform: translateY(100%);
            transition: transform 250ms cubic-bezier(0.16, 1, 0.3, 1);
            display: flex; flex-direction: column; gap: 12px;
        }
        .drawer-overlay.active .drawer-card { transform: translateY(0); }
        .drawer-handle {
            width: 40px; height: 5px; border-radius: 3px; background: var(--text-tertiary);
            margin: 0 auto 4px;
        }
        .tier-badge {
            font-size: 0.65rem; font-weight: 800; padding: 2px 7px; border-radius: 9999px;
            text-transform: uppercase; letter-spacing: 0.5px;
        }
        .tier-bronze { background: rgba(205, 127, 50, 0.2); color: #cd7f32; border: 1px solid #cd7f32; }
        .tier-silver { background: rgba(192, 192, 192, 0.2); color: #c0c0c0; border: 1px solid #c0c0c0; }
        .tier-gold { background: rgba(255, 215, 0, 0.2); color: #ffd700; border: 1px solid #ffd700; }
        .tier-master { background: rgba(168, 85, 247, 0.2); color: #c084fc; border: 1px solid #c084fc; box-shadow: 0 0 8px rgba(168, 85, 247, 0.4); }

        /* CRT Scanline Filter Overlay (Issue 08) */
        body.crt-active::after {
            content: " "; display: block; position: fixed; inset: 0;
            background: linear-gradient(rgba(18, 16, 16, 0) 50%, rgba(0, 0, 0, 0.25) 50%),
                        linear-gradient(90deg, rgba(255, 0, 0, 0.04), rgba(0, 255, 0, 0.02), rgba(0, 0, 255, 0.04));
            z-index: 9999; background-size: 100% 3px, 6px 100%;
            pointer-events: none; opacity: 0.85;
        }
    </style>
</head>
)rawliteral";

    static constexpr const char* INDEX_HTML_PART2B = R"rawliteral(
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
            <button id="btn-crt" class="icon-btn" title="Toggle CRT Retro Filter">
                <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                    <rect x="2" y="3" width="20" height="14" rx="2"/><line x1="8" y1="21" x2="16" y2="21"/><line x1="12" y1="17" x2="12" y2="21"/>
                </svg>
            </button>
            <button id="btn-trophy" class="icon-btn" title="Hall of Fame">
                <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                    <path d="M6 9H4.5a2.5 2.5 0 0 1 0-5H6"/><path d="M18 9h1.5a2.5 2.5 0 0 0 0-5H18"/><path d="M4 22h16"/><path d="M10 14.66V17c0 .55-.45 1-1 1H7v4h10v-4h-2c-.55 0-1-.45-1-1v-2.34"/><path d="M6 4h12a2 2 0 0 1 2 2v3a6 6 0 0 1-12 0V6a2 2 0 0 1 2-2z"/>
                </svg>
            </button>
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
                            <span class="game-desc">Classic 4-in-a-row vertical gravity grid</span>
                        </div>
                    </div>
                    <span class="game-tag">vs Bot</span>
                </div>

                <div class="game-item" id="card-solo-pong">
                    <div class="game-info">
                        <div class="game-icon-box">
                            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="3" width="18" height="18" rx="2"/><line x1="3" y1="9" x2="3" y2="15"/><line x1="21" y1="9" x2="21" y2="15"/><circle cx="12" cy="12" r="2"/></svg>
                        </div>
                        <div class="game-text">
                            <span class="game-title">Pong Arcade</span>
                            <span class="game-desc">Fast-paced 60 FPS physics paddle duel</span>
                        </div>
                    </div>
                    <span class="game-tag">vs AI</span>
                </div>

                <div class="game-item" id="card-solo-tron">
                    <div class="game-info">
                        <div class="game-icon-box">
                            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M4 17l6-6 4 4 6-6"/><path d="M14 5h7v7"/></svg>
                        </div>
                        <div class="game-text">
                            <span class="game-title">Tron Light Cycles</span>
                            <span class="game-desc">Outlive the Voronoi / BFS arena bot</span>
                        </div>
                    </div>
                    <span class="game-tag">vs Bot</span>
                </div>

                <div class="game-item" id="card-solo-battleship">
                    <div class="game-info">
                        <div class="game-icon-box">
                            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"/><path d="M12 2a15.3 15.3 0 0 1 4 10 15.3 15.3 0 0 1-4 10 15.3 15.3 0 0 1-4-10 15.3 15.3 0 0 1 4-10z"/><line x1="2" y1="12" x2="22" y2="12"/></svg>
                        </div>
                        <div class="game-text">
                            <span class="game-title">Battleship Fleet</span>
                            <span class="game-desc">Seek and destroy enemy fleet formations</span>
                        </div>
                    </div>
                    <span class="game-tag">vs Bot</span>
                </div>

                <div class="game-item" id="card-solo-domino">
                    <div class="game-info">
                        <div class="game-icon-box">
                            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="4" y="2" width="16" height="20" rx="3"/><line x1="4" y1="12" x2="20" y2="12"/><circle cx="9" cy="7" r="1.3" fill="currentColor"/><circle cx="15" cy="7" r="1.3" fill="currentColor"/><circle cx="12" cy="17" r="1.3" fill="currentColor"/></svg>
                        </div>
                        <div class="game-text">
                            <span class="game-title">Block Dominoes</span>
                            <span class="game-desc">Classic 28-tile double-six domino match</span>
                        </div>
                    </div>
                    <span class="game-tag">vs Bot</span>
                </div>
            </div>
)rawliteral";

    static constexpr const char* INDEX_HTML_PART3 = R"rawliteral(
            <!-- Multiplayer Games List -->
            <div id="list-mp" class="game-list" style="display:none;">
                <div class="game-item" id="card-mp-snake">
                    <div class="game-info">
                        <div class="game-icon-box duel">
                            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 2a4 4 0 0 0-4 4v3a3 3 0 0 1-6 0V8"/><path d="M12 2a4 4 0 0 1 4 4v8a4 4 0 0 1-8 0v-2a2 2 0 0 1 4 0"/><circle cx="10" cy="5" r="1" fill="currentColor"/></svg>
                        </div>
                        <div class="game-text">
                            <span class="game-title">Snake Arena</span>
                            <span class="game-desc">2-player simultaneous arena survival duel</span>
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
                            <span class="game-desc">Online multiplayer turn-based showdown</span>
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
                            <span class="game-title">Connect Four</span>
                            <span class="game-desc">Turn-based 4-in-a-row vertical gravity duel</span>
                        </div>
                    </div>
                    <span class="game-tag">2 Players</span>
                </div>

                <div class="game-item" id="card-mp-pong">
                    <div class="game-info">
                        <div class="game-icon-box duel">
                            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="3" width="18" height="18" rx="2"/><line x1="3" y1="9" x2="3" y2="15"/><line x1="21" y1="9" x2="21" y2="15"/><circle cx="12" cy="12" r="2"/></svg>
                        </div>
                        <div class="game-text">
                            <span class="game-title">Pong Duel</span>
                            <span class="game-desc">Real-time authoritative paddle tennis match</span>
                        </div>
                    </div>
                    <span class="game-tag">2 Players</span>
                </div>

                <div class="game-item" id="card-mp-tron">
                    <div class="game-info">
                        <div class="game-icon-box duel">
                            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M4 17l6-6 4 4 6-6"/><path d="M14 5h7v7"/></svg>
                        </div>
                        <div class="game-text">
                            <span class="game-title">Tron Light Cycles</span>
                            <span class="game-desc">Cut off your opponent in real-time grid arena</span>
                        </div>
                    </div>
                    <span class="game-tag">2 Players</span>
                </div>

                <div class="game-item" id="card-mp-battleship">
                    <div class="game-info">
                        <div class="game-icon-box duel">
                            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"/><path d="M12 2a15.3 15.3 0 0 1 4 10 15.3 15.3 0 0 1-4 10 15.3 15.3 0 0 1-4-10 15.3 15.3 0 0 1 4-10z"/><line x1="2" y1="12" x2="22" y2="12"/></svg>
                        </div>
                        <div class="game-text">
                            <span class="game-title">Battleship Fleet</span>
                            <span class="game-desc">Strategic fog-of-war naval warfare duel</span>
                        </div>
                    </div>
                    <span class="game-tag">2 Players</span>
                </div>

                <div class="game-item" id="card-mp-domino">
                    <div class="game-info">
                        <div class="game-icon-box duel">
                            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="4" y="2" width="16" height="20" rx="3"/><line x1="4" y1="12" x2="20" y2="12"/><circle cx="9" cy="7" r="1.3" fill="currentColor"/><circle cx="15" cy="7" r="1.3" fill="currentColor"/><circle cx="12" cy="17" r="1.3" fill="currentColor"/></svg>
                        </div>
                        <div class="game-text">
                            <span class="game-title">2-3P Block Dominoes</span>
                            <span class="game-desc">Classic Block Dominoes arena with up to 3 players</span>
                        </div>
                    </div>
                    <span class="game-tag">2-3 Players</span>
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
)rawliteral";

    static constexpr const char* INDEX_HTML_PART3B = R"rawliteral(
        <!-- 2. ROOM WAITING VIEW -->
        <div id="view-waiting" class="view">
            <div class="waiting-box">
                <div class="radar-pulse">
                    <svg width="28" height="28" viewBox="0 0 24 24" fill="none" stroke="var(--p1-cyan)" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                        <path d="M12 2a10 10 0 1 0 10 10A10 10 0 0 0 12 2zm0 18a8 8 0 1 1 8-8 8 8 0 0 1-8 8z"/>
                        <path d="M12 6a6 6 0 1 0 6 6 6 6 0 0 0-6-6zm0 10a4 4 0 1 1 4-4 4 4 0 0 1-4 4z"/>
                    </svg>
                </div>
                <div style="display:flex; flex-direction:column; gap:4px;">
                    <span id="waiting-game-title" style="font-weight:800; font-size:1.15rem;">Waiting Room</span>
                    <span id="waiting-host-msg" style="font-size:0.8rem; color:var(--text-secondary);">Match is open in the lobby feed.</span>
                </div>
                <button id="btn-start-early" class="btn-solid" style="display:none; width:100%; padding:10px; font-size:0.9rem;">Start Match with Current Players</button>
                <button id="btn-cancel-room" class="btn-cancel">Cancel & Leave</button>
            </div>
        </div>

        <!-- 3. GAME RUNTIME VIEW -->
        <div id="view-game" class="view">
            <div class="game-hud">
                <div class="hud-player hud-p1">
                    <span class="dot" style="background:var(--p1-cyan);"></span>
                    <span id="hud-p1-name">P1</span>
                    <span class="hud-score" id="hud-p1-score">0</span>
                </div>
                <div id="hud-status" style="font-size:0.78rem; font-weight:700; color:var(--text-secondary);">Match Active</div>
                <div class="hud-player hud-p2">
                    <span class="hud-score" id="hud-p2-score">0</span>
                    <span id="hud-p2-name">P2</span>
                    <span class="dot" id="hud-p2-marker" style="background:var(--p2-pink);"></span>
                </div>
            </div>

            <!-- Domino 3rd Player HUD Bar -->
            <div id="hud-p3-wrap" style="display:none; width:100%; justify-content:center; gap:8px; font-size:0.75rem; color:#c084fc; font-weight:700; margin-top:-6px;">
                <span class="p3-dot"></span>
                <span id="hud-p3-name">P3</span>: <span id="hud-p3-score">0</span>
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

            <!-- Domino Controls (Positioned directly below canvas near player's hand) -->
            <div id="domino-controls" class="domino-controls-bar">
                <button id="btn-domino-draw" class="btn-solid domino-btn">Draw Tile (<span id="boneyard-count">14</span>)</button>
                <button id="btn-domino-pass" class="btn-secondary domino-btn">Pass Turn</button>
                <button id="btn-domino-mode" class="btn-secondary domino-btn" style="font-size:0.75rem; padding:6px 10px; min-width:85px;">Rule: Block</button>
            </div>
            <div id="domino-end-choice" class="domino-end-choice">
                <button id="btn-place-left" class="btn-solid domino-btn" style="background:#0284c7;">Place Left End</button>
                <button id="btn-place-right" class="btn-solid domino-btn" style="background:#0284c7;">Place Right End</button>
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

        <!-- Hall of Fame Bottom-Sheet Drawer (Issue 08) -->
        <div id="stats-drawer" class="drawer-overlay">
            <div class="drawer-card">
                <div class="drawer-handle"></div>
                <div style="display:flex; justify-content:space-between; align-items:center;">
                    <div style="font-weight:700; font-size:1.05rem; display:flex; align-items:center; gap:8px; color:var(--text);">
                        <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="var(--bot-amber)" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M6 9H4.5a2.5 2.5 0 0 1 0-5H6"/><path d="M18 9h1.5a2.5 2.5 0 0 0 0-5H18"/><path d="M4 22h16"/><path d="M10 14.66V17c0 .55-.45 1-1 1H7v4h10v-4h-2c-.55 0-1-.45-1-1v-2.34"/><path d="M6 4h12a2 2 0 0 1 2 2v3a6 6 0 0 1-12 0V6a2 2 0 0 1 2-2z"/></svg>
                        Hall of Fame
                    </div>
                    <button id="btn-close-stats" class="icon-btn" style="border:none; width:28px; height:28px;">
                        <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><line x1="18" y1="6" x2="6" y2="18"/><line x1="6" y1="6" x2="18" y2="18"/></svg>
                    </button>
                </div>
                <div id="stats-content" style="display:flex; flex-direction:column; gap:6px; font-size:0.85rem;"></div>
                <button id="btn-clear-stats" class="btn-secondary" style="font-size:0.75rem; padding:8px; margin-top:4px;">Reset Records</button>
            </div>
        </div>
    </div>
)rawliteral";

    static constexpr const char* INDEX_HTML_PART4 = R"rawliteral(
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
            laser() {
                if (this.muted) return;
                this.init();
                if (!this.ctx) return;
                try {
                    const osc = this.ctx.createOscillator();
                    const gain = this.ctx.createGain();
                    osc.type = 'sawtooth';
                    osc.frequency.setValueAtTime(880, this.ctx.currentTime);
                    osc.frequency.exponentialRampToValueAtTime(110, this.ctx.currentTime + 0.1);
                    gain.gain.setValueAtTime(0.09, this.ctx.currentTime);
                    gain.gain.exponentialRampToValueAtTime(0.001, this.ctx.currentTime + 0.1);
                    osc.connect(gain); gain.connect(this.ctx.destination);
                    osc.start(); osc.stop(this.ctx.currentTime + 0.1);
                } catch(e) {}
            }
            hit() {
                if (this.muted) return;
                this.init();
                if (!this.ctx) return;
                try {
                    const osc = this.ctx.createOscillator();
                    const gain = this.ctx.createGain();
                    osc.type = 'square';
                    osc.frequency.setValueAtTime(160, this.ctx.currentTime);
                    osc.frequency.linearRampToValueAtTime(30, this.ctx.currentTime + 0.14);
                    gain.gain.setValueAtTime(0.16, this.ctx.currentTime);
                    gain.gain.exponentialRampToValueAtTime(0.001, this.ctx.currentTime + 0.14);
                    osc.connect(gain); gain.connect(this.ctx.destination);
                    osc.start(); osc.stop(this.ctx.currentTime + 0.14);
                } catch(e) {}
            }
            eat() {
                this.playTone(523.25, 'sine', 0.08, 0.12);
                setTimeout(() => this.playTone(659.25, 'sine', 0.08, 0.12), 60);
                setTimeout(() => this.playTone(783.99, 'sine', 0.12, 0.12), 120);
            }
            place() {
                this.playTone(320, 'triangle', 0.04, 0.12);
                setTimeout(() => this.playTone(480, 'sine', 0.03, 0.1), 30);
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

        // --- Universal Particle Engine (ADR 0013 / Issue 06) ---
        class ParticleEngine {
            constructor(canvas, ctx) {
                this.canvas = canvas;
                this.ctx = ctx;
                this.particles = [];
            }
            confetti(x = 200, y = 200, count = 55) {
                const colors = ['#38bdf8', '#ec4899', '#f59e0b', '#22c55e', '#a855f7', '#ffffff'];
                for (let i = 0; i < count; i++) {
                    const angle = Math.random() * Math.PI * 2;
                    const speed = 2 + Math.random() * 5.5;
                    this.particles.push({
                        type: 'confetti', x, y,
                        vx: Math.cos(angle) * speed,
                        vy: Math.sin(angle) * speed - 2.5,
                        color: colors[Math.floor(Math.random() * colors.length)],
                        size: 3 + Math.random() * 3.5,
                        rot: Math.random() * 360, vRot: (Math.random() - 0.5) * 12,
                        alpha: 1, decay: 0.012 + Math.random() * 0.012, gravity: 0.11
                    });
                }
            }
            sparks(x, y, count = 16, color = '#38bdf8') {
                for (let i = 0; i < count; i++) {
                    const angle = Math.random() * Math.PI * 2;
                    const speed = 1.2 + Math.random() * 4.2;
                    this.particles.push({
                        type: 'spark', x, y,
                        vx: Math.cos(angle) * speed, vy: Math.sin(angle) * speed,
                        color, size: 1.5 + Math.random() * 2,
                        alpha: 1, decay: 0.03 + Math.random() * 0.035, gravity: 0.04
                    });
                }
            }
            ripple(x, y, color = '#38bdf8') {
                this.particles.push({ type: 'ripple', x, y, radius: 2, maxRadius: 26, color, alpha: 1, decay: 0.035 });
            }
            explosion(x, y, color = '#ef4444') {
                this.sparks(x, y, 20, '#f59e0b');
                for (let i = 0; i < 10; i++) {
                    const angle = Math.random() * Math.PI * 2;
                    const speed = 0.5 + Math.random() * 2;
                    this.particles.push({
                        type: 'smoke', x, y,
                        vx: Math.cos(angle) * speed, vy: Math.sin(angle) * speed - 0.4,
                        color, size: 4 + Math.random() * 5, alpha: 0.8, decay: 0.02 + Math.random() * 0.02
                    });
                }
            }
            updateAndRender() {
                if (this.particles.length === 0) return;
                const ctx = this.ctx;
                for (let i = this.particles.length - 1; i >= 0; i--) {
                    const p = this.particles[i];
                    p.alpha -= p.decay;
                    if (p.alpha <= 0) { this.particles.splice(i, 1); continue; }
                    ctx.globalAlpha = Math.max(0, p.alpha);
                    if (p.type === 'confetti') {
                        p.x += p.vx; p.y += p.vy; p.vy += p.gravity; p.rot += p.vRot;
                        ctx.save(); ctx.translate(p.x, p.y); ctx.rotate(p.rot * Math.PI / 180);
                        ctx.fillStyle = p.color; ctx.fillRect(-p.size / 2, -p.size / 2, p.size, p.size * 0.6);
                        ctx.restore();
                    } else if (p.type === 'spark') {
                        p.x += p.vx; p.y += p.vy; p.vy += p.gravity;
                        ctx.fillStyle = p.color; ctx.beginPath(); ctx.arc(p.x, p.y, p.size, 0, Math.PI * 2); ctx.fill();
                    } else if (p.type === 'ripple') {
                        p.radius += 1.2; ctx.strokeStyle = p.color; ctx.lineWidth = 1.5;
                        ctx.beginPath(); ctx.arc(p.x, p.y, p.radius, 0, Math.PI * 2); ctx.stroke();
                    } else if (p.type === 'smoke') {
                        p.x += p.vx; p.y += p.vy; p.size += 0.25;
                        ctx.fillStyle = p.color; ctx.beginPath(); ctx.arc(p.x, p.y, p.size, 0, Math.PI * 2); ctx.fill();
                    }
                }
                ctx.globalAlpha = 1.0;
            }
        }

        // --- Camera Juice (ADR 0013 / Issue 06) ---
        class CameraJuice {
            constructor() {
                this.intensity = 0;
                this.decay = 0.88;
                this.offsetX = 0;
                this.offsetY = 0;
                this.reduced = window.matchMedia('(prefers-reduced-motion: reduce)').matches;
            }
            shake(tier = 4) {
                if (this.reduced) return;
                this.intensity = Math.min(this.intensity + tier, 18);
            }
            update() {
                if (this.intensity > 0.1) {
                    this.offsetX = (Math.random() - 0.5) * 2 * this.intensity;
                    this.offsetY = (Math.random() - 0.5) * 2 * this.intensity;
                    this.intensity *= this.decay;
                } else {
                    this.intensity = 0; this.offsetX = 0; this.offsetY = 0;
                }
            }
            apply(ctx) {
                if (this.intensity > 0) ctx.translate(this.offsetX, this.offsetY);
            }
        }

        // --- Clamped 2x Retina Viewport (Issue 06) ---
        function initHiDPICanvas(cvs, width = 400, height = 400) {
            const dpr = Math.min(window.devicePixelRatio || 1, 2);
            cvs.width = width * dpr;
            cvs.height = height * dpr;
            cvs.style.width = width + 'px';
            cvs.style.height = height + 'px';
            const c = cvs.getContext('2d');
            c.scale(dpr, dpr);
            return c;
        }
)rawliteral";

    static constexpr const char* INDEX_HTML_PART4B = R"rawliteral(
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
        const waitingHostMsg = document.getElementById('waiting-host-msg');
        const btnCancelRoom = document.getElementById('btn-cancel-room');
        const btnForfeit = document.getElementById('btn-forfeit');
        const battleshipControls = document.getElementById('battleship-controls');
        const btnDeployFleet = document.getElementById('btn-deploy-fleet');
        const dominoControls = document.getElementById('domino-controls');
        const dominoEndChoice = document.getElementById('domino-end-choice');
        const btnDominoDraw = document.getElementById('btn-domino-draw');
        const btnDominoPass = document.getElementById('btn-domino-pass');
        const btnDominoMode = document.getElementById('btn-domino-mode');
        const btnPlaceLeft = document.getElementById('btn-place-left');
        const btnPlaceRight = document.getElementById('btn-place-right');
        const boneyardCountSpan = document.getElementById('boneyard-count');
        const btnStartEarly = document.getElementById('btn-start-early');

        const canvas = document.getElementById('game-canvas');
        const ctx = initHiDPICanvas(canvas, 400, 400);
        const particleEngine = new ParticleEngine(canvas, ctx);
        const cameraJuice = new CameraJuice();

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
        const hudP3Wrap = document.getElementById('hud-p3-wrap');
        const hudP3Name = document.getElementById('hud-p3-name');
        const hudP3Score = document.getElementById('hud-p3-score');
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
        let selectedDominoIdx = -1;

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
                svgSoundOn.style.display = 'none'; svgSoundOff.style.display = 'block';
            } else {
                svgSoundOn.style.display = 'block'; svgSoundOff.style.display = 'none';
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
            tabBtnSolo.classList.add('active'); tabBtnMp.classList.remove('active');
            listSolo.style.display = 'flex'; listMp.style.display = 'none';
        });
        tabBtnMp.addEventListener('click', () => {
            sound.click();
            tabBtnMp.classList.add('active'); tabBtnSolo.classList.remove('active');
            listMp.style.display = 'flex'; listSolo.style.display = 'none';
        });

        function resetClientGameState() {
            lastState = null;
            prevFinished = false;
            prevScore = 0;
            particleEngine.particles = [];
            selectedDominoIdx = -1;
            c4DroppingChips = [];
            c4PrevBoard = null;
            pongTrails = [];
            if (window._dominoAutoPassTimer) {
                clearTimeout(window._dominoAutoPassTimer);
                window._dominoAutoPassTimer = null;
            }
            if (modalGameOver) modalGameOver.classList.remove('active');
            if (dominoControls) dominoControls.style.display = 'none';
            if (dominoEndChoice) dominoEndChoice.style.display = 'none';
            if (btnStartEarly) btnStartEarly.style.display = 'none';
            if (hudP3Wrap) hudP3Wrap.style.display = 'none';
        }

        function switchView(name) {
            [viewLobby, viewWaiting, viewGame].forEach(v => v.classList.remove('active'));
            if (modalGameOver) modalGameOver.classList.remove('active');
            if (name === 'lobby') {
                resetClientGameState();
                viewLobby.classList.add('active');
            } else if (name === 'waiting') {
                viewWaiting.classList.add('active');
            } else if (name === 'game') {
                viewGame.classList.add('active');
                setupGameView(activeGameType);
            }
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

    static constexpr const char* INDEX_HTML_PART5 = R"rawliteral(
        function getGameTitle(gameKey) {
            switch(gameKey) {
                case 'snake_duel': return 'Snake Arena';
                case 'snake_solo': return 'Snake Survival';
                case 'ttt_pvp': return '2P Tic-Tac-Toe';
                case 'ttt_solo': return 'Tic-Tac-Toe';
                case 'connect4_pvp': return 'Connect Four';
                case 'connect4_solo': return 'Connect Four';
                case 'pong_duel': return 'Pong Duel';
                case 'pong_solo': return 'Pong Arcade';
                case 'tron_duel': return 'Tron Light Cycles';
                case 'tron_solo': return 'Tron Light Cycles';
                case 'battleship_pvp': return 'Battleship Fleet';
                case 'battleship_solo': return 'Battleship Fleet';
                case 'domino_pvp': return '2-3P Block Dominoes';
                case 'domino_solo': return 'Block Dominoes';
                default: return 'Arcade Game';
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
                waitingHostMsg.textContent = 'Visible in the match feed. Anyone can join directly.';
                btnStartEarly.style.display = 'none';
            } else if (data.type === 'room_joined') {
                activeRoomCode = data.code;
                isHost = false;
                myPlayerNum = data.player || 2;
                opponentName = data.host || "Host";
                hudP2Name.textContent = nicknameInput.value;
                hudP1Name.textContent = opponentName;
                setupGameView(activeGameType);
                switchView('game');
                sound.click();
            } else if (data.type === 'room_waiting_update') {
                waitingHostMsg.textContent = `${data.count} / ${data.max} players joined.`;
                if (isHost && data.count >= 2) {
                    btnStartEarly.style.display = 'block';
                }
            } else if (data.type === 'rematch_update') {
                const reqCount = (data.requested && data.requested.length) || 1;
                const total = data.needed || 2;
                btnModalReplay.textContent = `Rematch (${reqCount}/${total})`;
            } else if (data.type === 'room_ready') {
                modalGameOver.classList.remove('active');
                btnModalReplay.textContent = "Play Again";
                btnStartEarly.style.display = 'none';
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
                if (!data.finished) {
                    modalGameOver.classList.remove('active');
                    btnModalReplay.textContent = "Play Again";
                }
                renderGameState(data);
            }
        }

        // Live Radar Beacon in Directory Feed (Issue 08)
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
                        <span class="room-name"><span class="radar-beacon"></span>${escapeHtml(r.host)} · ${getGameTitle(r.game)}</span>
                        <span class="room-sub">${r.players || 1}/${r.max || 2} players · Click to join</span>
                    </div>
                    <button class="btn-solid" data-code="${r.code}">Join Match</button>
                `;
                item.querySelector('.btn-solid').addEventListener('click', () => {
                    sound.click();
                    haptic(20);
                    if (ws && ws.readyState === WebSocket.OPEN) {
                        ws.send(JSON.stringify({ cmd: 'leave' }));
                        resetClientGameState();
                        activeGameType = r.game;
                        ws.send(JSON.stringify({ cmd: 'join', code: r.code }));
                    }
                });
                roomDirectoryList.appendChild(item);
            });
        }

        function escapeHtml(str) {
            return String(str || '').replace(/[&<>"']/g, s => ({
                '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;'
            }[s]));
        }

        function setupGameView(gt) {
            const isSolo = gt && gt.indexOf('_solo') !== -1;
            battleshipControls.style.display = 'none';
            dominoControls.style.display = 'none';
            dominoEndChoice.style.display = 'none';

            if (gt && (gt.indexOf('snake') !== -1 || gt.indexOf('pong') !== -1 || gt.indexOf('tron') !== -1)) {
                touchControls.style.display = 'grid';
            } else {
                touchControls.style.display = 'none';
            }

            if (isSolo) {
                hudP1Name.textContent = nicknameInput.value;
                if (gt === 'snake_solo') {
                    hudP2Name.textContent = "Record";
                    hudP2Marker.style.display = 'none';
                    hudP2Score.textContent = localStorage.getItem('gamebox_snake_high') || '0';
                } else {
                    hudP2Name.textContent = "Bot";
                    hudP2Marker.className = 'bot-dot';
                    hudP2Marker.style.display = 'inline-block';
                }
            } else {
                hudP2Marker.className = 'dot';
                hudP2Marker.style.display = 'inline-block';
            }
        }
)rawliteral";

    static constexpr const char* INDEX_HTML_PART5B = R"rawliteral(
        // --- Solo Game Triggers ---
        document.getElementById('card-solo-snake').addEventListener('click', () => {
            sound.click();
            if (ws && ws.readyState === WebSocket.OPEN) ws.send(JSON.stringify({ cmd: 'leave' }));
            resetClientGameState();
            activeGameType = 'snake_solo';
            myPlayerNum = 1;
            setupGameView('snake_solo');
            switchView('game');
            ws.send(JSON.stringify({ cmd: 'start', game: 'snake_solo' }));
            startGameLoop();
        });

        document.getElementById('card-solo-ttt').addEventListener('click', () => {
            sound.click();
            if (ws && ws.readyState === WebSocket.OPEN) ws.send(JSON.stringify({ cmd: 'leave' }));
            resetClientGameState();
            activeGameType = 'ttt_solo';
            myPlayerNum = 1;
            setupGameView('ttt_solo');
            switchView('game');
            ws.send(JSON.stringify({ cmd: 'start', game: 'ttt_solo' }));
        });

        document.getElementById('card-solo-c4').addEventListener('click', () => {
            sound.click();
            if (ws && ws.readyState === WebSocket.OPEN) ws.send(JSON.stringify({ cmd: 'leave' }));
            resetClientGameState();
            activeGameType = 'connect4_solo';
            myPlayerNum = 1;
            setupGameView('connect4_solo');
            switchView('game');
            ws.send(JSON.stringify({ cmd: 'start', game: 'connect4_solo' }));
        });

        document.getElementById('card-solo-pong').addEventListener('click', () => {
            sound.click();
            if (ws && ws.readyState === WebSocket.OPEN) ws.send(JSON.stringify({ cmd: 'leave' }));
            resetClientGameState();
            activeGameType = 'pong_solo';
            myPlayerNum = 1;
            setupGameView('pong_solo');
            switchView('game');
            ws.send(JSON.stringify({ cmd: 'start', game: 'pong_solo' }));
            startGameLoop();
        });

        document.getElementById('card-solo-tron').addEventListener('click', () => {
            sound.click();
            if (ws && ws.readyState === WebSocket.OPEN) ws.send(JSON.stringify({ cmd: 'leave' }));
            resetClientGameState();
            activeGameType = 'tron_solo';
            myPlayerNum = 1;
            setupGameView('tron_solo');
            switchView('game');
            ws.send(JSON.stringify({ cmd: 'start', game: 'tron_solo' }));
            startGameLoop();
        });

        document.getElementById('card-solo-battleship').addEventListener('click', () => {
            sound.click();
            if (ws && ws.readyState === WebSocket.OPEN) ws.send(JSON.stringify({ cmd: 'leave' }));
            resetClientGameState();
            activeGameType = 'battleship_solo';
            myPlayerNum = 1;
            setupGameView('battleship_solo');
            switchView('game');
            ws.send(JSON.stringify({ cmd: 'start', game: 'battleship_solo' }));
        });

        document.getElementById('card-solo-domino').addEventListener('click', () => {
            sound.click();
            if (ws && ws.readyState === WebSocket.OPEN) ws.send(JSON.stringify({ cmd: 'leave' }));
            resetClientGameState();
            activeGameType = 'domino_solo';
            myPlayerNum = 1;
            setupGameView('domino_solo');
            switchView('game');
            ws.send(JSON.stringify({ cmd: 'start', game: 'domino_solo' }));
        });

        // --- Multiplayer Game Triggers ---
        function hostMultiplayer(gameType) {
            sound.click();
            if (ws && ws.readyState === WebSocket.OPEN) ws.send(JSON.stringify({ cmd: 'leave' }));
            resetClientGameState();
            activeGameType = gameType;
            ws.send(JSON.stringify({ cmd: 'create', game: gameType, host: nicknameInput.value }));
        }

        document.getElementById('card-mp-snake').addEventListener('click', () => hostMultiplayer('snake_duel'));
        document.getElementById('card-mp-ttt').addEventListener('click', () => hostMultiplayer('ttt_pvp'));
        document.getElementById('card-mp-c4').addEventListener('click', () => hostMultiplayer('connect4_pvp'));
        document.getElementById('card-mp-pong').addEventListener('click', () => hostMultiplayer('pong_duel'));
        document.getElementById('card-mp-tron').addEventListener('click', () => hostMultiplayer('tron_duel'));
        document.getElementById('card-mp-battleship').addEventListener('click', () => hostMultiplayer('battleship_pvp'));
        document.getElementById('card-mp-domino').addEventListener('click', () => hostMultiplayer('domino_pvp'));

        btnStartEarly.addEventListener('click', () => {
            sound.click();
            if (ws && ws.readyState === WebSocket.OPEN) ws.send(JSON.stringify({ cmd: 'start_room' }));
        });

        btnDominoDraw.addEventListener('click', () => {
            sound.click(); haptic(15);
            if (ws && ws.readyState === WebSocket.OPEN) ws.send(JSON.stringify({ cmd: 'draw' }));
        });

        btnDominoPass.addEventListener('click', () => {
            sound.move(); haptic(15);
            if (ws && ws.readyState === WebSocket.OPEN) ws.send(JSON.stringify({ cmd: 'pass' }));
        });

        if (btnDominoMode) {
            btnDominoMode.addEventListener('click', () => {
                sound.click(); haptic(15);
                if (ws && ws.readyState === WebSocket.OPEN && lastState) {
                    const nextMode = (lastState.block_mode !== false) ? 'draw' : 'block';
                    ws.send(JSON.stringify({ cmd: 'mode', mode: nextMode }));
                }
            });
        }

        btnPlaceLeft.addEventListener('click', () => {
            if (selectedDominoIdx >= 0 && ws && ws.readyState === WebSocket.OPEN) {
                sound.place();
                ws.send(JSON.stringify({ cmd: 'play', index: selectedDominoIdx, end: 'left' }));
                selectedDominoIdx = -1;
                dominoEndChoice.style.display = 'none';
            }
        });

        btnPlaceRight.addEventListener('click', () => {
            if (selectedDominoIdx >= 0 && ws && ws.readyState === WebSocket.OPEN) {
                sound.place();
                ws.send(JSON.stringify({ cmd: 'play', index: selectedDominoIdx, end: 'right' }));
                selectedDominoIdx = -1;
                dominoEndChoice.style.display = 'none';
            }
        });

        btnCancelRoom.addEventListener('click', () => {
            sound.click();
            if (ws && ws.readyState === WebSocket.OPEN) ws.send(JSON.stringify({ cmd: 'leave' }));
            resetClientGameState();
            switchView('lobby');
        });

        btnForfeit.addEventListener('click', () => {
            sound.click();
            stopGameLoop();
            if (ws && ws.readyState === WebSocket.OPEN) ws.send(JSON.stringify({ cmd: 'leave' }));
            resetClientGameState();
            switchView('lobby');
        });

        btnModalLobby.addEventListener('click', () => {
            sound.click();
            stopGameLoop();
            if (ws && ws.readyState === WebSocket.OPEN) ws.send(JSON.stringify({ cmd: 'leave' }));
            resetClientGameState();
            switchView('lobby');
            if (ws && ws.readyState === WebSocket.OPEN) ws.send(JSON.stringify({ cmd: 'get_rooms' }));
        });

        // Rematch & Replay Handling (ADR 0013 / Issue 02)
        btnModalReplay.addEventListener('click', () => {
            sound.click();
            if (activeGameType && activeGameType.indexOf('_solo') === -1 && activeRoomCode) {
                btnModalReplay.textContent = "Rematch Requested...";
                btnModalReplay.disabled = true;
                setTimeout(() => { btnModalReplay.disabled = false; }, 1200);
                if (ws && ws.readyState === WebSocket.OPEN) {
                    ws.send(JSON.stringify({ cmd: 'rematch' }));
                }
                return;
            }
            stopGameLoop();
            resetClientGameState();
            if (activeGameType === 'snake_solo') {
                ws.send(JSON.stringify({ cmd: 'start', game: 'snake_solo' }));
                startGameLoop();
            } else if (activeGameType === 'ttt_solo') {
                ws.send(JSON.stringify({ cmd: 'start', game: 'ttt_solo' }));
            } else if (activeGameType === 'connect4_solo') {
                ws.send(JSON.stringify({ cmd: 'start', game: 'connect4_solo' }));
            } else if (activeGameType === 'domino_solo') {
                ws.send(JSON.stringify({ cmd: 'start', game: 'domino_solo' }));
            } else if (activeGameType === 'pong_solo') {
                ws.send(JSON.stringify({ cmd: 'start', game: 'pong_solo' }));
                startGameLoop();
            } else if (activeGameType === 'tron_solo') {
                ws.send(JSON.stringify({ cmd: 'start', game: 'tron_solo' }));
                startGameLoop();
            } else if (activeGameType === 'battleship_solo') {
                ws.send(JSON.stringify({ cmd: 'start', game: 'battleship_solo' }));
            } else {
                if (ws && ws.readyState === WebSocket.OPEN) ws.send(JSON.stringify({ cmd: 'leave' }));
                switchView('lobby');
            }
        });

        function startGameLoop() {
            clearInterval(gameLoop);
            gameLoop = setInterval(() => {
                if (ws && ws.readyState === WebSocket.OPEN && (activeGameType === 'snake_solo' || activeGameType === 'pong_solo' || activeGameType === 'tron_solo')) {
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

    static constexpr const char* INDEX_HTML_PART5C = R"rawliteral(
        // D-Pad and Keyboard Listeners
        const stopMoving = () => {
            if (activeGameType && activeGameType.indexOf('pong') !== -1) {
                sendDir('STOP');
            }
        };

        document.querySelectorAll('.dpad-btn').forEach(btn => {
            const dir = btn.getAttribute('data-dir');
            btn.addEventListener('pointerdown', (e) => {
                e.preventDefault();
                sendDir(dir);
            });
            btn.addEventListener('pointerup', stopMoving);
            btn.addEventListener('pointercancel', stopMoving);
            btn.addEventListener('pointerleave', stopMoving);
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

        window.addEventListener('keyup', (e) => {
            if (activeGameType && activeGameType.indexOf('pong') !== -1) {
                if (['ArrowUp', 'ArrowDown', 'w', 'W', 's', 'S'].includes(e.key)) {
                    sendDir('STOP');
                }
            }
        });

        // Pong Touch Drag & Tron/Snake Swipe Gestures (Issue 01)
        let touchStartX = 0;
        let touchStartY = 0;
        let swipeDone = false;
        let isPongDragging = false;

        canvas.addEventListener('pointerdown', (e) => {
            touchStartX = e.clientX;
            touchStartY = e.clientY;
            swipeDone = false;
            if (activeGameType && activeGameType.indexOf('pong') !== -1) {
                isPongDragging = true;
                handlePongPointerMove(e);
            }
        });

        function handlePongPointerMove(e) {
            if (!isPongDragging || !lastState) return;
            const rect = canvas.getBoundingClientRect();
            const y = (e.clientY - rect.top) * (400 / rect.height);
            const myPaddleY = (myPlayerNum === 2) ? (lastState.p2y || 200) : (lastState.p1y || 200);
            if (y < myPaddleY - 10) sendDir('UP');
            else if (y > myPaddleY + 10) sendDir('DOWN');
            else sendDir('STOP');
        }

        canvas.addEventListener('pointermove', (e) => {
            if (isPongDragging) {
                handlePongPointerMove(e);
                return;
            }
            if (activeGameType && (activeGameType.indexOf('snake') !== -1 || activeGameType.indexOf('tron') !== -1)) {
                if (swipeDone) return;
                const dx = e.clientX - touchStartX;
                const dy = e.clientY - touchStartY;
                if (Math.abs(dx) > 15 || Math.abs(dy) > 15) {
                    if (Math.abs(dx) > Math.abs(dy)) {
                        sendDir(dx > 0 ? 'RIGHT' : 'LEFT');
                    } else {
                        sendDir(dy > 0 ? 'DOWN' : 'UP');
                    }
                    swipeDone = true;
                    touchStartX = e.clientX;
                    touchStartY = e.clientY;
                }
            }
        });

        const endTouchGestures = () => {
            if (isPongDragging) {
                isPongDragging = false;
                sendDir('STOP');
            }
        };
        canvas.addEventListener('pointerup', endTouchGestures);
        canvas.addEventListener('pointercancel', endTouchGestures);
        canvas.addEventListener('pointerleave', endTouchGestures);
)rawliteral";

    static constexpr const char* INDEX_HTML_PART6 = R"rawliteral(
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
            const x = (e.clientX - rect.left) * (400 / rect.width);
            const y = (e.clientY - rect.top) * (400 / rect.height);

            // 1. Tic-Tac-Toe
            if (lastState.game === 'tictactoe') {
                const cellSize = 400 / 3;
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
                const colWidth = 400 / 7;
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
                    const cellSize = 40;
                    const col = Math.floor(x / cellSize);
                    const row = Math.floor(y / cellSize);
                    if (col >= 0 && col < 10 && row >= 0 && row < 10) {
                        sound.click();
                        haptic(25);
                        ws.send(JSON.stringify({ cmd: 'fire', row: row, col: col }));
                    }
                }
            }
            // 4. Domino Interaction
            else if (lastState.game === 'domino' && !lastState.finished) {
                if (lastState.turn === myPlayerNum) {
                    const myHand = (lastState.hands && lastState.hands[myPlayerNum]) || [];
                    const handCount = myHand.length;
                    if (handCount > 0 && y >= 300 && y <= 395) {
                        const tileW = Math.min(42, Math.max(20, Math.floor(380 / handCount)));
                        const totalW = handCount * tileW;
                        const startX = Math.max(10, Math.floor((400 - totalW) / 2));
                        if (x >= startX && x <= startX + totalW) {
                            const idx = Math.floor((x - startX) / tileW);
                            if (idx >= 0 && idx < handCount) {
                                const tile = myHand[idx];
                                const lEnd = lastState.left_end;
                                const rEnd = lastState.right_end;
                                if (lEnd === -1 && rEnd === -1) {
                                    sound.place(); haptic(20);
                                    ws.send(JSON.stringify({ cmd: 'play', index: idx, end: 'right' }));
                                } else {
                                    const mLeft = (tile[0] === lEnd || tile[1] === lEnd);
                                    const mRight = (tile[0] === rEnd || tile[1] === rEnd);
                                    if (mLeft && mRight) {
                                        selectedDominoIdx = idx;
                                        dominoEndChoice.style.display = 'flex';
                                        sound.click();
                                    } else if (mLeft) {
                                        sound.place(); haptic(20);
                                        ws.send(JSON.stringify({ cmd: 'play', index: idx, end: 'left' }));
                                        selectedDominoIdx = -1;
                                        dominoEndChoice.style.display = 'none';
                                    } else if (mRight) {
                                        sound.place(); haptic(20);
                                        ws.send(JSON.stringify({ cmd: 'play', index: idx, end: 'right' }));
                                        selectedDominoIdx = -1;
                                        dominoEndChoice.style.display = 'none';
                                    } else {
                                        sound.move();
                                    }
                                }
                            }
                        }
                    } else if (selectedDominoIdx >= 0 && y >= 50 && y < 300) {
                        sound.place(); haptic(20);
                        const bLen = (lastState.board && lastState.board.length) || 1;
                        const rRow = Math.floor((bLen - 1) / 7);
                        const rColInRow = (bLen - 1) % 7;
                        const rCol = (rRow % 2 === 0) ? rColInRow : (6 - rColInRow);
                        const x0 = 41; const y0 = 67;
                        const x1 = 22 + rCol * 46 + 19; const y1 = 56 + rRow * 38 + 11;
                        const d0 = (x - x0)*(x - x0) + (y - y0)*(y - y0);
                        const d1 = (x - x1)*(x - x1) + (y - y1)*(y - y1);
                        const end = (d0 <= d1) ? 'left' : 'right';
                        ws.send(JSON.stringify({ cmd: 'play', index: selectedDominoIdx, end: end }));
                        selectedDominoIdx = -1;
                        if (dominoEndChoice) dominoEndChoice.style.display = 'none';
                        if (dominoControls) dominoControls.style.display = 'flex';
                    }
                }
            }
        });

        let prevScore = 0;
        let prevFinished = false;

        function renderGameState(state) {
            cameraJuice.update();
            ctx.save();
            cameraJuice.apply(ctx);

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
            } else if (state.game === 'domino') {
                if (dominoControls && !state.finished) {
                    dominoControls.style.display = (selectedDominoIdx >= 0) ? 'none' : 'flex';
                }
                renderDominoGame(state);
            }

            particleEngine.updateAndRender();
            ctx.restore();

            // Audio cues & High score check
            if (state.game === 'snake' && !state.duel) {
                const currentHigh = parseInt(localStorage.getItem('gamebox_snake_high') || '0', 10);
                if ((state.score || 0) > currentHigh) {
                    localStorage.setItem('gamebox_snake_high', state.score);
                    hudP2Score.textContent = state.score;
                }
            }

            if (state.score > prevScore) {
                sound.eat();
                haptic(25);
            }
            prevScore = state.score || 0;

            if (state.finished && !prevFinished) {
                cameraJuice.shake(12);
                haptic(40);
                if (state.winner === myPlayerNum || (state.game === 'snake' && !state.duel)) {
                    sound.victory();
                    particleEngine.confetti(200, 200, 60);
                } else if (state.winner === -1) {
                    sound.bounce();
                } else {
                    sound.defeat();
                }

                // Win/Loss stats tracking
                try {
                    const stats = JSON.parse(localStorage.getItem('gamebox_stats') || '{}');
                    const gk = state.game;
                    if (!stats[gk]) stats[gk] = { w: 0, l: 0 };
                    if (state.winner === myPlayerNum || (state.game === 'snake' && !state.duel)) {
                        stats[gk].w++;
                    } else if (state.winner > 0) {
                        stats[gk].l++;
                    }
                    localStorage.setItem('gamebox_stats', JSON.stringify(stats));
                } catch(e) {}
            }
            prevFinished = state.finished;
        }
)rawliteral";

    static constexpr const char* INDEX_HTML_PART6B = R"rawliteral(
        // --- 1. Snake Renderer (Directional Robotic Eyes & Glowing Apple - Issue 07) ---
        function renderSnakeGame(state) {
            const grid = 20;
            const sz = 400 / grid;
            ctx.fillStyle = '#070b14';
            ctx.fillRect(0, 0, 400, 400);

            // Subtle grid lines
            ctx.strokeStyle = '#111827';
            ctx.lineWidth = 1;
            for (let i = 0; i <= grid; i++) {
                ctx.beginPath(); ctx.moveTo(i * sz, 0); ctx.lineTo(i * sz, 400); ctx.stroke();
                ctx.beginPath(); ctx.moveTo(0, i * sz); ctx.lineTo(400, i * sz); ctx.stroke();
            }

            // Food with pulse & bloom core
            if (state.food) {
                const fx = state.food[0] * sz + sz / 2;
                const fy = state.food[1] * sz + sz / 2;
                const pulse = 1 + Math.sin(Date.now() / 150) * 0.15;
                ctx.save();
                ctx.shadowBlur = 12; ctx.shadowColor = '#ef4444';
                ctx.fillStyle = '#ef4444';
                ctx.beginPath();
                ctx.arc(fx, fy, (sz / 2 - 2) * pulse, 0, Math.PI * 2);
                ctx.fill();
                ctx.fillStyle = '#fca5a5';
                ctx.beginPath();
                ctx.arc(fx - 1, fy - 1, (sz / 4) * pulse, 0, Math.PI * 2);
                ctx.fill();
                ctx.restore();
            }

            if (state.body && state.body.length > 0) renderSnakeBody(state.body, '#38bdf8', '#0284c7', sz);
            if (state.body2 && state.body2.length > 0) renderSnakeBody(state.body2, '#ec4899', '#be185d', sz);

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

                // Directional Robotic Eyes on Head
                if (idx === 0) {
                    const hx = rx + rsz / 2;
                    const hy = ry + rsz / 2;
                    let dx = 1, dy = 0;
                    if (body.length > 1) {
                        dx = seg[0] - body[1][0];
                        dy = seg[1] - body[1][1];
                    }
                    const eyeOff = 4;
                    const perpX = -dy * 3;
                    const perpY = dx * 3;
                    ctx.fillStyle = '#ffffff';
                    ctx.beginPath();
                    ctx.arc(hx + dx * eyeOff + perpX, hy + dy * eyeOff + perpY, 2, 0, Math.PI * 2);
                    ctx.arc(hx + dx * eyeOff - perpX, hy + dy * eyeOff - perpY, 2, 0, Math.PI * 2);
                    ctx.fill();
                }
            });
        }

        // --- 2. Tic-Tac-Toe Renderer (Laser Strike on Win - Issue 07) ---
        function renderTicTacToeGame(state) {
            const sz = 400 / 3;
            ctx.fillStyle = '#070b14';
            ctx.fillRect(0, 0, 400, 400);

            ctx.strokeStyle = '#1e293b';
            ctx.lineWidth = 3;
            for (let i = 1; i < 3; i++) {
                ctx.beginPath(); ctx.moveTo(i * sz, 24); ctx.lineTo(i * sz, 400 - 24); ctx.stroke();
                ctx.beginPath(); ctx.moveTo(24, i * sz); ctx.lineTo(400 - 24, i * sz); ctx.stroke();
            }

            if (state.board) {
                state.board.forEach((val, idx) => {
                    const row = Math.floor(idx / 3);
                    const col = idx % 3;
                    const cx = col * sz + sz / 2;
                    const cy = row * sz + sz / 2;
                    if (val === 1) {
                        ctx.save();
                        ctx.strokeStyle = '#38bdf8';
                        ctx.shadowBlur = 8; ctx.shadowColor = '#38bdf8';
                        ctx.lineWidth = 6;
                        ctx.beginPath();
                        ctx.moveTo(cx - 28, cy - 28); ctx.lineTo(cx + 28, cy + 28);
                        ctx.moveTo(cx + 28, cy - 28); ctx.lineTo(cx - 28, cy + 28);
                        ctx.stroke();
                        ctx.restore();
                    } else if (val === 2) {
                        ctx.save();
                        ctx.strokeStyle = '#ec4899';
                        ctx.shadowBlur = 8; ctx.shadowColor = '#ec4899';
                        ctx.lineWidth = 6;
                        ctx.beginPath();
                        ctx.arc(cx, cy, 30, 0, Math.PI * 2);
                        ctx.stroke();
                        ctx.restore();
                    }
                });

                // Laser Beam across Winning Mark (Issue 07)
                if (state.finished && state.winner > 0) {
                    const b = state.board;
                    const wins = [
                        [0,1,2],[3,4,5],[6,7,8], // rows
                        [0,3,6],[1,4,7],[2,5,8], // cols
                        [0,4,8],[2,4,6]          // diags
                    ];
                    for (const [p0, p1, p2] of wins) {
                        if (b[p0] === state.winner && b[p1] === state.winner && b[p2] === state.winner) {
                            const x0 = (p0 % 3) * sz + sz / 2; const y0 = Math.floor(p0 / 3) * sz + sz / 2;
                            const x2 = (p2 % 3) * sz + sz / 2; const y2 = Math.floor(p2 / 3) * sz + sz / 2;
                            ctx.save();
                            ctx.strokeStyle = state.winner === 1 ? '#38bdf8' : '#ec4899';
                            ctx.shadowBlur = 18; ctx.shadowColor = ctx.strokeStyle;
                            ctx.lineWidth = 8;
                            ctx.beginPath(); ctx.moveTo(x0, y0); ctx.lineTo(x2, y2); ctx.stroke();
                            ctx.strokeStyle = '#ffffff'; ctx.lineWidth = 3;
                            ctx.beginPath(); ctx.moveTo(x0, y0); ctx.lineTo(x2, y2); ctx.stroke();
                            ctx.restore();
                            break;
                        }
                    }
                }
            }

            if (!state.finished) {
                hudStatus.textContent = state.turn === myPlayerNum ? "Your Turn" : "Opponent Turn";
            } else {
                showModal(state);
            }
        }
)rawliteral";

    static constexpr const char* INDEX_HTML_PART7 = R"rawliteral(
        // --- 3. Connect Four Renderer (Elastic Chip Bounce Animation - Issue 07) ---
        let c4DroppingChips = [];
        let c4PrevBoard = null;

        function renderConnectFourGame(state) {
            const cols = 7;
            const rows = 6;
            const cw = 400 / cols;
            const ch = 400 / rows;

            ctx.fillStyle = '#0b1329';
            ctx.fillRect(0, 0, 400, 400);

            // Detect new falling chips
            if (state.board && c4PrevBoard && c4PrevBoard.length === state.board.length) {
                for (let r = 0; r < rows; r++) {
                    for (let c = 0; c < cols; c++) {
                        const idx = r * cols + c;
                        if (c4PrevBoard[idx] === 0 && state.board[idx] !== 0) {
                            c4DroppingChips.push({
                                col: c, targetY: r * ch + ch / 2, currentY: -ch,
                                vy: 0, gravity: 1.8, bounce: 0.35, val: state.board[idx],
                                targetRow: r
                            });
                        }
                    }
                }
            }
            c4PrevBoard = state.board ? [...state.board] : null;

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

                        const isAnimating = c4DroppingChips.some(d => d.col === c && d.targetRow === r);
                        if (!isAnimating && val > 0) {
                            ctx.fillStyle = val === 1 ? '#38bdf8' : '#ec4899';
                            ctx.beginPath();
                            ctx.arc(cx, cy, radius - 2, 0, Math.PI * 2);
                            ctx.fill();
                        }
                    }
                }
            }

            // Animate Dropping Chips
            for (let i = c4DroppingChips.length - 1; i >= 0; i--) {
                const d = c4DroppingChips[i];
                d.vy += d.gravity;
                d.currentY += d.vy;
                if (d.currentY >= d.targetY) {
                    d.currentY = d.targetY;
                    d.vy = -d.vy * d.bounce;
                    if (Math.abs(d.vy) < 1.2) {
                        c4DroppingChips.splice(i, 1);
                        cameraJuice.shake(2);
                        continue;
                    }
                }
                const cx = d.col * cw + cw / 2;
                const radius = Math.min(cw, ch) * 0.38;
                ctx.fillStyle = d.val === 1 ? '#38bdf8' : '#ec4899';
                ctx.beginPath();
                ctx.arc(cx, d.currentY, radius - 2, 0, Math.PI * 2);
                ctx.fill();
            }

            if (!state.finished) {
                hudStatus.textContent = state.turn === myPlayerNum ? "Your Turn (Tap Column)" : "Opponent Turn";
            } else {
                showModal(state);
            }
        }

        // --- 4. Pong Renderer (Phosphor Comet Motion Trail & Sparks - Issue 07) ---
        let pongTrails = [];

        function renderPongGame(state) {
            ctx.fillStyle = '#070b14';
            ctx.fillRect(0, 0, 400, 400);

            // Center dashed court line
            ctx.strokeStyle = '#1e293b';
            ctx.lineWidth = 2;
            ctx.setLineDash([6, 6]);
            ctx.beginPath(); ctx.moveTo(200, 0); ctx.lineTo(200, 400); ctx.stroke();
            ctx.setLineDash([]);

            // Paddles
            const padW = 10;
            const padH = 60;
            ctx.fillStyle = '#38bdf8';
            ctx.fillRect(padW, (state.p1y || 200) - padH / 2, padW, padH);

            ctx.fillStyle = '#ec4899';
            ctx.fillRect(400 - padW * 2, (state.p2y || 200) - padH / 2, padW, padH);

            // Ball Phosphor Comet Trails (Issue 07)
            const bx = state.bx || 200;
            const by = state.by || 200;
            pongTrails.push({ x: bx, y: by, alpha: 0.8 });
            if (pongTrails.length > 7) pongTrails.shift();

            for (let i = 0; i < pongTrails.length - 1; i++) {
                const t = pongTrails[i];
                t.alpha *= 0.82;
                ctx.fillStyle = `rgba(56, 189, 248, ${t.alpha * 0.4})`;
                ctx.beginPath();
                ctx.arc(t.x, t.y, 6 * t.alpha, 0, Math.PI * 2);
                ctx.fill();
            }

            // Ball Core
            ctx.save();
            ctx.fillStyle = '#f1f5f9';
            ctx.shadowBlur = 10; ctx.shadowColor = '#38bdf8';
            ctx.beginPath();
            ctx.arc(bx, by, 8, 0, Math.PI * 2);
            ctx.fill();
            ctx.restore();

            hudP1Score.textContent = state.score1 || 0;
            hudP2Score.textContent = state.score2 || 0;
            hudStatus.textContent = "First to 5 Wins";

            if (state.finished) showModal(state);
        }

        // --- 5. Tron Light Cycles Renderer (Neon Bloom & Shockwaves - Issue 07) ---
        function renderTronGame(state) {
            const grid = 30;
            const sz = 400 / grid;

            ctx.fillStyle = '#070b14';
            ctx.fillRect(0, 0, 400, 400);

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

            // Draw player heads with brighter tint and headlights
            if (state.p1 && state.p1.alive) {
                ctx.save();
                ctx.shadowBlur = 10; ctx.shadowColor = '#38bdf8';
                ctx.fillStyle = '#38bdf8';
                ctx.fillRect(state.p1.x * sz, state.p1.y * sz, sz, sz);
                ctx.restore();
            }
            if (state.p2 && state.p2.alive) {
                ctx.save();
                ctx.shadowBlur = 10; ctx.shadowColor = '#ec4899';
                ctx.fillStyle = '#ec4899';
                ctx.fillRect(state.p2.x * sz, state.p2.y * sz, sz, sz);
                ctx.restore();
            }

            hudStatus.textContent = "Avoid Wall Collision";
            if (state.finished) showModal(state);
        }
)rawliteral";

    static constexpr const char* INDEX_HTML_PART8 = R"rawliteral(
        // --- 6. Battleship Renderer (360 Sonar Radar Sweep & Ripples - Issue 07) ---
        function renderBattleshipGame(state) {
            const sz = 40;
            ctx.fillStyle = '#070b14';
            ctx.fillRect(0, 0, 400, 400);

            // Sonar grid lines
            ctx.strokeStyle = '#111e33';
            ctx.lineWidth = 1;
            for (let i = 0; i <= 10; i++) {
                ctx.beginPath(); ctx.moveTo(i * sz, 0); ctx.lineTo(i * sz, 400); ctx.stroke();
                ctx.beginPath(); ctx.moveTo(0, i * sz); ctx.lineTo(400, i * sz); ctx.stroke();
            }

            // Continuous 360 Sonar Radar Sweep Line (Issue 07)
            const sweepAngle = (Date.now() / 1400) % (Math.PI * 2);
            ctx.save();
            ctx.strokeStyle = 'rgba(56, 189, 248, 0.35)';
            ctx.lineWidth = 1.8;
            ctx.beginPath();
            ctx.moveTo(200, 200);
            ctx.lineTo(200 + Math.cos(sweepAngle) * 280, 200 + Math.sin(sweepAngle) * 280);
            ctx.stroke();
            ctx.restore();

            const me = myPlayerNum === 1 ? state.p1 : state.p2;

            if (state.phase === 'placement') {
                battleshipControls.style.display = 'flex';
                hudStatus.textContent = (myPlayerNum === 1 ? state.p1ready : state.p2ready) ?
                    "Fleet Deployed! Waiting for opponent..." : "Tap 'Deploy Fleet' to Arrange";

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

        // --- 7. Domino Renderer & Helpers (Emerald Felt Table & 3D Ivory Tiles - Issue 07) ---
        function drawDominoPips(c, val, x, y, w, h) {
            if (val <= 0) return;
            const r = Math.max(1.6, Math.min(2.8, w * 0.08));
            c.fillStyle = '#0f172a';

            const c1 = x + w * 0.26;
            const c2 = x + w * 0.5;
            const c3 = x + w * 0.74;

            const r1 = y + h * 0.26;
            const r2 = y + h * 0.5;
            const r3 = y + h * 0.74;

            const dot = (px, py) => {
                c.beginPath();
                c.arc(px, py, r, 0, Math.PI * 2);
                c.fill();
            };

            if (val === 1) { dot(c2, r2); }
            else if (val === 2) { dot(c1, r1); dot(c3, r3); }
            else if (val === 3) { dot(c1, r1); dot(c2, r2); dot(c3, r3); }
            else if (val === 4) { dot(c1, r1); dot(c3, r1); dot(c1, r3); dot(c3, r3); }
            else if (val === 5) { dot(c1, r1); dot(c3, r1); dot(c2, r2); dot(c1, r3); dot(c3, r3); }
            else if (val === 6) { dot(c1, r1); dot(c3, r1); dot(c1, r2); dot(c3, r2); dot(c1, r3); dot(c3, r3); }
        }

        function renderDominoGame(state) {
            // Havana Emerald Felt Gaming Table Radial Gradient (Issue 07)
            const grad = ctx.createRadialGradient(200, 200, 20, 200, 200, 260);
            grad.addColorStop(0, '#0f382a');
            grad.addColorStop(1, '#051811');
            ctx.fillStyle = grad;
            ctx.fillRect(0, 0, 400, 400);

            // Subtle table border
            ctx.strokeStyle = '#143c2e';
            ctx.lineWidth = 2;
            ctx.strokeRect(6, 6, 388, 388);

            // --- Top Bar: Opponents Hand Info ---
            ctx.fillStyle = '#94a3b8';
            ctx.font = '600 11px system-ui, sans-serif';
            ctx.textAlign = 'left';

            let oppStr = "";
            if (activeGameType === 'domino_solo') {
                oppStr = `Bot: ${state.p2_count || 0} tiles`;
            } else if (state.num_players === 3) {
                oppStr = `P1: ${state.p1_count} · P2: ${state.p2_count} · P3: ${state.p3_count}`;
            } else {
                oppStr = (myPlayerNum === 1) ? `Opponent: ${state.p2_count || 0} tiles` : `Opponent: ${state.p1_count || 0} tiles`;
            }
            ctx.fillText(oppStr, 14, 24);

            // Reserve / Boneyard badge
            ctx.textAlign = 'right';
            ctx.fillStyle = '#38bdf8';
            const boneyardLabel = (state.block_mode !== false) ? `Reserve: ${state.boneyard_count || 0}` : `Pioche: ${state.boneyard_count || 0}`;
            ctx.fillText(boneyardLabel, 386, 24);

            // --- Open End Indicators in Middle ---
            ctx.font = '700 11px system-ui, sans-serif';
            ctx.textAlign = 'left';
            if (state.left_end !== -1) {
                ctx.fillStyle = '#38bdf8';
                ctx.fillText(`◄ Left: ${state.left_end}`, 14, 42);
            }
            ctx.textAlign = 'right';
            if (state.right_end !== -1) {
                ctx.fillStyle = '#fbbf24';
                ctx.fillText(`Right: ${state.right_end} ►`, 386, 42);
            }

            if (state.last_action) {
                ctx.textAlign = 'center';
                ctx.fillStyle = '#94a3b8';
                ctx.font = '600 10px system-ui, sans-serif';
                ctx.fillText(state.last_action, 200, 42);
            }

            // --- Middle Board Domino Chain with 3D Ivory Tiles ---
            const board = state.board || [];
            if (board.length === 0) {
                ctx.textAlign = 'center';
                ctx.fillStyle = '#64748b';
                ctx.font = '500 13px system-ui, sans-serif';
                ctx.fillText("Board is open. Tap any tile in your hand to lead!", 200, 160);
            } else {
                const maxPerRow = 7;
                const bTileW = 38;
                const bTileH = 22;

                board.forEach((t, i) => {
                    const row = Math.floor(i / maxPerRow);
                    const colInRow = i % maxPerRow;
                    const isEvenRow = (row % 2 === 0);
                    const col = isEvenRow ? colInRow : (maxPerRow - 1 - colInRow);

                    const bx = 22 + col * (bTileW + 8);
                    const by = 56 + row * (bTileH + 16);

                    // Visual corner track connector
                    if (row > 0 && colInRow === 0) {
                        ctx.strokeStyle = '#1e3a2f';
                        ctx.lineWidth = 2.5;
                        ctx.beginPath();
                        const prevCenterY = by - 16 - bTileH / 2;
                        const currCenterY = by + bTileH / 2;
                        const midY = (prevCenterY + currCenterY) / 2;
                        const radius = (currCenterY - prevCenterY) / 2;
                        if (isEvenRow) {
                            ctx.arc(bx + 2, midY, radius, Math.PI / 2, 3 * Math.PI / 2, false);
                        } else {
                            ctx.arc(bx + bTileW - 2, midY, radius, -Math.PI / 2, Math.PI / 2, false);
                        }
                        ctx.stroke();
                    }

                    // 3D Ivory Tile Body (Soft Drop Shadow - Issue 07)
                    ctx.save();
                    ctx.shadowColor = 'rgba(0, 0, 0, 0.45)';
                    ctx.shadowBlur = 5; ctx.shadowOffsetY = 2;
                    ctx.fillStyle = '#fffbeb';
                    ctx.beginPath();
                    ctx.roundRect ? ctx.roundRect(bx, by, bTileW, bTileH, 4) : ctx.rect(bx, by, bTileW, bTileH);
                    ctx.fill();
                    ctx.restore();

                    ctx.strokeStyle = '#334155';
                    ctx.lineWidth = 1.2;
                    ctx.stroke();

                    // Center divider
                    ctx.strokeStyle = '#475569';
                    ctx.beginPath();
                    ctx.moveTo(bx + bTileW / 2, by + 2);
                    ctx.lineTo(bx + bTileW / 2, by + bTileH - 2);
                    ctx.stroke();

                    // Pips
                    const pipsLeft = isEvenRow ? t[0] : t[1];
                    const pipsRight = isEvenRow ? t[1] : t[0];
                    drawDominoPips(ctx, pipsLeft, bx, by, bTileW / 2, bTileH);
                    drawDominoPips(ctx, pipsRight, bx + bTileW / 2, by, bTileW / 2, bTileH);

                    // Pulsing Open End Halo (Issue 07)
                    const haloPulse = 0.5 + 0.5 * Math.sin(Date.now() / 200);
                    if (i === 0) {
                        ctx.strokeStyle = `rgba(56, 189, 248, ${0.6 + haloPulse * 0.4})`;
                        ctx.lineWidth = 2.5;
                        ctx.strokeRect(bx - 2, by - 2, bTileW + 4, bTileH + 4);
                    } else if (i === board.length - 1) {
                        ctx.strokeStyle = `rgba(251, 191, 36, ${0.6 + haloPulse * 0.4})`;
                        ctx.lineWidth = 2.5;
                        ctx.strokeRect(bx - 2, by - 2, bTileW + 4, bTileH + 4);
                    }
                });
            }

            // Separator line
            ctx.strokeStyle = '#143c2e';
            ctx.lineWidth = 1;
            ctx.beginPath();
            ctx.moveTo(10, 290);
            ctx.lineTo(390, 290);
            ctx.stroke();
)rawliteral";

    static constexpr const char* INDEX_HTML_PART9 = R"rawliteral(
            // --- Bottom Area: Player's Hand ---
            const myHand = (state.hands && state.hands[myPlayerNum]) || [];
            const count = myHand.length;

            ctx.textAlign = 'left';
            ctx.fillStyle = '#94a3b8';
            ctx.font = '600 11px system-ui, sans-serif';
            ctx.fillText(`Your Hand (${count} domino${count > 1 ? 'es' : ''}):`, 14, 306);

            if (count > 0) {
                const tileW = Math.min(42, Math.max(20, Math.floor(376 / count)));
                const tileH = 68;
                const totalW = count * tileW;
                const startX = Math.max(12, Math.floor((400 - totalW) / 2));
                const isMyTurn = (state.turn === myPlayerNum && !state.finished);

                myHand.forEach((t, i) => {
                    const tx = startX + i * tileW;
                    let ty = 314;
                    const isSelected = (selectedDominoIdx === i);
                    if (isSelected) ty -= 8;

                    const canPlay = (state.left_end === -1) ||
                                    (t[0] === state.left_end || t[1] === state.left_end ||
                                     t[0] === state.right_end || t[1] === state.right_end);

                    // 3D Ivory Body with Drop Shadow
                    ctx.save();
                    ctx.shadowColor = 'rgba(0, 0, 0, 0.45)';
                    ctx.shadowBlur = 6; ctx.shadowOffsetY = 3;
                    ctx.fillStyle = isSelected ? '#ffffff' : '#fffbeb';
                    ctx.beginPath();
                    ctx.roundRect ? ctx.roundRect(tx + 2, ty, tileW - 4, tileH, 5) : ctx.rect(tx + 2, ty, tileW - 4, tileH);
                    ctx.fill();
                    ctx.restore();

                    // Highlight border
                    if (isSelected) {
                        ctx.strokeStyle = '#fbbf24';
                        ctx.lineWidth = 2.5;
                    } else if (isMyTurn && canPlay) {
                        ctx.strokeStyle = '#38bdf8';
                        ctx.lineWidth = 2;
                    } else {
                        ctx.strokeStyle = '#334155';
                        ctx.lineWidth = 1.2;
                    }
                    ctx.stroke();

                    // Divider line
                    ctx.strokeStyle = '#475569';
                    ctx.lineWidth = 1;
                    ctx.beginPath();
                    ctx.moveTo(tx + 4, ty + tileH / 2);
                    ctx.lineTo(tx + tileW - 4, ty + tileH / 2);
                    ctx.stroke();

                    // Pips
                    drawDominoPips(ctx, t[0], tx + 2, ty, tileW - 4, tileH / 2);
                    drawDominoPips(ctx, t[1], tx + 2, ty + tileH / 2, tileW - 4, tileH / 2);
                });
            }

            // HUD scores and status updates
            hudP1Score.textContent = `${state.p1_count || 0} tiles`;
            hudP2Score.textContent = `${state.p2_count || 0} tiles`;
            if (state.num_players === 3) {
                hudP3Wrap.style.display = 'flex';
                hudP3Score.textContent = `${state.p3_count || 0} tiles`;
            } else {
                hudP3Wrap.style.display = 'none';
            }

            const isMyTurn = (state.turn === myPlayerNum && !state.finished);
            const hasPlayableTile = myHand.some(t =>
                (state.left_end === -1) ||
                (t[0] === state.left_end || t[1] === state.left_end ||
                 t[0] === state.right_end || t[1] === state.right_end)
            );

            if (state.finished) {
                if (dominoControls) dominoControls.style.display = 'none';
                if (dominoEndChoice) dominoEndChoice.style.display = 'none';
                hudStatus.textContent = "Game Over";
                showModal(state);
            } else {
                if (dominoControls) dominoControls.style.display = (selectedDominoIdx >= 0) ? 'none' : 'flex';
                if (dominoEndChoice) dominoEndChoice.style.display = (selectedDominoIdx >= 0) ? 'flex' : 'none';

                if (btnPlaceLeft && state.left_end !== -1) {
                    btnPlaceLeft.textContent = `Place Left (${state.left_end})`;
                }
                if (btnPlaceRight && state.right_end !== -1) {
                    btnPlaceRight.textContent = `Place Right (${state.right_end})`;
                }

                const actionPrefix = state.last_action ? `${state.last_action} · ` : "";
                if (isMyTurn) {
                    if (hasPlayableTile) {
                        hudStatus.textContent = (selectedDominoIdx >= 0) ? "Select End (Left / Right)" : (actionPrefix + "Your Turn");
                    } else {
                        if (state.block_mode !== false) {
                            hudStatus.textContent = actionPrefix + "No valid moves! Tap Pass Turn";
                        } else if ((state.boneyard_count || 0) > 0) {
                            hudStatus.textContent = actionPrefix + "No valid move! Draw or Pass";
                        } else {
                            hudStatus.textContent = actionPrefix + "Blocked! Passing turn...";
                        }
                    }
                } else {
                    if (activeGameType === 'domino_solo') {
                        hudStatus.textContent = state.last_action ? `${state.last_action} · Bot Turn` : "Bot Thinking...";
                    } else {
                        hudStatus.textContent = state.last_action ? `${state.last_action} · P${state.turn}'s Turn` : `P${state.turn}'s Turn`;
                    }
                }
            }

            if (boneyardCountSpan) {
                boneyardCountSpan.textContent = state.boneyard_count || 0;
            }

            if (btnDominoMode) {
                btnDominoMode.textContent = (state.block_mode !== false) ? "Rule: Block" : "Rule: Draw";
            }

            if (btnDominoDraw) {
                if (state.block_mode !== false) {
                    btnDominoDraw.style.display = 'none';
                } else {
                    btnDominoDraw.style.display = 'inline-block';
                    const canDraw = isMyTurn && ((state.boneyard_count || 0) > 0);
                    btnDominoDraw.disabled = !canDraw;
                    btnDominoDraw.style.opacity = canDraw ? '1.0' : '0.4';
                }
            }

            if (btnDominoPass) {
                const canPass = isMyTurn && !hasPlayableTile;
                btnDominoPass.disabled = !canPass;
                btnDominoPass.style.opacity = canPass ? '1.0' : '0.4';
            }

            // Auto-pass if blocked or in Block mode without moves
            const shouldAutoPass = isMyTurn && !hasPlayableTile && (state.block_mode !== false || (state.boneyard_count || 0) === 0);
            if (shouldAutoPass) {
                if (!window._dominoAutoPassTimer) {
                    window._dominoAutoPassTimer = setTimeout(() => {
                        window._dominoAutoPassTimer = null;
                        if (ws && ws.readyState === WebSocket.OPEN && lastState && lastState.turn === myPlayerNum && !lastState.finished) {
                            sound.move();
                            ws.send(JSON.stringify({ cmd: 'pass' }));
                        }
                    }, 1000);
                }
            } else {
                if (window._dominoAutoPassTimer) {
                    clearTimeout(window._dominoAutoPassTimer);
                    window._dominoAutoPassTimer = null;
                }
            }
        }

        function spawnVictoryCelebration() {
            particleEngine.confetti(200, 200, 65);
        }

        // --- Modal Dialog with Rematch & Series Score (ADR 0013 / Issue 02) ---
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
            } else if (state.game === 'domino') {
                if (isWin) {
                    sub = (state.p1_count === 0 || state.p2_count === 0 || state.p3_count === 0) ?
                          "Dominó! All tiles placed!" : "Lowest pip sum in blocked game!";
                } else if (isDraw) {
                    sub = "Deadlock on board! Tied on pip sum.";
                } else {
                    sub = state.winner > 0 ? `Player ${state.winner} won the domino match.` : "Opponent won the match.";
                }
            }

            if (state.series_p1 !== undefined) {
                sub += ` · Series: P1 ${state.series_p1} - ${state.series_p2} P2 (Match ${state.series_match || 1})`;
            }

            if (activeGameType && activeGameType.indexOf('_solo') === -1 && activeRoomCode) {
                btnModalReplay.textContent = "Rematch";
            } else {
                btnModalReplay.textContent = "Play Again";
            }

            modalTitle.textContent = title;
            modalTitle.style.color = isWin ? '#4ade80' : (isDraw ? '#f59e0b' : '#f87171');
            modalSub.textContent = sub;

            if (isWin) {
                const winKey = `gamebox_wins_${state.game}`;
                const curWins = parseInt(localStorage.getItem(winKey) || '0', 10);
                localStorage.setItem(winKey, curWins + 1);
            }

            // Render Pure SVG Icon
            if (isWin) {
                spawnVictoryCelebration();
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
)rawliteral";

    static constexpr const char* INDEX_HTML_PART10 = R"rawliteral(
        // --- Hall of Fame Leaderboard & Bottom-Sheet Drawer (Issue 08) ---
        const btnTrophy = document.getElementById('btn-trophy');
        const statsDrawer = document.getElementById('stats-drawer');
        const btnCloseStats = document.getElementById('btn-close-stats');
        const btnClearStats = document.getElementById('btn-clear-stats');
        const statsContent = document.getElementById('stats-content');
        const btnCrt = document.getElementById('btn-crt');

        // CRT Filter Toggle
        if (localStorage.getItem('gamebox_crt') === 'true') {
            document.body.classList.add('crt-active');
        }
        if (btnCrt) {
            btnCrt.addEventListener('click', () => {
                sound.click();
                const active = document.body.classList.toggle('crt-active');
                localStorage.setItem('gamebox_crt', active);
            });
        }

        function getTierBadge(wins, isSnake = false) {
            if (isSnake) {
                if (wins >= 50) return '<span class="tier-badge tier-master">Master</span>';
                if (wins >= 25) return '<span class="tier-badge tier-gold">Gold</span>';
                if (wins >= 10) return '<span class="tier-badge tier-silver">Silver</span>';
                return '<span class="tier-badge tier-bronze">Bronze</span>';
            }
            if (wins >= 15) return '<span class="tier-badge tier-master">Master</span>';
            if (wins >= 7) return '<span class="tier-badge tier-gold">Gold</span>';
            if (wins >= 3) return '<span class="tier-badge tier-silver">Silver</span>';
            return '<span class="tier-badge tier-bronze">Bronze</span>';
        }

        function updateStatsModal() {
            if (!statsContent) return;
            const games = [
                { id: 'snake', name: 'Snake Survival', highKey: 'gamebox_snake_high', highLabel: 'Best Score' },
                { id: 'tictactoe', name: 'Tic-Tac-Toe', winKey: 'gamebox_wins_tictactoe' },
                { id: 'connect4', name: 'Connect Four', winKey: 'gamebox_wins_connect4' },
                { id: 'pong', name: 'Pong Arcade', winKey: 'gamebox_wins_pong' },
                { id: 'tron', name: 'Tron Light Cycles', winKey: 'gamebox_wins_tron' },
                { id: 'battleship', name: 'Battleship Fleet', winKey: 'gamebox_wins_battleship' },
                { id: 'domino', name: 'Block Dominoes', winKey: 'gamebox_wins_domino' }
            ];

            let statsObj = {};
            try { statsObj = JSON.parse(localStorage.getItem('gamebox_stats') || '{}'); } catch(e) {}

            let html = '';
            games.forEach(g => {
                let statText = '';
                let tierHtml = '';
                if (g.highKey) {
                    const val = parseInt(localStorage.getItem(g.highKey) || '0', 10);
                    tierHtml = getTierBadge(val, true);
                    statText = `${g.highLabel}: <b style="color:var(--p1-cyan);">${val}</b>`;
                } else if (g.winKey) {
                    const wins = parseInt(localStorage.getItem(g.winKey) || '0', 10);
                    tierHtml = getTierBadge(wins, false);
                    statText = `Wins: <b style="color:var(--success);">${wins}</b>`;
                }
                html += `
                    <div style="display:flex; justify-content:space-between; align-items:center; padding:10px 12px; background:var(--bg); border:1px solid var(--border); border-radius:var(--radius-sm);">
                        <div style="display:flex; align-items:center; gap:8px;">
                            ${tierHtml}
                            <span style="font-weight:600; color:var(--text);">${g.name}</span>
                        </div>
                        <span>${statText}</span>
                    </div>
                `;
            });
            statsContent.innerHTML = html;
        }

        if (btnTrophy) {
            btnTrophy.addEventListener('click', () => {
                sound.click();
                updateStatsModal();
                statsDrawer.classList.add('active');
            });
        }
        if (btnCloseStats) {
            btnCloseStats.addEventListener('click', () => {
                sound.click();
                statsDrawer.classList.remove('active');
            });
        }
        if (statsDrawer) {
            statsDrawer.addEventListener('click', (e) => {
                if (e.target === statsDrawer) statsDrawer.classList.remove('active');
            });
        }

        // Two-Step Reset Confirmation (Issue 08)
        let resetConfirming = false;
        let resetTimer = null;
        if (btnClearStats) {
            btnClearStats.addEventListener('click', () => {
                sound.click();
                if (!resetConfirming) {
                    resetConfirming = true;
                    btnClearStats.textContent = "Confirm Reset Records?";
                    btnClearStats.style.background = "var(--danger)";
                    btnClearStats.style.color = "#ffffff";
                    resetTimer = setTimeout(() => {
                        resetConfirming = false;
                        btnClearStats.textContent = "Reset Records";
                        btnClearStats.style.background = "";
                        btnClearStats.style.color = "";
                    }, 3500);
                } else {
                    clearTimeout(resetTimer);
                    resetConfirming = false;
                    btnClearStats.textContent = "Reset Records";
                    btnClearStats.style.background = "";
                    btnClearStats.style.color = "";
                    ['gamebox_snake_high', 'gamebox_wins_tictactoe', 'gamebox_wins_connect4', 'gamebox_wins_pong', 'gamebox_wins_tron', 'gamebox_wins_battleship', 'gamebox_wins_domino', 'gamebox_stats'].forEach(k => localStorage.removeItem(k));
                    updateStatsModal();
                }
            });
        }

        // Startup Connection
        window.addEventListener('load', () => {
            connect();
        });
    </script>
</body>
</html>
)rawliteral";

};

} // namespace web
} // namespace gamehub
