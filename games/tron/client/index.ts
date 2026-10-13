import { TronRenderer } from './TronRenderer';
import type { ClientPlugin } from '../../../frontend/src/renderers/IGameRenderer';

export { TronRenderer };

export const plugin: ClientPlugin = {
  gameIds: ['tron', 'tron_solo', 'tron_duel'],
  createRenderer: () => new TronRenderer(),
};
