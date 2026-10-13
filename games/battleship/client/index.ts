import { BattleshipRenderer } from './BattleshipRenderer';
import type { ClientPlugin } from '../../../frontend/src/renderers/IGameRenderer';
import { CameraJuice } from '../../../frontend/src/renderers/CameraJuice';

export { BattleshipRenderer };

export const plugin: ClientPlugin = {
  gameIds: ['battleship', 'battleship_solo', 'battleship_pvp'],
  createRenderer: (juice) => new BattleshipRenderer(juice || new CameraJuice()),
};
