import { describe, it, expect, vi } from 'vitest';
import { SnakeRenderer } from './SnakeRenderer';

describe('SnakeRenderer', () => {
  it('sends direction on swipe gesture', () => {
    const dispatchSpy = vi.fn();
    const renderer = new SnakeRenderer();
    renderer.setDispatcher(dispatchSpy);
    renderer.resize(400, 400);

    renderer.handlePointerDown(100, 100);
    renderer.handlePointerMove(150, 100);
    expect(dispatchSpy).toHaveBeenCalledWith({ cmd: 'dir', val: 'RIGHT' });
  });
});
