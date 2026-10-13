import { PongRenderer } from './PongRenderer';
import type { ClientPlugin } from '../../../frontend/src/renderers/IGameRenderer';

export { PongRenderer };

export const plugin: ClientPlugin = {
  gameIds: ['pong', 'pong_solo', 'pong_duel'],
  createRenderer: () => new PongRenderer(),
};
