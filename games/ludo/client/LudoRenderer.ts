import { IGameRenderer, ActionDispatcher } from '../../../frontend/src/renderers/IGameRenderer';
import { sound, haptic } from '../../../frontend/src/audio/SoundEngine';

export interface PawnData {
  id: number;
  state: 'IN_YARD' | 'ON_TRACK' | 'IN_HOME' | 'FINISHED';
  step: number;
  track_cell: number;
}

export interface LudoPlayerData {
  id: number;
  color: string;
  name: string;
  is_bot: boolean;
  start_tile: number;
  finished_count: number;
  pawns: PawnData[];
}

export interface LudoState {
  game: string;
  is_quick: boolean;
  pawn_count: number;
  current_turn: number;
  dice: number;
  dice_rolled: boolean;
  consecutive_sixes: number;
  turn_time_left: number;
  finished: boolean;
  winner: number;
  last_action: string;
  players: LudoPlayerData[];
  legal_moves: number[];
}

interface PawnTargetArea {
  pawnId: number;
  x: number;
  y: number;
  radius: number;
}

interface RollButtonArea {
  x: number;
  y: number;
  w: number;
  h: number;
}

export class LudoRenderer implements IGameRenderer {
  private dispatch: ActionDispatcher | null = null;
  private playerNum: number = 1;
  private width: number = 800;
  private height: number = 600;

  private pawnTargets: PawnTargetArea[] = [];
  private rollBtnArea: RollButtonArea | null = null;

  // 52 Track cells coordinates on 15x15 grid: [col, row]
  private static readonly TRACK_CELLS: Array<[number, number]> = [
    [1, 6], [2, 6], [3, 6], [4, 6], [5, 6],
    [6, 5], [6, 4], [6, 3], [6, 2], [6, 1], [6, 0],
    [7, 0],
    [8, 0], [8, 1], [8, 2], [8, 3], [8, 4], [8, 5],
    [9, 6], [10, 6], [11, 6], [12, 6], [13, 6], [14, 6],
    [14, 7],
    [14, 8], [13, 8], [12, 8], [11, 8], [10, 8], [9, 8],
    [8, 9], [8, 10], [8, 11], [8, 12], [8, 13], [8, 14],
    [7, 14],
    [6, 14], [6, 13], [6, 12], [6, 11], [6, 10], [6, 9],
    [5, 8], [4, 8], [3, 8], [2, 8], [1, 8], [0, 8],
    [0, 7], [0, 6]
  ];

  // 8 Safe Squares (indices on track): 0, 8, 13, 21, 26, 34, 39, 47
  private static readonly SAFE_CELLS = new Set([0, 8, 13, 21, 26, 34, 39, 47]);

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
    this.pawnTargets = [];
    this.rollBtnArea = null;
  }

  public render(state: LudoState, ctx: CanvasRenderingContext2D): void {
    if (!state) return;
    this.pawnTargets = [];
    this.rollBtnArea = null;

    const w = this.width;
    const h = this.height;

    // 1. Background
    ctx.save();
    ctx.fillStyle = '#0f172a';
    ctx.fillRect(0, 0, w, h);

    // Board geometry
    const availableH = h - 80; // Reserve HUD top & bottom
    const availableW = w;
    const boardSize = Math.min(availableW - 20, availableH - 20);
    const cellSize = boardSize / 15;
    const boardX = (w - boardSize) / 2;
    const boardY = (h - boardSize) / 2 + 10;

    // 2. Draw 15x15 Ludo Board
    this.drawBoardGrid(ctx, boardX, boardY, cellSize);
    this.drawYards(ctx, boardX, boardY, cellSize);
    this.drawHomeStretches(ctx, boardX, boardY, cellSize);
    this.drawSafeStars(ctx, boardX, boardY, cellSize);

    // 3. Draw Pawns
    this.drawPawns(ctx, state, boardX, boardY, cellSize);

    // 4. Draw Header HUD & Turn Info
    this.drawHeaderHUD(ctx, state, w);

    // 5. Draw Dice Roll Control (Bottom HUD)
    this.drawDiceControls(ctx, state, w, h);

    ctx.restore();
  }

  private drawBoardGrid(
    ctx: CanvasRenderingContext2D,
    bx: number,
    by: number,
    cs: number
  ): void {
    ctx.save();
    // Board shadow
    ctx.fillStyle = '#ffffff';
    ctx.shadowColor = 'rgba(0, 0, 0, 0.5)';
    ctx.shadowBlur = 16;
    ctx.shadowOffsetY = 8;
    ctx.fillRect(bx, by, cs * 15, cs * 15);
    ctx.shadowColor = 'transparent';

    // Cell outlines
    ctx.strokeStyle = '#e2e8f0';
    ctx.lineWidth = 1;

    for (const [col, row] of LudoRenderer.TRACK_CELLS) {
      const cx = bx + col * cs;
      const cy = by + row * cs;
      ctx.strokeRect(cx, cy, cs, cs);
    }
    ctx.restore();
  }

  private drawYards(
    ctx: CanvasRenderingContext2D,
    bx: number,
    by: number,
    cs: number
  ): void {
    ctx.save();
    const yardColors = [
      { color: '#ef4444', col: 0, row: 0 },   // Red: Top-Left
      { color: '#22c55e', col: 9, row: 0 },   // Green: Top-Right
      { color: '#eab308', col: 9, row: 9 },   // Yellow: Bottom-Right
      { color: '#3b82f6', col: 0, row: 9 },   // Blue: Bottom-Left
    ];

    for (const y of yardColors) {
      const yx = bx + y.col * cs;
      const yy = by + y.row * cs;
      const size = cs * 6;

      ctx.fillStyle = y.color;
      ctx.fillRect(yx, yy, size, size);

      // Inner white area
      ctx.fillStyle = '#ffffff';
      ctx.fillRect(yx + cs, yy + cs, cs * 4, cs * 4);

      ctx.strokeStyle = '#0f172a';
      ctx.lineWidth = 2;
      ctx.strokeRect(yx, yy, size, size);
      ctx.strokeRect(yx + cs, yy + cs, cs * 4, cs * 4);
    }

    // Center 3x3 Home Goal Triangles
    const centerCol = 6;
    const centerRow = 6;
    const cx = bx + centerCol * cs;
    const cy = by + centerRow * cs;
    const cSize = cs * 3;
    const midX = cx + cSize / 2;
    const midY = cy + cSize / 2;

    // Red triangle (Left)
    ctx.fillStyle = '#ef4444';
    ctx.beginPath();
    ctx.moveTo(cx, cy);
    ctx.lineTo(midX, midY);
    ctx.lineTo(cx, cy + cSize);
    ctx.closePath();
    ctx.fill();

    // Green triangle (Top)
    ctx.fillStyle = '#22c55e';
    ctx.beginPath();
    ctx.moveTo(cx, cy);
    ctx.lineTo(midX, midY);
    ctx.lineTo(cx + cSize, cy);
    ctx.closePath();
    ctx.fill();

    // Yellow triangle (Right)
    ctx.fillStyle = '#eab308';
    ctx.beginPath();
    ctx.moveTo(cx + cSize, cy);
    ctx.lineTo(midX, midY);
    ctx.lineTo(cx + cSize, cy + cSize);
    ctx.closePath();
    ctx.fill();

    // Blue triangle (Bottom)
    ctx.fillStyle = '#3b82f6';
    ctx.beginPath();
    ctx.moveTo(cx, cy + cSize);
    ctx.lineTo(midX, midY);
    ctx.lineTo(cx + cSize, cy + cSize);
    ctx.closePath();
    ctx.fill();

    ctx.restore();
  }

  private drawHomeStretches(
    ctx: CanvasRenderingContext2D,
    bx: number,
    by: number,
    cs: number
  ): void {
    ctx.save();
    // Red: row 7, cols 1..5
    ctx.fillStyle = '#fca5a5';
    for (let c = 1; c <= 5; ++c) {
      ctx.fillRect(bx + c * cs, by + 7 * cs, cs, cs);
      ctx.strokeRect(bx + c * cs, by + 7 * cs, cs, cs);
    }
    // Red start tile
    ctx.fillStyle = '#ef4444';
    ctx.fillRect(bx + 1 * cs, by + 6 * cs, cs, cs);

    // Green: col 7, rows 1..5
    ctx.fillStyle = '#86efac';
    for (let r = 1; r <= 5; ++r) {
      ctx.fillRect(bx + 7 * cs, by + r * cs, cs, cs);
      ctx.strokeRect(bx + 7 * cs, by + r * cs, cs, cs);
    }
    // Green start tile
    ctx.fillStyle = '#22c55e';
    ctx.fillRect(bx + 8 * cs, by + 1 * cs, cs, cs);

    // Yellow: row 7, cols 9..13
    ctx.fillStyle = '#fef08a';
    for (let c = 9; c <= 13; ++c) {
      ctx.fillRect(bx + c * cs, by + 7 * cs, cs, cs);
      ctx.strokeRect(bx + c * cs, by + 7 * cs, cs, cs);
    }
    // Yellow start tile
    ctx.fillStyle = '#eab308';
    ctx.fillRect(bx + 13 * cs, by + 8 * cs, cs, cs);

    // Blue: col 7, rows 9..13
    ctx.fillStyle = '#93c5fd';
    for (let r = 9; r <= 13; ++r) {
      ctx.fillRect(bx + 7 * cs, by + r * cs, cs, cs);
      ctx.strokeRect(bx + 7 * cs, by + r * cs, cs, cs);
    }
    // Blue start tile
    ctx.fillStyle = '#3b82f6';
    ctx.fillRect(bx + 6 * cs, by + 13 * cs, cs, cs);

    ctx.restore();
  }

  private drawSafeStars(
    ctx: CanvasRenderingContext2D,
    bx: number,
    by: number,
    cs: number
  ): void {
    ctx.save();
    ctx.font = `bold ${Math.floor(cs * 0.7)}px system-ui, sans-serif`;
    ctx.textAlign = 'center';
    ctx.fillStyle = '#64748b';

    for (const cellIdx of LudoRenderer.SAFE_CELLS) {
      const [col, row] = LudoRenderer.TRACK_CELLS[cellIdx];
      const cx = bx + col * cs + cs / 2;
      const cy = by + row * cs + cs * 0.75;
      ctx.fillText('★', cx, cy);
    }
    ctx.restore();
  }

  private getPawnGridCoords(
    playerIdx: number,
    pawn: PawnData
  ): [number, number] {
    if (pawn.state === 'IN_YARD') {
      const yardOffsets = [
        [2, 2], [3.5, 2], [2, 3.5], [3.5, 3.5]
      ];
      const offset = yardOffsets[pawn.id % 4];
      if (playerIdx === 0) return [offset[0], offset[1]];           // Red: TL
      if (playerIdx === 1) return [9 + offset[0], offset[1]];       // Green: TR
      if (playerIdx === 2) return [9 + offset[0], 9 + offset[1]];   // Yellow: BR
      return [offset[0], 9 + offset[1]];                           // Blue: BL
    }

    if (pawn.state === 'ON_TRACK') {
      const trackIdx = pawn.track_cell;
      if (trackIdx >= 0 && trackIdx < 52) {
        return LudoRenderer.TRACK_CELLS[trackIdx];
      }
    }

    if (pawn.state === 'IN_HOME') {
      const homeStep = pawn.step - 51; // 0..4
      if (playerIdx === 0) return [1 + homeStep, 7];
      if (playerIdx === 1) return [7, 1 + homeStep];
      if (playerIdx === 2) return [13 - homeStep, 7];
      return [7, 13 - homeStep];
    }

    // Finished
    return [7, 7];
  }

  private drawPawns(
    ctx: CanvasRenderingContext2D,
    state: LudoState,
    bx: number,
    by: number,
    cs: number
  ): void {
    const isLocalTurn = state.current_turn === this.playerNum && state.dice_rolled;
    const legalSet = new Set(state.legal_moves || []);
    const colors = ['#ef4444', '#22c55e', '#eab308', '#3b82f6'];

    for (let plIdx = 0; plIdx < (state.players || []).length; ++plIdx) {
      const pl = state.players[plIdx];
      const pColor = colors[plIdx % 4];

      for (let pIdx = 0; pIdx < pl.pawns.length; ++pIdx) {
        const pawn = pl.pawns[pIdx];
        if (pawn.state === 'FINISHED') continue;

        const [col, row] = this.getPawnGridCoords(plIdx, pawn);
        const px = bx + col * cs + cs / 2;
        const py = by + row * cs + cs / 2;
        const radius = cs * 0.4;

        const isLegal = pl.id === this.playerNum && isLocalTurn && legalSet.has(pawn.id);

        ctx.save();
        // Glowing ring for legal selectable pawn
        if (isLegal) {
          ctx.beginPath();
          ctx.arc(px, py, radius + 5, 0, Math.PI * 2);
          ctx.fillStyle = 'rgba(250, 204, 21, 0.4)';
          ctx.fill();
          ctx.lineWidth = 3;
          ctx.strokeStyle = '#facc15';
          ctx.stroke();

          // Register hit-target for tapping
          this.pawnTargets.push({
            pawnId: pawn.id,
            x: px,
            y: py,
            radius: radius + 8,
          });
        }

        // Pawn base disc
        ctx.beginPath();
        ctx.arc(px, py, radius, 0, Math.PI * 2);
        ctx.fillStyle = pColor;
        ctx.shadowColor = 'rgba(0, 0, 0, 0.4)';
        ctx.shadowBlur = 6;
        ctx.shadowOffsetY = 3;
        ctx.fill();
        ctx.shadowColor = 'transparent';

        ctx.lineWidth = 2;
        ctx.strokeStyle = '#ffffff';
        ctx.stroke();

        // Inner crown ring
        ctx.beginPath();
        ctx.arc(px, py, radius * 0.5, 0, Math.PI * 2);
        ctx.fillStyle = 'rgba(255, 255, 255, 0.4)';
        ctx.fill();

        ctx.restore();
      }
    }
  }

  private drawHeaderHUD(
    ctx: CanvasRenderingContext2D,
    state: LudoState,
    w: number
  ): void {
    ctx.save();
    ctx.fillStyle = 'rgba(15, 23, 42, 0.85)';
    ctx.fillRect(0, 0, w, 44);

    const activePlayer = (state.players || []).find((p) => p.id === state.current_turn);
    const pName = activePlayer ? activePlayer.name : `Player ${state.current_turn}`;
    const modeName = state.is_quick ? '⚡ Quick (2 Pawns)' : '👑 Classic (4 Pawns)';

    ctx.font = 'bold 14px system-ui, sans-serif';
    ctx.fillStyle = '#f8fafc';
    ctx.textAlign = 'left';
    ctx.fillText(modeName, 16, 26);

    ctx.textAlign = 'center';
    if (state.finished) {
      const winner = (state.players || []).find((p) => p.id === state.winner);
      ctx.fillStyle = '#facc15';
      ctx.fillText(`🏆 ${winner ? winner.name : 'Player'} Won!`, w / 2, 26);
    } else {
      ctx.fillStyle = '#38bdf8';
      const timeLeft = Math.max(0, Math.ceil(state.turn_time_left || 0));
      ctx.fillText(`${pName}'s Turn (${timeLeft}s)`, w / 2, 26);
    }

    ctx.textAlign = 'right';
    ctx.font = '12px system-ui, sans-serif';
    ctx.fillStyle = '#94a3b8';
    ctx.fillText(state.last_action || '', w - 16, 26);
    ctx.restore();
  }

  private drawDiceControls(
    ctx: CanvasRenderingContext2D,
    state: LudoState,
    w: number,
    h: number
  ): void {
    ctx.save();
    const isLocalTurn = state.current_turn === this.playerNum;

    // Bottom control bar
    ctx.fillStyle = '#0f172a';
    ctx.fillRect(0, h - 60, w, 60);
    ctx.strokeStyle = '#334155';
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.moveTo(0, h - 60);
    ctx.lineTo(w, h - 60);
    ctx.stroke();

    const cx = w / 2;
    const cy = h - 30;

    if (!state.dice_rolled && isLocalTurn && !state.finished) {
      // Big "ROLL DICE" Button
      const btnW = 140;
      const btnH = 40;
      const bx = cx - btnW / 2;
      const by = cy - btnH / 2;

      ctx.fillStyle = '#10b981';
      ctx.beginPath();
      ctx.roundRect(bx, by, btnW, btnH, 8);
      ctx.fill();

      ctx.lineWidth = 2;
      ctx.strokeStyle = '#34d399';
      ctx.stroke();

      ctx.font = 'bold 15px system-ui, sans-serif';
      ctx.fillStyle = '#ffffff';
      ctx.textAlign = 'center';
      ctx.fillText('ROLL DICE 🎲', cx, cy + 5);

      this.rollBtnArea = { x: bx, y: by, w: btnW, h: btnH };
    } else if (state.dice > 0) {
      // 3D-styled dice cube
      const diceSize = 40;
      const dx = cx - diceSize / 2;
      const dy = cy - diceSize / 2;

      ctx.fillStyle = '#f8fafc';
      ctx.shadowColor = 'rgba(0, 0, 0, 0.4)';
      ctx.shadowBlur = 8;
      ctx.shadowOffsetY = 4;
      ctx.beginPath();
      ctx.roundRect(dx, dy, diceSize, diceSize, 8);
      ctx.fill();
      ctx.shadowColor = 'transparent';

      ctx.strokeStyle = '#cbd5e1';
      ctx.lineWidth = 1.5;
      ctx.stroke();

      // Draw Pips
      this.drawDicePips(ctx, dx, dy, diceSize, state.dice);

      // Subtitle helper
      ctx.font = 'bold 12px system-ui, sans-serif';
      ctx.fillStyle = isLocalTurn ? '#facc15' : '#94a3b8';
      ctx.textAlign = 'center';
      const prompt = isLocalTurn ? 'Tap a glowing pawn to move!' : `Rolled a ${state.dice}`;
      ctx.fillText(prompt, cx + 80, cy + 4);
    }

    ctx.restore();
  }

  private drawDicePips(
    ctx: CanvasRenderingContext2D,
    x: number,
    y: number,
    size: number,
    pips: number
  ): void {
    ctx.save();
    ctx.fillStyle = '#0f172a';
    const r = size * 0.09;
    const l = x + size * 0.25;
    const m = x + size * 0.5;
    const rt = x + size * 0.75;
    const t = y + size * 0.25;
    const my = y + size * 0.5;
    const b = y + size * 0.75;

    const drawPip = (px: number, py: number) => {
      ctx.beginPath();
      ctx.arc(px, py, r, 0, Math.PI * 2);
      ctx.fill();
    };

    if (pips % 2 === 1) drawPip(m, my); // 1, 3, 5 center
    if (pips >= 2) { drawPip(l, t); drawPip(rt, b); } // 2, 3, 4, 5, 6
    if (pips >= 4) { drawPip(rt, t); drawPip(l, b); } // 4, 5, 6
    if (pips === 6) { drawPip(l, my); drawPip(rt, my); } // 6 middle
    ctx.restore();
  }

  public handlePointerDown(x: number, y: number): void {
    // 1. Check Roll Button
    if (this.rollBtnArea) {
      const b = this.rollBtnArea;
      if (x >= b.x && x <= b.x + b.w && y >= b.y && y <= b.y + b.h) {
        sound.click();
        haptic(15);
        this.dispatchAction({ cmd: 'roll' });
        return;
      }
    }

    // 2. Check Pawn Targets
    for (const t of this.pawnTargets) {
      const dx = x - t.x;
      const dy = y - t.y;
      if (dx * dx + dy * dy <= t.radius * t.radius) {
        sound.move();
        haptic(20);
        this.dispatchAction({ cmd: 'move', pawn: t.pawnId });
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
