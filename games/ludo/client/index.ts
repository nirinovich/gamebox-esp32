import { LudoRenderer } from './LudoRenderer';
import type { ClientPlugin } from '../../../frontend/src/renderers/IGameRenderer';

export { LudoRenderer };

export const plugin: ClientPlugin = {
  gameIds: ['ludo', 'ludo_solo', 'ludo_quick', 'ludo_classic'],
  createRenderer: () => new LudoRenderer(),
};
