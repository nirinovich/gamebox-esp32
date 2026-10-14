import { IGameRenderer } from '../renderers/IGameRenderer';
import { SnakeRenderer } from '@games/snake/client';
import { TicTacToeRenderer } from '@games/tictactoe/client';
import { PongRenderer } from '@games/pong/client';
import { TronRenderer } from '@games/tron/client';
import { ConnectFourRenderer } from '@games/connectfour/client';
import { BattleshipRenderer } from '@games/battleship/client';
import { DominoRenderer } from '@games/domino/client';
import { FanoronaRenderer } from '@games/fanorona/client';
import { CameraJuice } from '../renderers/CameraJuice';

export type RendererFactory = (juice?: CameraJuice) => IGameRenderer;

export const gameRendererMap: Record<string, RendererFactory> = {
  snake_solo: () => new SnakeRenderer(),
  snake_duel: () => new SnakeRenderer(),
  ttt_solo: () => new TicTacToeRenderer(),
  ttt_pvp: () => new TicTacToeRenderer(),
  pong_solo: () => new PongRenderer(),
  pong_duel: () => new PongRenderer(),
  tron_solo: () => new TronRenderer(),
  tron_duel: () => new TronRenderer(),
  c4_solo: (juice) => new ConnectFourRenderer(juice || new CameraJuice()),
  c4_pvp: (juice) => new ConnectFourRenderer(juice || new CameraJuice()),
  battleship_solo: () => new BattleshipRenderer(),
  battleship_pvp: () => new BattleshipRenderer(),
  domino_solo: () => new DominoRenderer(),
  domino_pvp: () => new DominoRenderer(),
  fanorona_solo: (juice) => new FanoronaRenderer(juice),
  fanorona_duel: (juice) => new FanoronaRenderer(juice)
};

export function getGameRenderer(gameType: string, juice?: CameraJuice): IGameRenderer {
  const factory = gameRendererMap[gameType];
  if (factory) {
    return factory(juice);
  }
  // Fallback pattern matching
  if (gameType.includes('snake')) return new SnakeRenderer();
  if (gameType.includes('pong')) return new PongRenderer();
  if (gameType.includes('tron')) return new TronRenderer();
  if (gameType.includes('c4') || gameType.includes('connect')) return new ConnectFourRenderer(juice || new CameraJuice());
  if (gameType.includes('battleship')) return new BattleshipRenderer();
  if (gameType.includes('domino')) return new DominoRenderer();
  if (gameType.includes('fanorona')) return new FanoronaRenderer(juice);
  return new TicTacToeRenderer();
}
