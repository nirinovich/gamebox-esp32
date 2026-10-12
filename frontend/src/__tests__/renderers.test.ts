import { describe, it, expect, vi, beforeEach } from 'vitest';
import { TicTacToeRenderer } from '../renderers/TicTacToeRenderer';
import { ConnectFourRenderer } from '../renderers/ConnectFourRenderer';
import { BattleshipRenderer } from '../renderers/BattleshipRenderer';
import { ParticleEngine } from '../renderers/ParticleEngine';
import { CameraJuice } from '../renderers/CameraJuice';
import { gameSocket } from '../services/GameSocket';
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
    const sendSpy = vi.spyOn(gameSocket, 'send');
    const renderer = new TicTacToeRenderer();
    renderer.resize(400, 400);

    renderer.handlePointerDown(50, 50);
    expect(sendSpy).toHaveBeenCalledWith({ cmd: 'place', cell: 0 });

    renderer.handlePointerDown(200, 200);
    expect(sendSpy).toHaveBeenCalledWith({ cmd: 'place', cell: 4 });
  });

  it('ConnectFourRenderer calculates correct column index', () => {
    const sendSpy = vi.spyOn(gameSocket, 'send');
    const juice = new CameraJuice();
    const renderer = new ConnectFourRenderer(juice);
    renderer.resize(400, 400);

    // 400 / 7 ~= 57.14 px per column
    renderer.handlePointerDown(20, 100);
    expect(sendSpy).toHaveBeenCalledWith({ cmd: 'drop', col: 0 });

    renderer.handlePointerDown(380, 100);
    expect(sendSpy).toHaveBeenCalledWith({ cmd: 'drop', col: 6 });
  });

  it('BattleshipRenderer calculates sector coordinates', () => {
    const sendSpy = vi.spyOn(gameSocket, 'send');
    const particles = new ParticleEngine();
    const juice = new CameraJuice();
    const renderer = new BattleshipRenderer(particles, juice);
    renderer.resize(400, 400);

    // 400 / 10 = 40 px per sector
    renderer.handlePointerDown(20, 20);
    expect(sendSpy).toHaveBeenCalledWith({ cmd: 'fire', row: 0, col: 0 });

    renderer.handlePointerDown(100, 180);
    expect(sendSpy).toHaveBeenCalledWith({ cmd: 'fire', row: 4, col: 2 });
  });

  it('SoundEngine executes sound effects safely without errors', () => {
    expect(() => sound.click()).not.toThrow();
    expect(() => sound.laser()).not.toThrow();
    expect(() => sound.place()).not.toThrow();
    expect(() => sound.victory()).not.toThrow();
  });
});
