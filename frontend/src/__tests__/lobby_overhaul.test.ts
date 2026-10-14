import { describe, it, expect, beforeEach } from 'vitest';
import { h } from 'preact';
import {
  CALLSIGNS,
  getAvailableCallsign,
  getTakenNames,
  playerName,
  setPlayerName,
  shufflePlayerName,
  lastPlayedGame,
  recordPlayedGame,
  activeRoom,
  roomDirectory,
  gameCatalogue,
  selectedTab,
  resetRoomState
} from '../state/gameStore';
import { Lobby } from '../components/Lobby/Lobby';
import { Header } from '../components/Header/Header';

describe('Arcade Lobby Overhaul & Callsign System', () => {
  let mockStore: Record<string, string>;

  beforeEach(() => {
    mockStore = {};
    (globalThis as any).window = {
      localStorage: {
        getItem: (k: string) => mockStore[k] || null,
        setItem: (k: string, v: string) => { mockStore[k] = v; },
        clear: () => { mockStore = {}; }
      }
    };
    resetRoomState();
    selectedTab.value = 'SOLO';
    roomDirectory.value = [];
    gameCatalogue.value = [
      {
        key: 'domino_solo',
        title: 'Dominoes Solo',
        category: 'SOLO',
        description: 'Tactical draw dominoes vs AI engine',
        tag: 'Solo AI',
        icon: 'domino_solo',
        isDuel: false
      },
      {
        key: 'katro_pvp',
        title: 'Katro Duel',
        category: 'DUEL',
        description: 'Traditional Malagasy pit and seed strategy',
        tag: '2P Duel',
        icon: 'katro_pvp',
        isDuel: true
      }
    ];
  });

  describe('Callsign Pool & Untaken Resolution', () => {
    it('provides exactly 18 curated retro arcade callsigns without digits', () => {
      expect(CALLSIGNS.length).toBe(18);
      for (const cs of CALLSIGNS) {
        expect(typeof cs).toBe('string');
        expect(cs.length).toBeGreaterThanOrEqual(3);
        // Ensure no numeric digits in the callsigns
        expect(/\d/.test(cs)).toBe(false);
      }
    });

    it('returns an untaken callsign and excludes current handle', () => {
      const initial = 'Viper';
      const available = getAvailableCallsign(initial);
      expect(CALLSIGNS).toContain(available);
      expect(available.toLowerCase()).not.toBe(initial.toLowerCase());
    });

    it('filters out taken names from active rooms and room directory', () => {
      roomDirectory.value = [
        { code: 'ABCD', host: 'Viper', game: 'katro_pvp', players: 1, max: 2 },
        { code: 'WXYZ', host: 'Rocket', game: 'katro_pvp', players: 1, max: 2 }
      ];
      activeRoom.value = {
        code: 'TEST',
        isHost: true,
        gameType: 'katro_pvp',
        playerNum: 1,
        opponentName: 'Pilot'
      };

      const taken = getTakenNames();
      expect(taken.has('viper')).toBe(true);
      expect(taken.has('rocket')).toBe(true);
      expect(taken.has('pilot')).toBe(true);

      // Generating available callsign must not pick Viper, Rocket, or Pilot
      for (let i = 0; i < 20; ++i) {
        const cs = getAvailableCallsign();
        expect(['viper', 'rocket', 'pilot']).not.toContain(cs.toLowerCase());
      }
    });

    it('shuffles player nickname to a new untaken callsign', () => {
      setPlayerName('Ace');
      const next = shufflePlayerName();
      expect(next).not.toBe('Ace');
      expect(CALLSIGNS).toContain(next);
      expect(playerName.value).toBe(next);
      expect(window.localStorage.getItem('gamebox_name')).toBe(next);
    });

    it('prevents stuck Guest nickname and auto-assigns callsign if empty or Guest', () => {
      setPlayerName('');
      expect(CALLSIGNS).toContain(playerName.value);
      expect(playerName.value.toLowerCase()).not.toBe('guest');

      setPlayerName('Guest');
      expect(CALLSIGNS).toContain(playerName.value);
      expect(playerName.value.toLowerCase()).not.toBe('guest');
    });

    it('allows custom player handles up to 14 characters', () => {
      setPlayerName('RetroCommander');
      expect(playerName.value).toBe('RetroCommander');

      setPlayerName('SuperLongArcadeChampion99');
      expect(playerName.value.length).toBe(14);
    });
  });

  describe('Quick Match Marquee & Last Played Game', () => {
    it('defaults last played game to domino_solo', () => {
      expect(lastPlayedGame.value).toBe('domino_solo');
    });

    it('records played game and updates state and localStorage', () => {
      recordPlayedGame('katro_pvp');
      expect(lastPlayedGame.value).toBe('katro_pvp');
      expect(window.localStorage.getItem('gamebox_last_game')).toBe('katro_pvp');
    });
  });

  describe('Lobby Layout & Anti-Slop Component Structure', () => {
    it('renders Quick Match Marquee without kickers or eyebrows', () => {
      const vnode = Lobby();
      const json = JSON.stringify(vnode);
      expect(json).toContain('QUICK MATCH');
      expect(json).toContain('PLAY NOW');
      expect(json).toContain('Dominoes Solo');
    });

    it('collapses Live Match Feed completely when roomDirectory has 0 rooms', () => {
      roomDirectory.value = [];
      const vnode = Lobby();
      const json = JSON.stringify(vnode);
      expect(json).not.toContain('LIVE MATCH FEED');
      expect(json).not.toContain('No open matches');
    });

    it('expands Live Match Feed when roomDirectory has active rooms', () => {
      roomDirectory.value = [
        { code: 'TEST', host: 'Phantom', game: 'katro_pvp', players: 1, max: 2 }
      ];
      const vnode = Lobby();
      const json = JSON.stringify(vnode);
      expect(json).toContain('LIVE MATCH FEED');
      expect(json).toContain('ACTIVE');
      expect(json).toContain('Phantom');
      expect(json).toContain('TEST');
    });

    it('renders Unified Cartridge Rack with direct action triggers', () => {
      const vnodeSolo = Lobby();
      const jsonSolo = JSON.stringify(vnodeSolo);
      expect(jsonSolo).toContain('CARTRIDGE RACK');
      expect(jsonSolo).toContain('TITLES');
      expect(jsonSolo).toContain('PLAY');
      expect(jsonSolo).toContain('Dominoes Solo');

      // Verify DUEL tab renders HOST trigger
      selectedTab.value = 'DUEL';
      const vnodeDuel = Lobby();
      const jsonDuel = JSON.stringify(vnodeDuel);
      expect(jsonDuel).toContain('HOST');
      expect(jsonDuel).toContain('Katro Duel');
    });

    it('renders Header with profile input and dedicated shuffle button', () => {
      const vnode = Header();
      const json = JSON.stringify(vnode);
      expect(json).toContain('profileInputWrapper');
      expect(json).toContain('shuffleBtn');
      expect(json).toContain('Shuffle Callsign');
    });
  });
});
