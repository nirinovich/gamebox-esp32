import { IGameRenderer, ClientPlugin } from '../renderers/IGameRenderer';
import { CameraJuice } from '../renderers/CameraJuice';

export type RendererFactory = (juice?: CameraJuice) => IGameRenderer;

export const gameRendererMap: Record<string, RendererFactory> = {};

// Vite glob auto-discovery of all client game packages under games/*/client/
const gameModules = import.meta.glob<Record<string, any>>('../../../games/*/client/index.ts', { eager: true });

for (const modulePath in gameModules) {
  const mod = gameModules[modulePath];
  if (mod.plugin && mod.plugin.createRenderer) {
    const plugin = mod.plugin as ClientPlugin;
    const ids = Array.isArray(plugin.gameIds) ? plugin.gameIds : [plugin.gameIds];
    for (const id of ids) {
      if (id) {
        gameRendererMap[id] = plugin.createRenderer;
      }
    }
  } else {
    // Fallback: extract any exported *Renderer class
    const match = modulePath.match(/games\/([^/]+)\/client/);
    const folderName = match ? match[1] : '';
    for (const exp in mod) {
      if (typeof mod[exp] === 'function' && exp.endsWith('Renderer')) {
        const RendererClass = mod[exp];
        const factory: RendererFactory = (juice) => new RendererClass(juice || new CameraJuice());
        if (folderName) {
          gameRendererMap[folderName] = factory;
          gameRendererMap[`${folderName}_solo`] = factory;
          gameRendererMap[`${folderName}_duel`] = factory;
          gameRendererMap[`${folderName}_pvp`] = factory;
        }
      }
    }
  }
}

export function getGameRenderer(gameType: string, juice?: CameraJuice): IGameRenderer {
  const factory = gameRendererMap[gameType];
  if (factory) {
    return factory(juice);
  }
  for (const key in gameRendererMap) {
    if (gameType.includes(key)) {
      return gameRendererMap[key](juice);
    }
  }
  const fallback = gameRendererMap['domino_solo'] || gameRendererMap['katro_solo'] || (Object.values(gameRendererMap)[0] ? Object.values(gameRendererMap)[0] : null);
  if (fallback) return fallback(juice);
  throw new Error(`No renderer registered for gameType: ${gameType}`);
}
