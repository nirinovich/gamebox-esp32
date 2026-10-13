export type GameKey =
  | 'snake_solo'
  | 'snake_duel'
  | 'ttt_solo'
  | 'ttt_pvp'
  | 'connect4_solo'
  | 'connect4_pvp'
  | 'pong_solo'
  | 'pong_duel'
  | 'tron_solo'
  | 'tron_duel'
  | 'battleship_solo'
  | 'battleship_pvp'
  | 'domino_solo'
  | 'domino_duel'
  | 'domino_pvp'
  | 'katro_solo'   // Katro solo vs Bot IA
  | 'katro_pvp';  // Katro PvP 2 joueurs

export type GameCategory = 'ALL' | 'SOLO' | 'DUEL' | 'PARTY';

export interface GameMetadata {
  key: GameKey;
  title: string;
  category: 'SOLO' | 'DUEL' | 'PARTY';
  description: string;
  tag: string;
  icon: string;
  isDuel: boolean;
}

export interface RoomDirectoryEntry {
  code: string;
  host: string;
  game: GameKey;
  players: number;
  max: number;
}

export type ConnectionStatus = 'DISCONNECTED' | 'CONNECTING' | 'CONNECTED';

export type ViewState = 'LOBBY' | 'WAITING' | 'GAME';

export interface ActiveRoomState {
  code: string;
  isHost: boolean;
  gameType: GameKey;
  playerNum: number;
  opponentName: string;
  waitingCount?: number;
  waitingMax?: number;
  rematchCount?: number;
  rematchNeeded?: number;
  seriesScore?: { p1?: number; p2?: number };
}

export interface ServerMessage {
  type: string;
  ts?: number;
  rooms?: RoomDirectoryEntry[];
  code?: string;
  player?: number;
  host?: string;
  count?: number;
  max?: number;
  requested?: number[];
  needed?: number;
  finished?: boolean;
  winner?: number;
  [key: string]: unknown;
}
