// ============================================================================
// KatroRenderer.test.ts — Tests unitaires Vitest co-localisés du renderer
//
// Exécution : depuis le dossier frontend/
//   npm test -- --run ../games/katro/client/KatroRenderer.test.ts
// ============================================================================

import { describe, it, expect, vi } from 'vitest';
import { KatroRenderer } from './KatroRenderer';

// ---- Minimal canvas mock (JSDOM ne fournit pas getContext CanvasRenderingContext2D) ----
function makeCtx(): CanvasRenderingContext2D {
  const target: any = {
    fillStyle: '',
    font: '',
    textAlign: 'left',
    fillRect: vi.fn(),
    fillText: vi.fn(),
    beginPath: vi.fn(),
    arc: vi.fn(),
    fill: vi.fn(),
    save: vi.fn(),
    restore: vi.fn(),
    stroke: vi.fn(),
    strokeRect: vi.fn(),
    measureText: vi.fn(() => ({ width: 50 })),
    moveTo: vi.fn(),
    lineTo: vi.fn(),
  };
  return new Proxy(target, {
    get(t, prop) {
      if (prop in t) return t[prop];
      t[prop] = vi.fn();
      return t[prop];
    }
  }) as unknown as CanvasRenderingContext2D;
}

// Payload d'état canonique Katro (4 graines, tour P1)
const initialState = {
  type: 'state',
  game: 'katro',
  turn: 1,
  finished: false,
  winner: 0,
  scores: [0, 0],
  board: Array<number>(32).fill(4),
};

describe('KatroRenderer', () => {
  it("s'instancie sans erreur", () => {
    const renderer = new KatroRenderer();
    expect(renderer).toBeDefined();
  });

  it('dispatch une commande sow au pointer-up sur son camp', () => {
    const dispatch = vi.fn();
    const renderer = new KatroRenderer();
    renderer.init(undefined, undefined, dispatch, 1);
    renderer.resize(400, 400);

    // Pointer-down puis pointer-up sur la case 2 (col 2, rangée 0 → P1)
    // topOffset = 42, cellW = 400/4 = 100
    // Case 2 : x = 2*100+50 = 250, y = 84
    renderer.handlePointerDown(250, 84);
    renderer.handlePointerUp(250, 84);

    // Vérifie qu'une action sow a été dispatchée
    expect(dispatch).toHaveBeenCalledOnce();
    const call = dispatch.mock.calls[0][0];
    expect(call.cmd).toBe('sow');
    expect(typeof call.cell).toBe('number');
    expect(call.cell).toBeGreaterThanOrEqual(0);
    expect(call.cell).toBeLessThan(16);  // Case P1
  });

  it("n'envoie pas de commande si le pointer-up est hors de la case initiale", () => {
    const dispatch = vi.fn();
    const renderer = new KatroRenderer();
    renderer.init(undefined, undefined, dispatch, 1);
    renderer.resize(400, 400);

    renderer.handlePointerDown(25, 84);   // Case 0 (x: 0..100)
    renderer.handlePointerUp(150, 84);    // Case 1 (x: 100..200)

    expect(dispatch).not.toHaveBeenCalled();
  });

  it("n'envoie pas de commande si le joueur clique dans le camp adverse", () => {
    const dispatch = vi.fn();
    const renderer = new KatroRenderer();
    renderer.init(undefined, undefined, dispatch, 1);  // Joueur local = P1
    renderer.resize(400, 400);

    // Rangée 2 (P2) : y ≈ 40 + 2*88 + 44 = 260
    renderer.handlePointerDown(25, 260);
    renderer.handlePointerUp(25, 260);

    expect(dispatch).not.toHaveBeenCalled();
  });

  it('render() ne lance pas d\'exception avec un état valide', () => {
    const renderer = new KatroRenderer();
    renderer.init();
    renderer.resize(400, 400);
    const ctx = makeCtx();

    expect(() => renderer.render(initialState, ctx)).not.toThrow();
  });

  it('render() tolère un état null (écran de connexion)', () => {
    const renderer = new KatroRenderer();
    renderer.init();
    renderer.resize(400, 400);
    const ctx = makeCtx();

    expect(() => renderer.render(null, ctx)).not.toThrow();
  });

  it('destroy() ne lance pas d\'exception', () => {
    const renderer = new KatroRenderer();
    expect(() => renderer.destroy()).not.toThrow();
  });
});
