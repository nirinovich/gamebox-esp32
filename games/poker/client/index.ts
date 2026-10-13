import { PokerRenderer } from './PokerRenderer';
import type { ClientPlugin } from '../../../frontend/src/renderers/IGameRenderer';

export { PokerRenderer };

export const plugin: ClientPlugin = {
  gameIds: ['poker', 'poker_solo', 'poker_table'],
  createRenderer: () => new PokerRenderer(),
};
