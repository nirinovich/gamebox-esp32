import { IGameRenderer } from './IGameRenderer';
import { gameSocket } from '../services/GameSocket';
import { sound, haptic } from '../audio/SoundEngine';

export class TicTacToeRenderer implements IGameRenderer {
  private width = 400;
  private height = 400;

  public init() {}

  public resize(width: number, height: number) {
    this.width = width;
    this.height = height;
  }

  public render(state: any, ctx: CanvasRenderingContext2D) {
    const sz = this.width / 3;

    ctx.fillStyle = '#070b14';
    ctx.fillRect(0, 0, this.width, this.height);

    ctx.strokeStyle = '#1e293b';
    ctx.lineWidth = 3;
    for (let i = 1; i < 3; i++) {
      ctx.beginPath();
      ctx.moveTo(i * sz, 24);
      ctx.lineTo(i * sz, this.height - 24);
      ctx.stroke();

      ctx.beginPath();
      ctx.moveTo(24, i * sz);
      ctx.lineTo(this.width - 24, i * sz);
      ctx.stroke();
    }

    if (state?.board) {
      state.board.forEach((val: number, idx: number) => {
        const row = Math.floor(idx / 3);
        const col = idx % 3;
        const cx = col * sz + sz / 2;
        const cy = row * sz + sz / 2;

        if (val === 1) {
          ctx.save();
          ctx.strokeStyle = '#38bdf8';
          ctx.shadowBlur = 8;
          ctx.shadowColor = '#38bdf8';
          ctx.lineWidth = 6;
          ctx.beginPath();
          ctx.moveTo(cx - 28, cy - 28);
          ctx.lineTo(cx + 28, cy + 28);
          ctx.moveTo(cx + 28, cy - 28);
          ctx.lineTo(cx - 28, cy + 28);
          ctx.stroke();
          ctx.restore();
        } else if (val === 2) {
          ctx.save();
          ctx.strokeStyle = '#ec4899';
          ctx.shadowBlur = 8;
          ctx.shadowColor = '#ec4899';
          ctx.lineWidth = 6;
          ctx.beginPath();
          ctx.arc(cx, cy, 30, 0, Math.PI * 2);
          ctx.stroke();
          ctx.restore();
        }
      });

      // Laser Beam across Winning Mark (Issue 07)
      if (state.finished && state.winner > 0) {
        const b = state.board;
        const wins = [
          [0, 1, 2],
          [3, 4, 5],
          [6, 7, 8],
          [0, 3, 6],
          [1, 4, 7],
          [2, 5, 8],
          [0, 4, 8],
          [2, 4, 6]
        ];
        for (const [p0, p1, p2] of wins) {
          if (b[p0] === state.winner && b[p1] === state.winner && b[p2] === state.winner) {
            const x0 = (p0 % 3) * sz + sz / 2;
            const y0 = Math.floor(p0 / 3) * sz + sz / 2;
            const x2 = (p2 % 3) * sz + sz / 2;
            const y2 = Math.floor(p2 / 3) * sz + sz / 2;
            ctx.save();
            ctx.strokeStyle = state.winner === 1 ? '#38bdf8' : '#ec4899';
            ctx.shadowBlur = 18;
            ctx.shadowColor = ctx.strokeStyle;
            ctx.lineWidth = 8;
            ctx.beginPath();
            ctx.moveTo(x0, y0);
            ctx.lineTo(x2, y2);
            ctx.stroke();
            ctx.strokeStyle = '#ffffff';
            ctx.lineWidth = 3;
            ctx.beginPath();
            ctx.moveTo(x0, y0);
            ctx.lineTo(x2, y2);
            ctx.stroke();
            ctx.restore();
            break;
          }
        }
      }
    }
  }

  public handlePointerDown(x: number, y: number) {
    const col = Math.floor(x / (this.width / 3));
    const row = Math.floor(y / (this.height / 3));
    if (col >= 0 && col < 3 && row >= 0 && row < 3) {
      const cell = row * 3 + col;
      sound.place();
      haptic(15);
      gameSocket.send({ cmd: 'place', cell });
    }
  }

  public handlePointerMove(_x: number, _y: number) {}
  public handlePointerUp(_x: number, _y: number) {}
  public destroy() {}
}
