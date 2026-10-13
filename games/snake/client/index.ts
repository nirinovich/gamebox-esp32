import { SnakeRenderer } from './SnakeRenderer';
import type { ClientPlugin } from '../../../frontend/src/renderers/IGameRenderer';

export { SnakeRenderer };

export const plugin: ClientPlugin = {
  gameIds: ['snake', 'snake_solo', 'snake_duel'],
  createRenderer: () => new SnakeRenderer(),
};
