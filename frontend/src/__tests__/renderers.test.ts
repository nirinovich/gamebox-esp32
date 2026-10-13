import { describe, it, expect, vi, beforeEach } from 'vitest';
import { TicTacToeRenderer } from '../renderers/TicTacToeRenderer';
import { ConnectFourRenderer } from '../renderers/ConnectFourRenderer';
import { BattleshipRenderer } from '../renderers/BattleshipRenderer';
import { ParticleEngine } from '../renderers/ParticleEngine';
import { CameraJuice } from '../renderers/CameraJuice';
import { sound } from '../audio/SoundEngine';

describe('Turn-Based Game Renderers', () => {
  let mockCtx: any;

  beforeEach(() => {
    mockCtx = {
      fillStyle: '',
      strokeStyle: '',
      lineWidth: 0,
      shadowBlur: 0,
      shadowColor: '',
      fillRect: vi.fn(),
      beginPath: vi.fn(),
      moveTo: vi.fn(),
      lineTo: vi.fn(),
      stroke: vi.fn(),
      fill: vi.fn(),
      arc: vi.fn(),
      save: vi.fn(),
      restore: vi.fn(),
      translate: vi.fn(),
      scale: vi.fn(),
      rotate: vi.fn()
    };
  });

  it('TicTacToeRenderer calculates cell 0 on top-left click', () => {
    const dispatchSpy = vi.fn();
    const renderer = new TicTacToeRenderer();
    renderer.setDispatcher(dispatchSpy);
    renderer.resize(400, 400);

    renderer.handlePointerDown(50, 50);
    expect(dispatchSpy).toHaveBeenCalledWith({ cmd: 'place', cell: 0 });

    renderer.handlePointerDown(200, 200);
    expect(dispatchSpy).toHaveBeenCalledWith({ cmd: 'place', cell: 4 });
  });

  it('ConnectFourRenderer calculates correct column index', () => {
    const dispatchSpy = vi.fn();
    const juice = new CameraJuice();
    const renderer = new ConnectFourRenderer(juice);
    renderer.setDispatcher(dispatchSpy);
    renderer.resize(400, 400);

    // 400 / 7 ~= 57.14 px per column
    renderer.handlePointerDown(20, 100);
    expect(dispatchSpy).toHaveBeenCalledWith({ cmd: 'drop', col: 0 });

    renderer.handlePointerDown(380, 100);
    expect(dispatchSpy).toHaveBeenCalledWith({ cmd: 'drop', col: 6 });
  });

  it('BattleshipRenderer calculates sector coordinates', () => {
    const dispatchSpy = vi.fn();
    const particles = new ParticleEngine();
    const juice = new CameraJuice();
    const renderer = new BattleshipRenderer(particles, juice);
    renderer.setDispatcher(dispatchSpy);
    renderer.resize(400, 400);

    // 400 / 10 = 40 px per sector
    renderer.handlePointerDown(20, 20);
    expect(dispatchSpy).toHaveBeenCalledWith({ cmd: 'fire', row: 0, col: 0 });

    renderer.handlePointerDown(100, 180);
    expect(dispatchSpy).toHaveBeenCalledWith({ cmd: 'fire', row: 4, col: 2 });
  });

  it('SoundEngine executes sound effects safely without errors', () => {
    expect(() => sound.click()).not.toThrow();
    expect(() => sound.laser()).not.toThrow();
    expect(() => sound.place()).not.toThrow();
    expect(() => sound.victory()).not.toThrow();
  });

  it('Renderers support isolated ActionDispatcher without socket dependency', () => {
    const customDispatch = vi.fn();
    const renderer = new TicTacToeRenderer();
    renderer.init(undefined, undefined, customDispatch);
    renderer.resize(400, 400);

    renderer.handlePointerDown(50, 50);
    expect(customDispatch).toHaveBeenCalledWith({ cmd: 'place', cell: 0 });
  });
});
