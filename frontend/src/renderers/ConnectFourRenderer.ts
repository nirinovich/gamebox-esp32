import { IGameRenderer, ActionDispatcher } from './IGameRenderer';
import { sound, haptic } from '../audio/SoundEngine';
import { CameraJuice } from './CameraJuice';

interface DroppingChip {
  col: number;
  targetY: number;
  currentY: number;
  vy: number;
  gravity: number;
  bounce: number;
  val: number;
  targetRow: number;
}

export class ConnectFourRenderer implements IGameRenderer {
  private width = 400;
  private height = 400;
  private droppingChips: DroppingChip[] = [];
  private prevBoard: number[] | null = null;
  private hoveredCol = -1;
  private juice: CameraJuice;
  private dispatch: ActionDispatcher = () => {};

  constructor(juice: CameraJuice) {
    this.juice = juice;
  }

  public init(_canvas?: HTMLCanvasElement, _ctx?: CanvasRenderingContext2D, dispatch?: ActionDispatcher) {
    if (dispatch) this.dispatch = dispatch;
    this.droppingChips = [];
    this.prevBoard = null;
  }

  public setDispatcher(dispatch: ActionDispatcher) {
    this.dispatch = dispatch;
  }

  public resize(width: number, height: number) {
    this.width = width;
    this.height = height;
  }

  public render(state: any, ctx: CanvasRenderingContext2D) {
    const cols = 7;
    const rows = 6;
    const cw = this.width / cols;
    const ch = this.height / rows;

    ctx.fillStyle = '#0b1329';
    ctx.fillRect(0, 0, this.width, this.height);

    // Hover Column Targeting Beam (Issue 07)
    if (this.hoveredCol >= 0 && this.hoveredCol < cols && !state?.finished) {
      ctx.fillStyle = 'rgba(56, 189, 248, 0.08)';
      ctx.fillRect(this.hoveredCol * cw, 0, cw, this.height);
      ctx.fillStyle = 'rgba(56, 189, 248, 0.35)';
      ctx.fillRect(this.hoveredCol * cw, 0, cw, 4);
    }

    // Detect new falling chips
    if (state?.board && this.prevBoard && this.prevBoard.length === state.board.length) {
      for (let r = 0; r < rows; r++) {
        for (let c = 0; c < cols; c++) {
          const idx = r * cols + c;
          if (this.prevBoard[idx] === 0 && state.board[idx] !== 0) {
            this.droppingChips.push({
              col: c,
              targetY: r * ch + ch / 2,
              currentY: -ch,
              vy: 0,
              gravity: 1.8,
              bounce: 0.35,
              val: state.board[idx],
              targetRow: r
            });
            sound.place();
            haptic(15);
          }
        }
      }
    }
    this.prevBoard = state?.board ? [...state.board] : null;

    // Static Board Discs
    if (state?.board) {
      for (let r = 0; r < rows; r++) {
        for (let c = 0; c < cols; c++) {
          const idx = r * cols + c;
          const val = state.board[idx];
          const cx = c * cw + cw / 2;
          const cy = r * ch + ch / 2;
          const radius = Math.min(cw, ch) * 0.38;

          ctx.fillStyle = '#070b14';
          ctx.beginPath();
          ctx.arc(cx, cy, radius, 0, Math.PI * 2);
          ctx.fill();

          const isAnimating = this.droppingChips.some(d => d.col === c && d.targetRow === r);
          if (!isAnimating && val > 0) {
            ctx.fillStyle = val === 1 ? '#38bdf8' : '#ec4899';
            ctx.beginPath();
            ctx.arc(cx, cy, radius - 2, 0, Math.PI * 2);
            ctx.fill();
          }
        }
      }
    }

    // Animate Dropping Chips
    for (let i = this.droppingChips.length - 1; i >= 0; i--) {
      const d = this.droppingChips[i];
      d.vy += d.gravity;
      d.currentY += d.vy;
      if (d.currentY >= d.targetY) {
        d.currentY = d.targetY;
        d.vy = -d.vy * d.bounce;
        if (Math.abs(d.vy) < 1.2) {
          this.droppingChips.splice(i, 1);
          this.juice.shake(2);
          sound.bounce();
          continue;
        }
      }
      const cx = d.col * cw + cw / 2;
      const radius = Math.min(cw, ch) * 0.38;
      ctx.fillStyle = d.val === 1 ? '#38bdf8' : '#ec4899';
      ctx.beginPath();
      ctx.arc(cx, d.currentY, radius - 2, 0, Math.PI * 2);
      ctx.fill();
    }
  }

  public handlePointerDown(x: number, _y: number) {
    const col = Math.floor(x / (this.width / 7));
    if (col >= 0 && col < 7) {
      this.dispatch({ cmd: 'drop', col });
    }
  }

  public handlePointerMove(x: number, _y: number) {
    this.hoveredCol = Math.floor(x / (this.width / 7));
  }

  public handlePointerUp(_x: number, _y: number) {
    this.hoveredCol = -1;
  }

  public destroy() {
    this.droppingChips = [];
  }
}
