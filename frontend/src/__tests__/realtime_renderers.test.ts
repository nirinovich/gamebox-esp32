import { describe, it, expect, vi } from 'vitest';
import { SnakeRenderer } from '../renderers/SnakeRenderer';
import { PongRenderer } from '../renderers/PongRenderer';
import { TronRenderer } from '../renderers/TronRenderer';
import { DominoRenderer } from '../renderers/DominoRenderer';
import { gameSocket } from '../services/GameSocket';

describe('Real-Time & Tabletop Renderers', () => {
  it('SnakeRenderer sends direction on swipe gesture', () => {
    const sendSpy = vi.spyOn(gameSocket, 'send');
    const renderer = new SnakeRenderer();
    renderer.resize(400, 400);

    renderer.handlePointerDown(100, 100);
    // Swipe Right
    renderer.handlePointerMove(150, 100);
    expect(sendSpy).toHaveBeenCalledWith({ cmd: 'dir', val: 'RIGHT' });
  });

  it('TronRenderer sends direction on swipe gesture', () => {
    const sendSpy = vi.spyOn(gameSocket, 'send');
    const renderer = new TronRenderer();
    renderer.resize(400, 400);

    renderer.handlePointerDown(200, 200);
    // Swipe Down
    renderer.handlePointerMove(200, 250);
    expect(sendSpy).toHaveBeenCalledWith({ cmd: 'dir', val: 'DOWN' });
  });

  it('PongRenderer sends paddle direction on vertical drag', () => {
    const sendSpy = vi.spyOn(gameSocket, 'send');
    const renderer = new PongRenderer();
    renderer.resize(400, 400);

    // Click in upper half (< 185)
    renderer.handlePointerDown(200, 100);
    expect(sendSpy).toHaveBeenCalledWith({ cmd: 'dir', val: 'UP' });

    // Drag to lower half (> 215)
    renderer.handlePointerMove(200, 300);
    expect(sendSpy).toHaveBeenCalledWith({ cmd: 'dir', val: 'DOWN' });

    // Release stops moving
    renderer.handlePointerUp(200, 300);
    expect(sendSpy).toHaveBeenCalledWith({ cmd: 'dir', val: 'STOP' });
  });

  it('DominoRenderer initializes with clean selection state', () => {
    const renderer = new DominoRenderer();
    renderer.init();
    expect(renderer.selectedTileIdx).toBe(-1);
    expect(renderer.choiceNeeded).toBe(false);
  });
});
