import { describe, it, expect, vi } from 'vitest';
import { TronRenderer } from './TronRenderer';

describe('TronRenderer', () => {
  it('sends direction on swipe gesture', () => {
    const dispatchSpy = vi.fn();
    const renderer = new TronRenderer();
    renderer.setDispatcher(dispatchSpy);
    renderer.resize(400, 400);

    renderer.handlePointerDown(200, 200);
    // Swipe Down
    renderer.handlePointerMove(200, 250);
    expect(dispatchSpy).toHaveBeenCalledWith({ cmd: 'dir', val: 'DOWN' });
  });
});
