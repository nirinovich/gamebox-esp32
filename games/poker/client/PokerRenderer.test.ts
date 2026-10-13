import { describe, it, expect, vi } from 'vitest';
import { PokerRenderer, PokerState } from './PokerRenderer';

function createMockCtx(): CanvasRenderingContext2D {
  const target: any = {
    createRadialGradient: vi.fn(() => ({ addColorStop: vi.fn() })),
    fillRect: vi.fn(),
    strokeRect: vi.fn(),
    fillText: vi.fn(),
    beginPath: vi.fn(),
    arc: vi.fn(),
    ellipse: vi.fn(),
    fill: vi.fn(),
    stroke: vi.fn(),
    save: vi.fn(),
    restore: vi.fn(),
    roundRect: vi.fn(),
    moveTo: vi.fn(),
    lineTo: vi.fn(),
    measureText: vi.fn(() => ({ width: 60 })),
    setLineDash: vi.fn(),
  };
  return new Proxy(target, {
    get(t, prop) {
      if (prop in t) return t[prop];
      t[prop] = vi.fn();
      return t[prop];
    },
  }) as unknown as CanvasRenderingContext2D;
}

describe('PokerRenderer', () => {
  const mockState: PokerState = {
    game: 'poker',
    street: 'PRE_FLOP',
    pot: 60,
    current_bet: 20,
    min_raise: 20,
    current_turn: 1,
    dealer_seat: 2,
    turn_time_left: 19.5,
    community_cards: [],
    players: [
      {
        id: 1,
        name: 'Player 1',
        chips: 980,
        bet: 10,
        folded: false,
        all_in: false,
        is_bot: false,
        cards: [
          { rank: 14, suit: 3, code: 'As' },
          { rank: 13, suit: 2, code: 'Kh' },
        ],
      },
      {
        id: 2,
        name: 'Bot 1',
        chips: 960,
        bet: 20,
        folded: false,
        all_in: false,
        is_bot: true,
        cards: [{ hidden: true }, { hidden: true }],
      },
    ],
  };

  it('renders felt table, community card slots, and player seats without throwing', () => {
    const renderer = new PokerRenderer();
    renderer.init(undefined, undefined, undefined, 1);
    renderer.resize(800, 600);
    const mockCtx = createMockCtx();

    expect(() => renderer.render(mockState, mockCtx)).not.toThrow();
  });

  it('dispatches fold command when fold button is clicked', () => {
    const dispatchSpy = vi.fn();
    const renderer = new PokerRenderer();
    renderer.init(undefined, undefined, dispatchSpy, 1);
    renderer.resize(800, 600);
    const mockCtx = createMockCtx();

    renderer.render(mockState, mockCtx);

    // Click in betting area for fold button
    // The betting bar is at bottom (y ~ 544..586)
    // Buttons: Fold (x ~ 205..305), Call (x ~ 315..415), Raise (x ~ 425..525), All-In (x ~ 535..635)
    renderer.handlePointerDown(250, 565);

    expect(dispatchSpy).toHaveBeenCalledWith({ cmd: 'fold' });
  });

  it('dispatches call command when call button is clicked', () => {
    const dispatchSpy = vi.fn();
    const renderer = new PokerRenderer();
    renderer.init(undefined, undefined, dispatchSpy, 1);
    renderer.resize(800, 600);
    const mockCtx = createMockCtx();

    renderer.render(mockState, mockCtx);

    // Click Call button (second button)
    renderer.handlePointerDown(360, 565);

    expect(dispatchSpy).toHaveBeenCalledWith({ cmd: 'call' });
  });

  it('dispatches raise command when raise button is clicked', () => {
    const dispatchSpy = vi.fn();
    const renderer = new PokerRenderer();
    renderer.init(undefined, undefined, dispatchSpy, 1);
    renderer.resize(800, 600);
    const mockCtx = createMockCtx();

    renderer.render(mockState, mockCtx);

    // Click Raise button (third button)
    renderer.handlePointerDown(470, 565);

    expect(dispatchSpy).toHaveBeenCalledWith({ cmd: 'raise', amount: 40 });
  });

  it('dispatches all-in command when all-in button is clicked', () => {
    const dispatchSpy = vi.fn();
    const renderer = new PokerRenderer();
    renderer.init(undefined, undefined, dispatchSpy, 1);
    renderer.resize(800, 600);
    const mockCtx = createMockCtx();

    renderer.render(mockState, mockCtx);

    // Click All-in button (fourth button)
    renderer.handlePointerDown(580, 565);

    expect(dispatchSpy).toHaveBeenCalledWith({ cmd: 'all_in' });
  });

  it('renders next hand button and dispatches next_hand at HAND_OVER', () => {
    const dispatchSpy = vi.fn();
    const renderer = new PokerRenderer();
    renderer.init(undefined, undefined, dispatchSpy, 1);
    renderer.resize(800, 600);
    const mockCtx = createMockCtx();

    const handOverState: PokerState = {
      ...mockState,
      street: 'HAND_OVER',
      winners: [{ id: 1, payout: 60, hand_rank: 'Pair of Aces' }],
    };

    renderer.render(handOverState, mockCtx);

    // Click Next hand button in center of betting bar
    renderer.handlePointerDown(400, 565);

    expect(dispatchSpy).toHaveBeenCalledWith({ cmd: 'next_hand' });
  });
});
