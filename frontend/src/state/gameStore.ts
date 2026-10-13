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

export const connectionStatus = signal<ConnectionStatus>('DISCONNECTED');
export const latencyMs = signal<number | null>(null);
export const playerName = signal<string>(getStorage('gamebox_name', 'Guest'));
export const soundMuted = signal<boolean>(getStorage('gamebox_muted', 'false') === 'true');
export const crtFilter = signal<boolean>(getStorage('gamebox_crt', 'false') === 'true');
export const currentView = signal<ViewState>('LOBBY');
export const selectedTab = signal<GameCategory>('ALL');
export const activeRoom = signal<ActiveRoomState | null>(null);
export const roomDirectory = signal<RoomDirectoryEntry[]>([]);
export const gameState = signal<any | null>(null);
export const errorMessage = signal<string | null>(null);

export const filteredGames = computed(() => {
  const tab = selectedTab.value;
  const catalogue = gameCatalogue.value;
  if (tab === 'ALL') return catalogue;
  return catalogue.filter(g => g.category === tab);
});

export function setPlayerName(name: string) {
  const clean = name.trim().slice(0, 14) || 'Guest';
  playerName.value = clean;
  setStorage('gamebox_name', clean);
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
