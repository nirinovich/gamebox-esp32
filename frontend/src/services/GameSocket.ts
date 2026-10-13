import {
  connectionStatus,
  latencyMs,
  playerName,
  currentView,
  activeRoom,
  roomDirectory,
  gameState,
  gameCatalogue
} from '../state/gameStore';
import { GameKey, ServerMessage } from '../types/protocol';

class GameSocketService {
  private ws: WebSocket | null = null;
  private reconnectTimeout: number | null = null;
  private pingInterval: number | null = null;
  private roomPollInterval: number | null = null;
  private backoffDelay = 1000;
  private forcedClosed = false;

  public init() {
    this.connect();
  }

  private connect() {
    if (this.ws && (this.ws.readyState === WebSocket.OPEN || this.ws.readyState === WebSocket.CONNECTING)) {
      return;
    }

    connectionStatus.value = 'CONNECTING';

    const proto = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
    const host = window.location.host || 'localhost:8080';
    const url = `${proto}//${host}/ws`;

    try {
      this.ws = new WebSocket(url);
    } catch (e) {
      this.scheduleReconnect();
      return;
    }

    this.ws.onopen = () => {
      connectionStatus.value = 'CONNECTED';
      this.backoffDelay = 1000;

      // Register player handle with Host
      this.send({ cmd: 'name', name: playerName.value });
      this.send({ cmd: 'get_rooms' });

      this.startHeartbeat();
      this.startRoomPolling();
    };

    this.ws.onmessage = (event: MessageEvent) => {
      try {
        const msg: ServerMessage = JSON.parse(event.data);
        this.handleMessage(msg);
      } catch (err) {
        console.error('Failed to parse WebSocket packet', err);
      }
    };

    this.ws.onclose = () => {
      connectionStatus.value = 'DISCONNECTED';
      latencyMs.value = null;
      this.stopHeartbeat();
      this.stopRoomPolling();
      if (!this.forcedClosed) {
        this.scheduleReconnect();
      }
    };

    this.ws.onerror = () => {
      if (this.ws) {
        this.ws.close();
      }
    };
  }

  private scheduleReconnect() {
    if (this.reconnectTimeout) return;
    this.reconnectTimeout = window.setTimeout(() => {
      this.reconnectTimeout = null;
      this.backoffDelay = Math.min(this.backoffDelay * 1.5, 8000);
      this.connect();
    }, this.backoffDelay);
  }

  private startHeartbeat() {
    this.stopHeartbeat();
    this.pingInterval = window.setInterval(() => {
      if (this.isOpen()) {
        this.send({ cmd: 'ping', ts: Date.now() });
      }
    }, 2500);
  }

  private stopHeartbeat() {
    if (this.pingInterval) {
      clearInterval(this.pingInterval);
      this.pingInterval = null;
    }
  }

  private startRoomPolling() {
    this.stopRoomPolling();
    this.roomPollInterval = window.setInterval(() => {
      if (this.isOpen() && currentView.value === 'LOBBY') {
        this.send({ cmd: 'get_rooms' });
      }
    }, 3000);
  }

  private stopRoomPolling() {
    if (this.roomPollInterval) {
      clearInterval(this.roomPollInterval);
      this.roomPollInterval = null;
    }
  }

  private handleMessage(msg: ServerMessage) {
    if (msg.type === 'state' || (msg as any).game) {
      gameState.value = msg;
      return;
    }

    switch (msg.type) {
      case 'pong':
        if (msg.ts) {
          latencyMs.value = Math.max(0, Date.now() - msg.ts);
        }
        break;

      case 'room_directory':
        roomDirectory.value = msg.rooms || [];
        if (msg.games && Array.isArray(msg.games) && msg.games.length > 0) {
          gameCatalogue.value = msg.games.map((g: any) => ({
            key: g.id as GameKey,
            title: g.title,
            category: g.category,
            description: g.description,
            tag: g.category === 'SOLO' ? 'Solo AI' : (g.isDuel ? '2P Duel' : `${g.max}P Party`),
            icon: g.icon || g.id,
            isDuel: g.isDuel ?? (g.category === 'DUEL'),
            inputs: g.inputs || []
          }));
        }
        break;

      case 'room_created':
        gameState.value = null;
        activeRoom.value = {
          code: msg.code || '',
          isHost: true,
          gameType: (msg.game as GameKey) || 'katro_pvp',
          playerNum: 1,
          opponentName: 'Waiting for challenger...',
          waitingCount: 1,
          waitingMax: msg.max || 2
        };
        currentView.value = 'WAITING';
        break;

      case 'room_joined': {
        gameState.value = null;
        const playerNum = msg.player || 2;
        const maxPlayers = msg.max || 2;
        activeRoom.value = {
          code: msg.code || '',
          isHost: false,
          gameType: (msg.game as GameKey) || (activeRoom.value?.gameType || 'katro_pvp'),
          playerNum,
          opponentName: msg.host || 'Host',
          waitingCount: playerNum,
          waitingMax: maxPlayers
        };
        if (playerNum >= maxPlayers) {
          currentView.value = 'GAME';
        } else {
          currentView.value = 'WAITING';
        }
        break;
      }

      case 'room_waiting_update':
        if (activeRoom.value) {
          activeRoom.value = {
            ...activeRoom.value,
            waitingCount: msg.count || activeRoom.value.waitingCount,
            waitingMax: msg.max || activeRoom.value.waitingMax
          };
        }
        break;

      case 'rematch_update':
        if (activeRoom.value) {
          activeRoom.value = {
            ...activeRoom.value,
            rematchCount: (msg.requested && msg.requested.length) || 1,
            rematchNeeded: msg.needed || 2
          };
        }
        break;

      case 'room_ready':
        currentView.value = 'GAME';
        break;

      case 'opponent_left':
        activeRoom.value = null;
        gameState.value = null;
        currentView.value = 'LOBBY';
        this.send({ cmd: 'get_rooms' });
        break;

      case 'state':
        gameState.value = msg;
        break;

      default:
        break;
    }
  }

  public isOpen(): boolean {
    return this.ws !== null && this.ws.readyState === WebSocket.OPEN;
  }

  public send(payload: object) {
    if (this.isOpen()) {
      this.ws?.send(JSON.stringify(payload));
    }
  }

  public updateName(name: string) {
    this.send({ cmd: 'name', name });
  }

  public createRoom(game: GameKey) {
    gameState.value = null;
    const players = game === 'domino_duel' ? 2 : (game === 'domino_pvp' ? 3 : 2);
    this.send({ cmd: 'create', game, players, max: players });
  }

  public startSolo(game: GameKey) {
    gameState.value = null;
    activeRoom.value = {
      code: 'SOLO',
      isHost: true,
      gameType: game,
      playerNum: 1,
      opponentName: 'Bot Engine'
    };
    currentView.value = 'GAME';
    this.send({ cmd: 'start', game });
  }

  public joinRoom(code: string) {
    gameState.value = null;
    this.send({ cmd: 'leave' });
    this.send({ cmd: 'join', code: code.toUpperCase() });
  }

  public leaveRoom() {
    this.send({ cmd: 'leave' });
    activeRoom.value = null;
    gameState.value = null;
    currentView.value = 'LOBBY';
    this.send({ cmd: 'get_rooms' });
  }

  public sendRematch() {
    this.send({ cmd: 'rematch' });
  }

  public sendTick() {
    this.send({ cmd: 'tick' });
  }
}

export const gameSocket = new GameSocketService();
