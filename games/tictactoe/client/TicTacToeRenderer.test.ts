import { describe, it, expect, vi } from 'vitest';
import { TicTacToeRenderer } from './TicTacToeRenderer';

describe('TicTacToeRenderer', () => {
  it('calculates cell 0 on top-left click', () => {
    const dispatchSpy = vi.fn();
    const renderer = new TicTacToeRenderer();
    renderer.setDispatcher(dispatchSpy);
    renderer.resize(400, 400);

    renderer.handlePointerDown(50, 50);
    expect(dispatchSpy).toHaveBeenCalledWith({ cmd: 'place', cell: 0 });

    renderer.handlePointerDown(200, 200);
    expect(dispatchSpy).toHaveBeenCalledWith({ cmd: 'place', cell: 4 });
  });
});
