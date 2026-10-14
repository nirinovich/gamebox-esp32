import { KatroRenderer } from './KatroRenderer';
import type { ClientPlugin } from '../../../frontend/src/renderers/IGameRenderer';

export { KatroRenderer };

export const plugin: ClientPlugin = {
  gameIds: ['katro', 'katro_solo', 'katro_duel', 'katro_pvp'],
  createRenderer: (juice?: any) => new KatroRenderer(juice),
};
