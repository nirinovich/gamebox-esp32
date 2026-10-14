import { describe, it, expect, vi } from 'vitest';
import { FanoronaRenderer } from './FanoronaRenderer';

describe('FanoronaRenderer', () => {
  const createMockCtx = () => ({
    createRadialGradient: () => ({ addColorStop: () => {} }),
    fillRect: () => {},
    strokeRect: () => {},
    beginPath: () => {},
    roundRect: () => {},
    fill: () => {},
    stroke: () => {},
    moveTo: () => {},
    lineTo: () => {},
    arc: () => {},
    save: () => {},
    restore: () => {},
    fillText: () => {},
    setLineDash: () => {}
  } as unknown as CanvasRenderingContext2D);

  it('initializes with clean selection and dual-choice state', () => {
    const renderer = new FanoronaRenderer();
    renderer.init();
    expect(renderer.selectedNode).toBe(-1);
    expect(renderer.pendingChoice).toBeNull();
    expect(renderer.movingStone).toBeNull();
    expect(renderer.capturedStones).toEqual([]);
    expect(renderer.sparks).toEqual([]);
  });

  it('correctly identifies strong vs weak nodes in traditional Fanorona grid', () => {
    const renderer = new FanoronaRenderer();
    // Center (2, 4) -> 2+4=6 is even (strong)
    expect(renderer.isStrongNode(2, 4)).toBe(true);
    // Corner (0, 0) -> 0+0=0 is even (strong)
    expect(renderer.isStrongNode(0, 0)).toBe(true);
    // (0, 1) -> 0+1=1 is odd (weak)
    expect(renderer.isStrongNode(0, 1)).toBe(false);
  });

  it('validates graph connections including diagonal restrictions', () => {
    const renderer = new FanoronaRenderer();
    // Corner (0,0) connects orthogonally to (0,1) [node 1] and diagonally to (1,1) [node 10]
    expect(renderer.isConnected(0, 1)).toBe(true);
    expect(renderer.isConnected(0, 10)).toBe(true);

    // Weak node (0,1) [node 1] does NOT connect diagonally to (1,0) [node 9]
    expect(renderer.isConnected(1, 9)).toBe(false);
  });

  it('detects approach captures correctly', () => {
    const renderer = new FanoronaRenderer();
    const board = new Array(45).fill(0);
    // Player 1 at 20 (row 2, col 2), moving to 21 (row 2, col 3)
    // Enemy (player 2) at 22 (row 2, col 4) and 23 (row 2, col 5)
    board[20] = 1;
    board[22] = 2;
    board[23] = 2;

    const app = renderer.getApproachCaptures(20, 21, board);
    expect(app).toEqual([22, 23]);
    const wit = renderer.getWithdrawalCaptures(20, 21, board);
    expect(wit).toEqual([]);
  });

  it('triggers dual choice modal when a move offers both approach and withdrawal captures', () => {
    const renderer = new FanoronaRenderer();
    const dispatchMock = vi.fn();
    renderer.init(undefined, undefined, dispatchMock, 1);
    renderer.resize(450, 320);

    const board = new Array(45).fill(0);
    // Player 1 at 20, moving to 21
    // Enemy at 22 (approach) and enemy at 19 (withdrawal)
    board[20] = 1;
    board[22] = 2;
    board[19] = 2;

    const state = {
      game: 'fanorona',
      turn: 1,
      finished: false,
      board
    };

    const dummyCtx = createMockCtx();
    renderer.render(state, dummyCtx);

    // Select stone at 20
    renderer.selectedNode = 20;

    const app = renderer.getApproachCaptures(20, 21, board);
    const wit = renderer.getWithdrawalCaptures(20, 21, board);
    expect(app.length).toBeGreaterThan(0);
    expect(wit.length).toBeGreaterThan(0);
  });

  it('starts move animation and triggers captures with sparks', () => {
    const renderer = new FanoronaRenderer();
    renderer.init();
    renderer.resize(450, 320);

    renderer.startMoveAnimation(20, 21, 1, [22]);
    expect(renderer.movingStone).not.toBeNull();
    expect(renderer.movingStone?.fromNode).toBe(20);
    expect(renderer.movingStone?.toNode).toBe(21);
    expect(renderer.movingStone?.player).toBe(1);

    renderer.triggerCaptures([22], 2);
    expect(renderer.capturedStones.length).toBe(1);
    expect(renderer.capturedStones[0].node).toBe(22);
    expect(renderer.capturedStones[0].player).toBe(2);
    expect(renderer.sparks.length).toBeGreaterThan(0);
  });

  it('animates move when new last_move arrives from server', () => {
    const renderer = new FanoronaRenderer();
    renderer.init();
    renderer.resize(450, 320);

    const dummyCtx = createMockCtx();

    // Initial state (e.g. opening bot move)
    const initialState = {
      game: 'fanorona',
      turn: 1,
      finished: false,
      last_move: { id: 1, player: 2, from: 31, to: 22, type: '', captured: [] },
      board: new Array(45).fill(0)
    };
    renderer.render(initialState, dummyCtx);
    expect(renderer.movingStone).toBeNull(); // Initial state should not replay past move

    // Opponent executes move 2
    const nextState = {
      game: 'fanorona',
      turn: 2,
      finished: false,
      last_move: { id: 2, player: 1, from: 20, to: 21, type: 'approach', captured: [22] },
      board: new Array(45).fill(0)
    };
    renderer.render(nextState, dummyCtx);

    expect(renderer.movingStone).not.toBeNull();
    expect(renderer.movingStone?.fromNode).toBe(20);
    expect(renderer.movingStone?.toNode).toBe(21);
    expect(renderer.movingStone?.player).toBe(1);
    expect(renderer.movingStone?.captured).toEqual([22]);
  });

  it('allows voluntarily ending a multi-capture series', () => {
    const renderer = new FanoronaRenderer();
    const dispatchMock = vi.fn();
    renderer.init(undefined, undefined, dispatchMock, 1);
    renderer.resize(450, 320);

    const state = {
      game: 'fanorona',
      turn: 1,
      finished: false,
      in_multi: true,
      active_piece: 20,
      board: new Array(45).fill(0)
    };
    const dummyCtx = createMockCtx();
    renderer.render(state, dummyCtx);

    // Click the turn completion button (centered at width/2 = 225, height-32 = 288)
    renderer.handlePointerDown(225, 295);
    expect(dispatchMock).toHaveBeenCalledWith({ cmd: 'end_turn' });
  });

  it('handles 1v1 duel opening move from Player 2 (claires)', () => {
    const renderer = new FanoronaRenderer();
    const dispatchMock = vi.fn();
    renderer.init(undefined, undefined, dispatchMock, 2);
    renderer.resize(400, 400);

    const initialBoard = [
      1,1,1,1,1,1,1,1,1,
      1,1,1,1,1,1,1,1,1,
      2,1,2,1,0,2,1,2,1,
      2,2,2,2,2,2,2,2,2,
      2,2,2,2,2,2,2,2,2
    ];

    const state = {
      game: 'fanorona',
      turn: 2,
      finished: false,
      board: initialBoard
    };

    const dummyCtx = createMockCtx();
    renderer.render(state, dummyCtx);

    const layout = renderer.getBoardLayout();
    const x31 = layout.startX + 4 * layout.step;
    const y31 = layout.startY + 3 * layout.step;

    // Click own stone at 31
    renderer.handlePointerDown(x31, y31);
    expect(renderer.selectedNode).toBe(31);

    // Click destination at 22
    const x22 = layout.startX + 4 * layout.step;
    const y22 = layout.startY + 2 * layout.step;
    renderer.handlePointerDown(x22, y22);

    expect(dispatchMock).toHaveBeenCalledWith({
      cmd: 'move',
      from: 31,
      to: 22,
      captureType: 'approach'
    });
  });

  it('handles 1v1 duel opening move from Player 1 (Host / foncées)', () => {
    const renderer = new FanoronaRenderer();
    const dispatchMock = vi.fn();
    renderer.init(undefined, undefined, dispatchMock, 1);
    renderer.resize(400, 400);

    const initialBoard = [
      1,1,1,1,1,1,1,1,1,
      1,1,1,1,1,1,1,1,1,
      2,1,2,1,0,2,1,2,1,
      2,2,2,2,2,2,2,2,2,
      2,2,2,2,2,2,2,2,2
    ];

    const state = {
      game: 'fanorona',
      turn: 1,
      finished: false,
      must_capture: true,
      board: initialBoard
    };

    const dummyCtx = createMockCtx();
    renderer.render(state, dummyCtx);

    const layout = renderer.getBoardLayout();
    const x13 = layout.startX + 4 * layout.step;
    const y13 = layout.startY + 1 * layout.step;

    // Click own stone at 13 (r=1, c=4)
    renderer.handlePointerDown(x13, y13);
    expect(renderer.selectedNode).toBe(13);

    // Click destination at 22 (r=2, c=4)
    const x22 = layout.startX + 4 * layout.step;
    const y22 = layout.startY + 2 * layout.step;
    renderer.handlePointerDown(x22, y22);

    expect(dispatchMock).toHaveBeenCalledWith({
      cmd: 'move',
      from: 13,
      to: 22,
      captureType: 'approach'
    });
  });

  it('rejects selecting non-capturing stone when must_capture is active', () => {
    const renderer = new FanoronaRenderer();
    const dispatchMock = vi.fn();
    renderer.init(undefined, undefined, dispatchMock, 1);
    renderer.resize(400, 400);

    const initialBoard = [
      1,1,1,1,1,1,1,1,1,
      1,1,1,1,1,1,1,1,1,
      2,1,2,1,0,2,1,2,1,
      2,2,2,2,2,2,2,2,2,
      2,2,2,2,2,2,2,2,2
    ];

    const state = {
      game: 'fanorona',
      turn: 1,
      finished: false,
      must_capture: true,
      board: initialBoard
    };

    const dummyCtx = createMockCtx();
    renderer.render(state, dummyCtx);

    const layout = renderer.getBoardLayout();
    // Node 0 (corner): has no legal captures
    const x0 = layout.startX;
    const y0 = layout.startY;

    renderer.handlePointerDown(x0, y0);
    expect(renderer.selectedNode).toBe(-1);
    expect(renderer.captureNotice).toContain('Capture obligatoire');
  });
});
