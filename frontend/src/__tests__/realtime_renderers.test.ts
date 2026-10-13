import { describe, it, expect, vi } from 'vitest';
import { SnakeRenderer } from '@games/snake/client';
import { PongRenderer } from '@games/pong/client';
import { TronRenderer } from '@games/tron/client';
import { DominoRenderer } from '@games/domino/client';

describe('Real-Time & Tabletop Renderers', () => {
  it('SnakeRenderer sends direction on swipe gesture', () => {
    const dispatchSpy = vi.fn();
    const renderer = new SnakeRenderer();
    renderer.setDispatcher(dispatchSpy);
    renderer.resize(400, 400);

    renderer.handlePointerDown(100, 100);
    // Swipe Right
    renderer.handlePointerMove(150, 100);
    expect(dispatchSpy).toHaveBeenCalledWith({ cmd: 'dir', val: 'RIGHT' });
  });

  it('TronRenderer sends direction on swipe gesture', () => {
    const dispatchSpy = vi.fn();
    const renderer = new TronRenderer();
    renderer.setDispatcher(dispatchSpy);
    renderer.resize(400, 400);

    renderer.handlePointerDown(200, 200);
    // Swipe Down
    renderer.handlePointerMove(200, 250);
    expect(dispatchSpy).toHaveBeenCalledWith({ cmd: 'dir', val: 'DOWN' });
  });

  it('PongRenderer sends paddle direction on vertical drag', () => {
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

  it('DominoRenderer initializes with clean selection state', () => {
    const renderer = new DominoRenderer();
    renderer.init();
    expect(renderer.selectedTileIdx).toBe(-1);
    expect(renderer.choiceNeeded).toBe(false);
  });
});
