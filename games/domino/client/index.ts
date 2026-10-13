import { DominoRenderer } from './DominoRenderer';
import type { ClientPlugin } from '../../../frontend/src/renderers/IGameRenderer';

export { DominoRenderer };

export const plugin: ClientPlugin = {
  gameIds: ['domino', 'domino_solo', 'domino_duel', 'domino_pvp'],
  createRenderer: () => new DominoRenderer(),
};
