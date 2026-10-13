import { describe, it, expect, vi } from 'vitest';
import { PongRenderer } from './PongRenderer';

describe('PongRenderer', () => {
  it('sends paddle direction on vertical drag', () => {
    const dispatchSpy = vi.fn();
    const renderer = new PongRenderer();
    renderer.setDispatcher(dispatchSpy);
    renderer.resize(400, 400);

    // Click in upper half (< 185)
    renderer.handlePointerDown(200, 100);
    expect(dispatchSpy).toHaveBeenCalledWith({ cmd: 'dir', val: 'UP' });

    // Drag to lower half (> 215)
    renderer.handlePointerMove(200, 300);
    expect(dispatchSpy).toHaveBeenCalledWith({ cmd: 'dir', val: 'DOWN' });

    // Release stops moving
    renderer.handlePointerUp(200, 300);
    expect(dispatchSpy).toHaveBeenCalledWith({ cmd: 'dir', val: 'STOP' });
  });
});
