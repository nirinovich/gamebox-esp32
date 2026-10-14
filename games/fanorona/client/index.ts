import { FanoronaRenderer } from './FanoronaRenderer';
import type { ClientPlugin } from '../../../frontend/src/renderers/IGameRenderer';

export { FanoronaRenderer };

export const plugin: ClientPlugin = {
  gameIds: ['fanorona', 'fanorona_solo', 'fanorona_duel', 'fanorona_pvp'],
  createRenderer: (juice) => new FanoronaRenderer(juice),
};
