import { describe, it, expect, vi } from 'vitest';
import { DominoRenderer } from './DominoRenderer';

function createMockCtx(): CanvasRenderingContext2D {
  const target: any = {
    createRadialGradient: vi.fn(() => ({ addColorStop: vi.fn() })),
    fillRect: vi.fn(),
    strokeRect: vi.fn(),
    fillText: vi.fn(),
    beginPath: vi.fn(),
    arc: vi.fn(),
    fill: vi.fn(),
    stroke: vi.fn(),
    save: vi.fn(),
    restore: vi.fn(),
    roundRect: vi.fn(),
    moveTo: vi.fn(),
    lineTo: vi.fn(),
    rect: vi.fn(),
  };
  return new Proxy(target, {
    get(t, prop) {
      if (prop in t) return t[prop];
      t[prop] = vi.fn();
      return t[prop];
    }
  }) as unknown as CanvasRenderingContext2D;
}

describe('DominoRenderer', () => {
  it('dispatches play command when hand tile is clicked and matches single end', () => {
    const dispatchSpy = vi.fn();
    const renderer = new DominoRenderer();
    renderer.init(undefined, undefined, dispatchSpy, 1);

    const mockCtx = createMockCtx();
    const mockState = {
      left_end: 3,
      right_end: 6,
      hands: {
        1: [[3, 5], [6, 6]]
      }
    };

    renderer.render(mockState, mockCtx);
    // Click tile 0 in hand
    renderer.handlePointerDown(160, 320);

    expect(dispatchSpy).toHaveBeenCalledWith({ cmd: 'play', index: 0, end: 'left' });
  });

  it('activates tactile board anchors when tile matches both ends, and commits on anchor tap', () => {
    const dispatchSpy = vi.fn();
    const renderer = new DominoRenderer();
    renderer.init(undefined, undefined, dispatchSpy, 1);

    const mockCtx = createMockCtx();
    const mockState = {
      left_end: 3,
      right_end: 5,
      hands: {
        1: [[3, 5]]
      }
    };

    renderer.render(mockState, mockCtx);
    // Tap tile in hand
    renderer.handlePointerDown(180, 320);

    // Activates selection and dispatches select_tile
    expect(dispatchSpy).toHaveBeenCalledWith({ cmd: 'select_tile', index: 0 });
    expect(renderer.selectedTileIdx).toBe(0);
    expect(renderer.choiceNeeded).toBe(true);

    // Now tap Right Board End Anchor (x: 300, y: 45)
    renderer.handlePointerDown(300, 45);
    expect(dispatchSpy).toHaveBeenCalledWith({ cmd: 'play', index: 0, end: 'right' });
    expect(renderer.selectedTileIdx).toBe(-1);
    expect(renderer.choiceNeeded).toBe(false);
  });

  it('commits on Left Board End Anchor tap when ambiguous tile is selected', () => {
    const dispatchSpy = vi.fn();
    const renderer = new DominoRenderer();
    renderer.init(undefined, undefined, dispatchSpy, 1);

    const mockCtx = createMockCtx();
    const mockState = {
      left_end: 3,
      right_end: 5,
      hands: {
        1: [[3, 5]]
      }
    };

    renderer.render(mockState, mockCtx);
    // Tap tile in hand
    renderer.handlePointerDown(180, 320);

    // Tap Left Board End Anchor (x: 50, y: 45)
    renderer.handlePointerDown(50, 45);
    expect(dispatchSpy).toHaveBeenCalledWith({ cmd: 'play', index: 0, end: 'left' });
    expect(renderer.selectedTileIdx).toBe(-1);
  });
});
