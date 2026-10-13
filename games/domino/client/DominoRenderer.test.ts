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

    // Click in hand area (y >= 300)
    // 2 tiles: tileW = 42, totalW = 84, startX = (400 - 84)/2 = 158
    // Click on first tile index 0: x = 160, y = 320
    renderer.handlePointerDown(160, 320);

    expect(dispatchSpy).toHaveBeenCalledWith({ cmd: 'play', index: 0, end: 'left' });
  });

  it('flags choiceNeeded when tile matches both ends', () => {
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
    // 1 tile: startX = (400 - 42)/2 = 179
    renderer.handlePointerDown(180, 320);

    expect(renderer.choiceNeeded).toBe(true);
    expect(renderer.selectedTileIdx).toBe(0);
    expect(dispatchSpy).not.toHaveBeenCalled();
  });
});
