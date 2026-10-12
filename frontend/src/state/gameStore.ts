import { signal, computed } from '@preact/signals';
import {
  ConnectionStatus,
  ViewState,
  GameCategory,
  RoomDirectoryEntry,
  ActiveRoomState,
  GameMetadata
} from '../types/protocol';

export const GAME_CATALOGUE: GameMetadata[] = [
  {
    key: 'snake_solo',
    title: 'Snake Survival',
    category: 'SOLO',
    description: 'Classic arcade snake with robotic glowing eyes & embers',
    tag: 'Solo AI',
    icon: '🐍',
    isDuel: false
  },
  {
    key: 'snake_duel',
    title: 'Snake Arena',
    category: 'DUEL',
    description: '2P Competitive snake head-to-head collision duel',
    tag: '2P Duel',
    icon: '⚔️',
    isDuel: true
  },
  {
    key: 'ttt_solo',
    title: 'Tic-Tac-Toe AI',
    category: 'SOLO',
    description: 'Challenge the Minimax algorithmic AI engine',
    tag: 'Solo AI',
    icon: '⭕',
    isDuel: false
  },
  {
    key: 'ttt_pvp',
    title: '2P Tic-Tac-Toe',
    category: 'DUEL',
    description: 'Turn-based tactical duel with overdrive laser lines',
    tag: '2P Duel',
    icon: '❌',
    isDuel: true
  },
  {
    key: 'connect4_solo',
    title: 'Connect Four AI',
    category: 'SOLO',
    description: 'Gravity disc drops with physical elastic bounce',
    tag: 'Solo AI',
    icon: '🔴',
    isDuel: false
  },
  {
    key: 'connect4_pvp',
    title: 'Connect Four Duel',
    category: 'DUEL',
    description: 'Vertical four-in-a-row column targeting showdown',
    tag: '2P Duel',
    icon: '🟡',
    isDuel: true
  },
  {
    key: 'pong_solo',
    title: 'Pong Arcade',
    category: 'SOLO',
    description: 'Phosphor comet trails & paddle collision sparks vs AI',
    tag: 'Solo AI',
    icon: '🏓',
    isDuel: false
  },
  {
    key: 'pong_duel',
    title: 'Pong Duel',
    category: 'DUEL',
    description: 'High-speed vertical drag paddle volley match',
    tag: '2P Duel',
    icon: '⚡',
    isDuel: true
  },
  {
    key: 'tron_solo',
    title: 'Tron Arena AI',
    category: 'SOLO',
    description: 'Neon bloom light cycle trails vs Voronoi bot',
    tag: 'Solo AI',
    icon: '🏍️',
    isDuel: false
  },
  {
    key: 'tron_duel',
    title: 'Tron Light Cycles',
    category: 'DUEL',
    description: 'Fast-paced grid trapping synthwave duel',
    tag: '2P Duel',
    icon: '🌌',
    isDuel: true
  },
  {
    key: 'battleship_solo',
    title: 'Battleship Solo',
    category: 'SOLO',
    description: 'Naval grid warfare with 360-degree sonar sweep',
    tag: 'Solo AI',
    icon: '🚢',
    isDuel: false
  },
  {
    key: 'battleship_pvp',
    title: 'Battleship Fleet',
    category: 'DUEL',
    description: 'Blind targeting naval duel with torpedo fire VFX',
    tag: '2P Duel',
    icon: '🎯',
    isDuel: true
  },
  {
    key: 'domino_solo',
    title: 'Dominoes Solo',
    category: 'SOLO',
    description: 'Double-six draw dominoes on emerald felt table vs Bot',
    tag: 'Solo AI',
    icon: '🀄',
    isDuel: false
  },
  {
    key: 'domino_pvp',
    title: '2-3P Dominoes',
    category: 'PARTY',
    description: 'Social parlor dominoes with Boneyard & Fog of War',
    tag: 'Party 3P',
    icon: '🎲',
    isDuel: false
  }
];

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
  if (tab === 'ALL') return GAME_CATALOGUE;
  return GAME_CATALOGUE.filter(g => g.category === tab);
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
