import { describe, it, expect, vi } from 'vitest';
import { LudoRenderer, LudoState } from './LudoRenderer';

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
    closePath: vi.fn(),
    measureText: vi.fn(() => ({ width: 60 })),
  };
  return new Proxy(target, {
    get(t, prop) {
      if (prop in t) return t[prop];
      t[prop] = vi.fn();
      return t[prop];
    },
  }) as unknown as CanvasRenderingContext2D;
}

describe('LudoRenderer', () => {
  const mockState: LudoState = {
    game: 'ludo',
    is_quick: true,
    pawn_count: 2,
    current_turn: 1,
    dice: 6,
    dice_rolled: false,
    consecutive_sixes: 0,
    turn_time_left: 19.8,
    finished: false,
    winner: 0,
    last_action: 'Player 1 to roll',
    players: [
      {
        id: 1,
        color: 'red',
        name: 'Player 1',
        is_bot: false,
        start_tile: 0,
        finished_count: 0,
        pawns: [
          { id: 0, state: 'IN_YARD', step: -1, track_cell: -1 },
          { id: 1, state: 'IN_YARD', step: -1, track_cell: -1 },
        ],
      },
      {
        id: 2,
        color: 'green',
        name: 'Bot 1',
        is_bot: true,
        start_tile: 13,
        finished_count: 0,
        pawns: [
          { id: 0, state: 'IN_YARD', step: -1, track_cell: -1 },
          { id: 1, state: 'IN_YARD', step: -1, track_cell: -1 },
        ],
      },
    ],
    legal_moves: [],
  };

  it('renders Ludo board, yards, stars, and pawns without throwing', () => {
    const renderer = new LudoRenderer();
    renderer.init(undefined, undefined, undefined, 1);
    renderer.resize(800, 600);
    const mockCtx = createMockCtx();

    expect(() => renderer.render(mockState, mockCtx)).not.toThrow();
  });

  it('dispatches roll command when ROLL DICE button is clicked', () => {
    const dispatchSpy = vi.fn();
    const renderer = new LudoRenderer();
    renderer.init(undefined, undefined, dispatchSpy, 1);
    renderer.resize(800, 600);
    const mockCtx = createMockCtx();

    renderer.render(mockState, mockCtx);

    // Roll button is centered at bottom (cx = 400, cy = 570)
    renderer.handlePointerDown(400, 570);

    expect(dispatchSpy).toHaveBeenCalledWith({ cmd: 'roll' });
  });

  it('dispatches move command when a highlighted legal pawn is clicked', () => {
    const dispatchSpy = vi.fn();
    const renderer = new LudoRenderer();
    renderer.init(undefined, undefined, dispatchSpy, 1);
    renderer.resize(800, 600);
    const mockCtx = createMockCtx();

    // State where dice has been rolled to 6, and pawn 0 is legal to move
    const rolledState: LudoState = {
      ...mockState,
      dice: 6,
      dice_rolled: true,
      legal_moves: [0],
    };

    renderer.render(rolledState, mockCtx);

    // Red Yard pawn 0 position is top-left yard:
    // With 800x600, boardSize = 500, cellSize = 33.33, boardX = 150, boardY = 60
    // Red pawn 0 is at col 2, row 2 -> x = 150 + 2.5 * 33.33 = 233, y = 60 + 2.5 * 33.33 = 143
    renderer.handlePointerDown(233, 143);

    expect(dispatchSpy).toHaveBeenCalledWith({ cmd: 'move', pawn: 0 });
  });
});
