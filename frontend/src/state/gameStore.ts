import { signal, computed } from '@preact/signals';
import {
  ConnectionStatus,
  ViewState,
  GameCategory,
  RoomDirectoryEntry,
  ActiveRoomState,
  GameMetadata
} from '../types/protocol';

export const gameCatalogue = signal<GameMetadata[]>([]);
export const connectionStatus = signal<ConnectionStatus>('DISCONNECTED');
export const latencyMs = signal<number | null>(null);
export const currentView = signal<ViewState>('LOBBY');
export const selectedTab = signal<GameCategory>('SOLO');
export const activeRoom = signal<ActiveRoomState | null>(null);
export const roomDirectory = signal<RoomDirectoryEntry[]>([]);
export const gameState = signal<any | null>(null);
export const errorMessage = signal<string | null>(null);

function getStorage(key: string, fallback: string): string {
  try {
    if (typeof window !== 'undefined' && window.localStorage) {
      const val = window.localStorage.getItem(key);
      return val !== null ? val : fallback;
    }
  } catch {}
  return fallback;
}

function setStorage(key: string, val: string) {
  try {
    if (typeof window !== 'undefined' && window.localStorage) {
      window.localStorage.setItem(key, val);
    }
  } catch {}
}

export const CALLSIGNS = [
  'Viper',
  'Pilot',
  'Neon',
  'Rocket',
  'Ace',
  'Ghost',
  'Striker',
  'Shadow',
  'Blaze',
  'Radar',
  'Titan',
  'Falcon',
  'Havoc',
  'Apex',
  'Phantom',
  'Volt',
  'Cipher',
  'Vortex'
] as const;

export type Callsign = (typeof CALLSIGNS)[number];

export function getTakenNames(): Set<string> {
  const taken = new Set<string>();
  if (activeRoom.value?.opponentName) {
    const opp = activeRoom.value.opponentName.trim().toLowerCase();
    if (opp && opp !== 'waiting for challenger...') {
      taken.add(opp);
    }
  }
  for (const r of roomDirectory.value) {
    if (r.host) {
      taken.add(r.host.trim().toLowerCase());
    }
  }
  return taken;
}

export function getAvailableCallsign(excludeCurrent?: string): string {
  const taken = getTakenNames();
  const currentLower = excludeCurrent ? excludeCurrent.trim().toLowerCase() : '';
  const untaken = CALLSIGNS.filter(
    cs => !taken.has(cs.toLowerCase()) && cs.toLowerCase() !== currentLower
  );
  if (untaken.length > 0) {
    return untaken[Math.floor(Math.random() * untaken.length)];
  }
  const different = CALLSIGNS.filter(cs => cs.toLowerCase() !== currentLower);
  if (different.length > 0) {
    return different[Math.floor(Math.random() * different.length)];
  }
  return CALLSIGNS[0];
}

function initPlayerName(): string {
  const stored = getStorage('gamebox_name', '');
  if (!stored || stored.trim() === '' || stored.trim().toLowerCase() === 'guest') {
    const fresh = getAvailableCallsign();
    setStorage('gamebox_name', fresh);
    return fresh;
  }
  return stored.trim().slice(0, 14);
}

export const playerName = signal<string>(initPlayerName());
export const soundMuted = signal<boolean>(getStorage('gamebox_muted', 'false') === 'true');
export const crtFilter = signal<boolean>(getStorage('gamebox_crt', 'false') === 'true');
export const lastPlayedGame = signal<string>(getStorage('gamebox_last_game', 'domino_solo'));

export const filteredGames = computed(() => {
  const tab = selectedTab.value;
  const catalogue = gameCatalogue.value;
  if (tab === 'ALL') return catalogue;
  return catalogue.filter(g => g.category === tab);
});

export function setPlayerName(name: string) {
  const trimmed = name.trim().slice(0, 14);
  const clean = (!trimmed || trimmed.toLowerCase() === 'guest') ? getAvailableCallsign() : trimmed;
  playerName.value = clean;
  setStorage('gamebox_name', clean);
}

export function shufflePlayerName(): string {
  const next = getAvailableCallsign(playerName.value);
  setPlayerName(next);
  return next;
}

export function recordPlayedGame(key: string) {
  lastPlayedGame.value = key;
  setStorage('gamebox_last_game', key);
}

export function toggleSound() {
  const next = !soundMuted.value;
  soundMuted.value = next;
  setStorage('gamebox_muted', String(next));
}

export function toggleCrt() {
  const next = !crtFilter.value;
  crtFilter.value = next;
  setStorage('gamebox_crt', String(next));
}

export function resetRoomState() {
  activeRoom.value = null;
  gameState.value = null;
  currentView.value = 'LOBBY';
}
