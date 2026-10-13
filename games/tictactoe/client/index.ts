import { TicTacToeRenderer } from './TicTacToeRenderer';
import type { ClientPlugin } from '../../../frontend/src/renderers/IGameRenderer';

export { TicTacToeRenderer };

export const plugin: ClientPlugin = {
  gameIds: ['tictactoe', 'ttt_solo', 'ttt_pvp', 'tictactoe_pvp'],
  createRenderer: () => new TicTacToeRenderer(),
};
