import { IGameRenderer, ActionDispatcher } from '../../../frontend/src/renderers/IGameRenderer';
import { sound, haptic } from '../../../frontend/src/audio/SoundEngine';

export interface CardData {
  rank?: number;
  suit?: number;
  code?: string;
  hidden?: boolean;
}

export interface PlayerData {
  id: number;
  name: string;
  chips: number;
  bet: number;
  total_bet?: number;
  folded: boolean;
  all_in: boolean;
  is_bot: boolean;
  cards: CardData[];
  hand_rank?: string;
}

export interface WinnerData {
  id: number;
  payout: number;
  hand_rank: string;
}

export interface PokerState {
  game: string;
  street: string;
  hand_number?: number;
  pot: number;
  current_bet: number;
  min_raise: number;
  current_turn: number;
  dealer_seat: number;
  turn_time_left: number;
  is_solo?: boolean;
  finished?: boolean;
  winner?: number;
  last_action?: string;
  community_cards: CardData[];
  players: PlayerData[];
  winners?: WinnerData[];
}

interface ButtonArea {
  id: string;
  x: number;
  y: number;
  w: number;
  h: number;
  label: string;
  color: string;
  enabled: boolean;
  action: () => void;
}

export class PokerRenderer implements IGameRenderer {
  private dispatch: ActionDispatcher | null = null;
  private playerNum: number = 1;
  private width: number = 800;
  private height: number = 600;
  private buttons: ButtonArea[] = [];
  private raiseAmount: number = 40;

  public init(
    _canvas?: HTMLCanvasElement,
    _ctx?: CanvasRenderingContext2D,
    dispatch?: ActionDispatcher,
    playerNum?: number
  ): void {
    if (dispatch) this.dispatch = dispatch;
    if (playerNum) this.playerNum = playerNum;
  }

  public setPlayerNum(num: number): void {
    this.playerNum = num;
  }

  public setIsSolo(_solo: boolean): void {}

  public setDispatcher(dispatch: ActionDispatcher): void {
    this.dispatch = dispatch;
  }

  public resize(w: number, h: number): void {
    this.width = Math.max(320, w);
    this.height = Math.max(320, h);
  }

  public destroy(): void {
    this.buttons = [];
  }

  public render(state: PokerState, ctx: CanvasRenderingContext2D): void {
    if (!state) return;
    this.buttons = [];

    const w = this.width;
    const h = this.height;

    // 1. Dark felt background
    ctx.save();
    ctx.fillStyle = '#0b1610';
    ctx.fillRect(0, 0, w, h);

    // 2. Felt Poker Table Oval
    const tableMargin = Math.min(w, h) * 0.05;
    const tableX = tableMargin;
    const tableY = tableMargin + 20;
    const tableW = w - tableMargin * 2;
    const tableH = h - tableMargin * 2 - 80; // reserve bottom for betting controls

    this.drawFeltTable(ctx, tableX, tableY, tableW, tableH);

    // 3. Center Pot & Street HUD
    const centerX = tableX + tableW / 2;
    const centerY = tableY + tableH / 2;
    this.drawTableCenter(ctx, state, centerX, centerY);

    // 4. Community Cards Tray
    this.drawCommunityCards(ctx, state.community_cards || [], centerX, centerY - 15);

    // 5. Seats Layout
    const players = state.players || [];
    this.drawPlayerSeats(ctx, state, players, tableX, tableY, tableW, tableH);

    // 6. Winner / Showdown Banner
    if (state.street === 'HAND_OVER' || state.street === 'SHOWDOWN') {
      this.drawWinnerBanner(ctx, state, centerX, centerY + 65);
    }

    // 7. Betting Controls Bar (at bottom)
    this.drawBettingBar(ctx, state, w, h);

    ctx.restore();
  }

  private drawFeltTable(
    ctx: CanvasRenderingContext2D,
    x: number,
    y: number,
    w: number,
    h: number
  ): void {
    // Outer leather rail
    ctx.save();
    ctx.beginPath();
    ctx.ellipse(x + w / 2, y + h / 2, w / 2, h / 2, 0, 0, Math.PI * 2);
    ctx.fillStyle = '#1c130d';
    ctx.shadowColor = 'rgba(0, 0, 0, 0.6)';
    ctx.shadowBlur = 20;
    ctx.shadowOffsetY = 10;
    ctx.fill();

    // Wood/Gold inner ring
    ctx.lineWidth = 6;
    ctx.strokeStyle = '#cda869';
    ctx.stroke();

    // Inner Green Felt
    ctx.beginPath();
    ctx.ellipse(x + w / 2, y + h / 2, w / 2 - 14, h / 2 - 14, 0, 0, Math.PI * 2);
    const feltGrad = ctx.createRadialGradient(
      x + w / 2,
      y + h / 2,
      20,
      x + w / 2,
      y + h / 2,
      w / 2
    );
    feltGrad.addColorStop(0, '#1c5e38');
    feltGrad.addColorStop(0.7, '#144629');
    feltGrad.addColorStop(1, '#0e311d');
    ctx.fillStyle = feltGrad;
    ctx.fill();

    // Subtle table rim line
    ctx.lineWidth = 2;
    ctx.strokeStyle = 'rgba(255, 255, 255, 0.1)';
    ctx.stroke();
    ctx.restore();
  }

  private drawTableCenter(
    ctx: CanvasRenderingContext2D,
    state: PokerState,
    cx: number,
    cy: number
  ): void {
    ctx.save();
    // Street badge
    ctx.font = 'bold 11px system-ui, sans-serif';
    ctx.textAlign = 'center';
    ctx.fillStyle = 'rgba(205, 168, 105, 0.8)';
    const streetName = (state.street || '').replace('_', ' ');
    ctx.fillText(streetName, cx, cy - 75);

    // Pot Pill Badge
    const potText = `POT: ${state.pot || 0} 🪙`;
    ctx.font = 'bold 16px system-ui, sans-serif';
    const potWidth = ctx.measureText(potText).width + 30;

    ctx.fillStyle = 'rgba(10, 25, 18, 0.85)';
    ctx.strokeStyle = '#cda869';
    ctx.lineWidth = 1.5;
    ctx.beginPath();
    ctx.roundRect(cx - potWidth / 2, cy - 105, potWidth, 26, 13);
    ctx.fill();
    ctx.stroke();

    ctx.fillStyle = '#ffdf7a';
    ctx.fillText(potText, cx, cy - 87);

    // Current Bet subtitle if active
    if (state.current_bet > 0 && state.street !== 'HAND_OVER') {
      ctx.font = '11px system-ui, sans-serif';
      ctx.fillStyle = 'rgba(255, 255, 255, 0.7)';
      ctx.fillText(`To Call: ${state.current_bet}`, cx, cy - 60);
    }

    ctx.restore();
  }

  private drawCommunityCards(
    ctx: CanvasRenderingContext2D,
    cards: CardData[],
    cx: number,
    cy: number
  ): void {
    const cardW = 44;
    const cardH = 64;
    const gap = 8;
    const totalW = 5 * cardW + 4 * gap;
    const startX = cx - totalW / 2;

    for (let i = 0; i < 5; ++i) {
      const x = startX + i * (cardW + gap);
      const y = cy - cardH / 2;

      if (i < cards.length && cards[i] && !cards[i].hidden) {
        this.drawCardFace(ctx, x, y, cardW, cardH, cards[i]);
      } else {
        // Empty placeholder slot
        ctx.save();
        ctx.strokeStyle = 'rgba(205, 168, 105, 0.25)';
        ctx.lineWidth = 1.5;
        ctx.setLineDash([4, 4]);
        ctx.fillStyle = 'rgba(0, 0, 0, 0.15)';
        ctx.beginPath();
        ctx.roundRect(x, y, cardW, cardH, 5);
        ctx.fill();
        ctx.stroke();
        ctx.restore();
      }
    }
  }

  private drawCardFace(
    ctx: CanvasRenderingContext2D,
    x: number,
    y: number,
    w: number,
    h: number,
    card: CardData
  ): void {
    ctx.save();
    // Card base
    ctx.fillStyle = '#f8fafc';
    ctx.shadowColor = 'rgba(0, 0, 0, 0.4)';
    ctx.shadowBlur = 6;
    ctx.shadowOffsetY = 3;
    ctx.beginPath();
    ctx.roundRect(x, y, w, h, 5);
    ctx.fill();
    ctx.shadowColor = 'transparent';

    ctx.strokeStyle = '#cbd5e1';
    ctx.lineWidth = 1;
    ctx.stroke();

    const isRed = card.suit === 1 || card.suit === 2; // diamonds or hearts
    const suitSymbols = ['♣', '♦', '♥', '♠'];
    const suitSymbol = suitSymbols[card.suit ?? 0] || '♠';
    const rankStrs: Record<number, string> = {
      10: '10',
      11: 'J',
      12: 'Q',
      13: 'K',
      14: 'A',
    };
    const rankStr = (card.rank && rankStrs[card.rank]) || String(card.rank || '');

    ctx.fillStyle = isRed ? '#dc2626' : '#0f172a';

    // Top-left rank & mini suit
    ctx.font = 'bold 12px system-ui, sans-serif';
    ctx.textAlign = 'left';
    ctx.fillText(rankStr, x + 4, y + 14);
    ctx.font = '10px system-ui, sans-serif';
    ctx.fillText(suitSymbol, x + 4, y + 25);

    // Center big suit
    ctx.font = '22px system-ui, sans-serif';
    ctx.textAlign = 'center';
    ctx.fillText(suitSymbol, x + w / 2, y + h / 2 + 8);

    ctx.restore();
  }

  private drawCardBack(
    ctx: CanvasRenderingContext2D,
    x: number,
    y: number,
    w: number,
    h: number
  ): void {
    ctx.save();
    ctx.fillStyle = '#1e3a8a';
    ctx.shadowColor = 'rgba(0, 0, 0, 0.4)';
    ctx.shadowBlur = 6;
    ctx.shadowOffsetY = 3;
    ctx.beginPath();
    ctx.roundRect(x, y, w, h, 5);
    ctx.fill();
    ctx.shadowColor = 'transparent';

    // Crosshatch / border pattern
    ctx.strokeStyle = '#93c5fd';
    ctx.lineWidth = 1.5;
    ctx.beginPath();
    ctx.roundRect(x + 3, y + 3, w - 6, h - 6, 3);
    ctx.stroke();

    ctx.strokeStyle = 'rgba(147, 197, 253, 0.4)';
    ctx.beginPath();
    ctx.moveTo(x + 3, y + 3);
    ctx.lineTo(x + w - 3, y + h - 3);
    ctx.moveTo(x + w - 3, y + 3);
    ctx.lineTo(x + 3, y + h - 3);
    ctx.stroke();

    ctx.restore();
  }

  private drawPlayerSeats(
    ctx: CanvasRenderingContext2D,
    state: PokerState,
    players: PlayerData[],
    tx: number,
    ty: number,
    tw: number,
    th: number
  ): void {
    const numPlayers = players.length;
    if (numPlayers === 0) return;

    // Compute seat positions relative to table oval
    const cx = tx + tw / 2;
    const cy = ty + th / 2;
    const rx = tw * 0.44;
    const ry = th * 0.42;

    for (let i = 0; i < numPlayers; ++i) {
      const p = players[i];
      let angle = Math.PI / 2; // Default bottom for P1

      if (numPlayers === 2) {
        angle = p.id === this.playerNum ? Math.PI / 2 : -Math.PI / 2;
      } else if (numPlayers === 3) {
        if (p.id === this.playerNum) angle = Math.PI / 2;
        else if (i === 1) angle = -Math.PI * 0.75;
        else angle = -Math.PI * 0.25;
      } else {
        // 4 players
        if (p.id === this.playerNum) angle = Math.PI / 2; // bottom
        else if (i === 1) angle = Math.PI; // left
        else if (i === 2) angle = -Math.PI / 2; // top
        else angle = 0; // right
      }

      const seatX = cx + Math.cos(angle) * rx;
      const seatY = cy + Math.sin(angle) * ry;

      this.drawSeatBadge(ctx, state, p, seatX, seatY);
    }
  }

  private drawSeatBadge(
    ctx: CanvasRenderingContext2D,
    state: PokerState,
    player: PlayerData,
    x: number,
    y: number
  ): void {
    ctx.save();
    const isLocal = player.id === this.playerNum;
    const isTurn = state.current_turn === player.id && state.street !== 'HAND_OVER';
    const isDealer = state.dealer_seat === player.id;

    // Active turn pulsing glow
    if (isTurn) {
      ctx.beginPath();
      ctx.arc(x, y, 32, 0, Math.PI * 2);
      ctx.fillStyle = 'rgba(234, 179, 8, 0.25)';
      ctx.fill();
      ctx.lineWidth = 3;
      ctx.strokeStyle = '#eab308';
      ctx.stroke();

      // Countdown text
      const timeLeft = Math.max(0, Math.ceil(state.turn_time_left || 0));
      ctx.font = 'bold 10px system-ui, sans-serif';
      ctx.fillStyle = '#fef08a';
      ctx.textAlign = 'center';
      ctx.fillText(`${timeLeft}s`, x, y - 36);
    }

    // Avatar pill
    ctx.beginPath();
    ctx.arc(x, y, 22, 0, Math.PI * 2);
    ctx.fillStyle = player.folded ? '#334155' : isLocal ? '#0369a1' : '#1e293b';
    ctx.fill();
    ctx.lineWidth = 2;
    ctx.strokeStyle = isLocal ? '#38bdf8' : '#94a3b8';
    ctx.stroke();

    // Avatar text (Initial or Bot)
    ctx.font = 'bold 12px system-ui, sans-serif';
    ctx.fillStyle = '#ffffff';
    ctx.textAlign = 'center';
    ctx.fillText(player.is_bot ? '🤖' : player.name.slice(0, 2).toUpperCase(), x, y + 4);

    // Dealer button
    if (isDealer) {
      ctx.beginPath();
      ctx.arc(x + 24, y - 16, 9, 0, Math.PI * 2);
      ctx.fillStyle = '#ffffff';
      ctx.fill();
      ctx.lineWidth = 1.5;
      ctx.strokeStyle = '#dc2626';
      ctx.stroke();
      ctx.font = 'bold 9px system-ui, sans-serif';
      ctx.fillStyle = '#0f172a';
      ctx.fillText('D', x + 24, y - 13);
    }

    // Player Name & Chips
    ctx.font = 'bold 11px system-ui, sans-serif';
    ctx.fillStyle = player.folded ? '#94a3b8' : '#f8fafc';
    const displayName = isLocal ? `${player.name} (You)` : player.name;
    ctx.fillText(displayName, x, y + 36);

    ctx.font = 'bold 12px system-ui, sans-serif';
    ctx.fillStyle = '#facc15';
    ctx.fillText(`${player.chips} 🪙`, x, y + 50);

    // Status / Bet badge
    if (player.folded) {
      ctx.font = 'italic 10px system-ui, sans-serif';
      ctx.fillStyle = '#ef4444';
      ctx.fillText('FOLDED', x, y + 63);
    } else if (player.all_in) {
      ctx.font = 'bold 10px system-ui, sans-serif';
      ctx.fillStyle = '#ec4899';
      ctx.fillText('ALL-IN', x, y + 63);
    } else if (player.bet > 0) {
      ctx.font = '10px system-ui, sans-serif';
      ctx.fillStyle = '#86efac';
      ctx.fillText(`Bet: ${player.bet}`, x, y + 63);
    }

    // Hole Cards Rendering next to seat
    const cardW = 34;
    const cardH = 50;
    const cards = player.cards || [];
    const cardY = isLocal ? y - 65 : y - 25;
    const cardStartX = x + 30;

    for (let c = 0; c < 2; ++c) {
      const cx = cardStartX + c * (cardW + 4);
      if (c < cards.length && !cards[c].hidden && cards[c].rank) {
        this.drawCardFace(ctx, cx, cardY, cardW, cardH, cards[c]);
      } else if (!player.folded) {
        this.drawCardBack(ctx, cx, cardY, cardW, cardH);
      }
    }

    // Hand rank string badge
    if (player.hand_rank && (!player.folded || state.street === 'HAND_OVER')) {
      ctx.font = 'bold 10px system-ui, sans-serif';
      ctx.fillStyle = '#67e8f9';
      ctx.fillText(player.hand_rank, x, y + (isLocal ? -72 : 75));
    }

    ctx.restore();
  }

  private drawWinnerBanner(
    ctx: CanvasRenderingContext2D,
    state: PokerState,
    cx: number,
    cy: number
  ): void {
    const winners = state.winners || [];
    if (winners.length === 0) return;

    ctx.save();
    const win = winners[0];
    const player = (state.players || []).find((p) => p.id === win.id);
    const winName = player ? player.name : `Player ${win.id}`;
    const text = `🏆 ${winName} won ${win.payout} chips! (${win.hand_rank})`;

    ctx.font = 'bold 14px system-ui, sans-serif';
    const tw = ctx.measureText(text).width + 36;

    ctx.fillStyle = 'rgba(15, 23, 42, 0.9)';
    ctx.strokeStyle = '#eab308';
    ctx.lineWidth = 2;
    ctx.beginPath();
    ctx.roundRect(cx - tw / 2, cy - 14, tw, 30, 8);
    ctx.fill();
    ctx.stroke();

    ctx.fillStyle = '#fef08a';
    ctx.textAlign = 'center';
    ctx.fillText(text, cx, cy + 6);
    ctx.restore();
  }

  private drawBettingBar(
    ctx: CanvasRenderingContext2D,
    state: PokerState,
    w: number,
    h: number
  ): void {
    const localPlayer = (state.players || []).find((p) => p.id === this.playerNum);
    const isMyTurn = state.current_turn === this.playerNum;
    const isHandOver = state.street === 'HAND_OVER';

    // Bar container
    ctx.save();
    ctx.fillStyle = '#0f172a';
    ctx.fillRect(0, h - 70, w, 70);
    ctx.strokeStyle = '#334155';
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.moveTo(0, h - 70);
    ctx.lineTo(w, h - 70);
    ctx.stroke();

    if (isHandOver) {
      // Show "Next Hand" button
      const btnW = 160;
      const btnH = 44;
      const btnX = w / 2 - btnW / 2;
      const btnY = h - 57;

      this.registerButton({
        id: 'next_hand',
        x: btnX,
        y: btnY,
        w: btnW,
        h: btnH,
        label: 'Next Hand ▶',
        color: '#10b981',
        enabled: true,
        action: () => this.dispatchAction({ cmd: 'next_hand' }),
      });
      this.drawButton(ctx, this.buttons[this.buttons.length - 1]);
      ctx.restore();
      return;
    }

    if (!localPlayer || localPlayer.folded || localPlayer.all_in) {
      // Waiting message
      ctx.font = '13px system-ui, sans-serif';
      ctx.fillStyle = '#94a3b8';
      ctx.textAlign = 'center';
      const msg = localPlayer?.folded
        ? 'You folded. Waiting for round to complete...'
        : localPlayer?.all_in
        ? 'You are All-In! Watching showdown...'
        : 'Waiting for players...';
      ctx.fillText(msg, w / 2, h - 30);
      ctx.restore();
      return;
    }

    const canCheck = localPlayer.bet >= state.current_bet;
    const toCall = Math.max(0, state.current_bet - localPlayer.bet);
    const callLabel = canCheck ? 'Check' : `Call ${toCall}`;
    const minRaise = state.min_raise || 20;
    const currentRaiseTarget = Math.max(state.current_bet + minRaise, this.raiseAmount);

    const btnW = Math.min(100, (w - 60) / 4);
    const btnH = 42;
    const startX = (w - (btnW * 4 + 30)) / 2;
    const btnY = h - 56;

    // 1. Fold Button
    this.registerButton({
      id: 'fold',
      x: startX,
      y: btnY,
      w: btnW,
      h: btnH,
      label: 'Fold',
      color: '#ef4444',
      enabled: isMyTurn,
      action: () => this.dispatchAction({ cmd: 'fold' }),
    });

    // 2. Check / Call Button
    this.registerButton({
      id: 'call',
      x: startX + btnW + 10,
      y: btnY,
      w: btnW,
      h: btnH,
      label: callLabel,
      color: '#22c55e',
      enabled: isMyTurn,
      action: () => this.dispatchAction({ cmd: canCheck ? 'check' : 'call' }),
    });

    // 3. Raise Button
    this.registerButton({
      id: 'raise',
      x: startX + (btnW + 10) * 2,
      y: btnY,
      w: btnW,
      h: btnH,
      label: `Raise ${currentRaiseTarget}`,
      color: '#3b82f6',
      enabled: isMyTurn && localPlayer.chips > toCall,
      action: () => this.dispatchAction({ cmd: 'raise', amount: currentRaiseTarget }),
    });

    // 4. All-In Button
    this.registerButton({
      id: 'all_in',
      x: startX + (btnW + 10) * 3,
      y: btnY,
      w: btnW,
      h: btnH,
      label: 'All-In 💥',
      color: '#a855f7',
      enabled: isMyTurn && localPlayer.chips > 0,
      action: () => this.dispatchAction({ cmd: 'all_in' }),
    });

    for (const b of this.buttons) {
      this.drawButton(ctx, b);
    }

    ctx.restore();
  }

  private registerButton(btn: ButtonArea): void {
    this.buttons.push(btn);
  }

  private drawButton(ctx: CanvasRenderingContext2D, btn: ButtonArea): void {
    ctx.save();
    ctx.fillStyle = btn.enabled ? btn.color : '#334155';
    ctx.beginPath();
    ctx.roundRect(btn.x, btn.y, btn.w, btn.h, 6);
    ctx.fill();

    ctx.strokeStyle = btn.enabled ? 'rgba(255, 255, 255, 0.3)' : 'transparent';
    ctx.lineWidth = 1;
    ctx.stroke();

    ctx.font = 'bold 12px system-ui, sans-serif';
    ctx.fillStyle = btn.enabled ? '#ffffff' : '#64748b';
    ctx.textAlign = 'center';
    ctx.fillText(btn.label, btn.x + btn.w / 2, btn.y + btn.h / 2 + 4);
    ctx.restore();
  }

  public handlePointerDown(x: number, y: number): void {
    for (const btn of this.buttons) {
      if (
        btn.enabled &&
        x >= btn.x &&
        x <= btn.x + btn.w &&
        y >= btn.y &&
        y <= btn.y + btn.h
      ) {
        sound.click();
        haptic(15);
        btn.action();
        return;
      }
    }
  }

  public handlePointerMove(_x: number, _y: number): void {}
  public handlePointerUp(_x: number, _y: number): void {}

  private dispatchAction(payload: any): void {
    if (this.dispatch) {
      this.dispatch(payload);
    }
  }
}
