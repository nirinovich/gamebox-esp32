import { describe, it, expect, beforeEach } from 'vitest';
import {
  setPlayerName,
  playerName,
  toggleSound,
  soundMuted,
  toggleCrt,
  crtFilter,
  selectedTab,
  filteredGames,
  gameCatalogue,
  resetRoomState,
  activeRoom,
  gameState,
  currentView
} from '../state/gameStore';

describe('GameStore and Reactive State Transitions', () => {
  beforeEach(() => {
    // Mock localStorage in test environment
    const store: Record<string, string> = {};
    (globalThis as any).window = {
      localStorage: {
        getItem: (k: string) => store[k] || null,
        setItem: (k: string, v: string) => { store[k] = v; },
        clear: () => { for (const k in store) delete store[k]; }
      }
    };
    resetRoomState();
  });

  it('updates and persists player nickname', () => {
    setPlayerName('CyberPilot');
    expect(playerName.value).toBe('CyberPilot');
    expect(window.localStorage.getItem('gamebox_name')).toBe('CyberPilot');
  });

  it('clamps long nicknames to 14 characters', () => {
    setPlayerName('VeryLongNicknameBeyondLimits');
    expect(playerName.value.length).toBeLessThanOrEqual(14);
  });

  it('toggles sound mute preference and persists state', () => {
    const initial = soundMuted.value;
    toggleSound();
    expect(soundMuted.value).toBe(!initial);
    expect(window.localStorage.getItem('gamebox_muted')).toBe(String(!initial));
  });

  it('toggles CRT retro scanline preference', () => {
    const initial = crtFilter.value;
    toggleCrt();
    expect(crtFilter.value).toBe(!initial);
    expect(window.localStorage.getItem('gamebox_crt')).toBe(String(!initial));
  });

  it('filters game directory correctly based on category tab', () => {
    gameCatalogue.value = [
      { key: 's1', title: 'S1', category: 'SOLO', tag: 'Solo', icon: 's1', isDuel: false, inputs: [] },
      { key: 's2', title: 'S2', category: 'SOLO', tag: 'Solo', icon: 's2', isDuel: false, inputs: [] },
      { key: 'd1', title: 'D1', category: 'DUEL', tag: 'Duel', icon: 'd1', isDuel: true, inputs: [] },
      { key: 'p1', title: 'P1', category: 'PARTY', tag: 'Party', icon: 'p1', isDuel: false, inputs: [] },
    ];

    selectedTab.value = 'ALL';
    expect(filteredGames.value.length).toBe(4);

    selectedTab.value = 'SOLO';
    expect(filteredGames.value.every(g => g.category === 'SOLO')).toBe(true);
    expect(filteredGames.value.length).toBe(2);

    selectedTab.value = 'DUEL';
    expect(filteredGames.value.every(g => g.category === 'DUEL')).toBe(true);
    expect(filteredGames.value.length).toBe(1);

    selectedTab.value = 'PARTY';
    expect(filteredGames.value.every(g => g.category === 'PARTY')).toBe(true);
    expect(filteredGames.value.length).toBe(1);
  });

  it('resets room state cleanly back to LOBBY and clears gameState', () => {
    activeRoom.value = {
      code: 'ABCD',
      isHost: true,
      gameType: 'domino_pvp',
      playerNum: 1,
      opponentName: 'Challenger'
    };
    gameState.value = { finished: true, winner: 1 };
    currentView.value = 'GAME';

    resetRoomState();
    expect(activeRoom.value).toBeNull();
    expect(gameState.value).toBeNull();
    expect(currentView.value).toBe('LOBBY');
  });

  it('updates waiting room player count and rematch counts', () => {
    activeRoom.value = {
      code: 'WXYZ',
      isHost: true,
      gameType: 'domino_pvp',
      playerNum: 1,
      opponentName: 'Waiting...',
      waitingCount: 1,
      waitingMax: 3
    };

    expect(activeRoom.value.waitingCount).toBe(1);
    expect(activeRoom.value.waitingMax).toBe(3);

    // Simulate rematch update
    activeRoom.value = {
      ...activeRoom.value,
      rematchCount: 2,
      rematchNeeded: 3
    };

    expect(activeRoom.value.rematchCount).toBe(2);
    expect(activeRoom.value.rematchNeeded).toBe(3);
  });
});
