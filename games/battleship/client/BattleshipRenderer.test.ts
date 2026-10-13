import { describe, it, expect, vi } from 'vitest';
import { BattleshipRenderer } from './BattleshipRenderer';

describe('BattleshipRenderer', () => {
  it('calculates row and col on click to fire', () => {
    const dispatchSpy = vi.fn();
    const renderer = new BattleshipRenderer();
    renderer.setDispatcher(dispatchSpy);
    renderer.resize(400, 400);

    // 400 / 10 = 40px per cell. Click at (45, 85) -> col 1, row 2
    renderer.handlePointerDown(45, 85);
    expect(dispatchSpy).toHaveBeenCalledWith({ cmd: 'fire', row: 2, col: 1 });
  });
});
