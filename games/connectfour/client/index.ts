import { ConnectFourRenderer } from './ConnectFourRenderer';
import type { ClientPlugin } from '../../../frontend/src/renderers/IGameRenderer';
import { CameraJuice } from '../../../frontend/src/renderers/CameraJuice';

export { ConnectFourRenderer };

export const plugin: ClientPlugin = {
  gameIds: ['connectfour', 'connect4_solo', 'connect4_pvp', 'c4_solo', 'c4_pvp'],
  createRenderer: (juice) => new ConnectFourRenderer(juice || new CameraJuice()),
};
