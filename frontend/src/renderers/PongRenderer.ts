import { IGameRenderer } from './IGameRenderer';
import { gameSocket } from '../services/GameSocket';
import { sound, haptic } from '../audio/SoundEngine';

interface TrailPoint {
  x: number;
  y: number;
  alpha: number;
}

export class PongRenderer implements IGameRenderer {
  private width = 400;
  private height = 400;
  private trails: TrailPoint[] = [];
  private isDragging = false;

  public init() {
    this.trails = [];
    this.isDragging = false;
  }

  public resize(width: number, height: number) {
    this.width = width;
    this.height = height;
  }

  public render(state: any, ctx: CanvasRenderingContext2D) {
    ctx.fillStyle = '#070b14';
    ctx.fillRect(0, 0, this.width, this.height);

    // Dashed center court line
    ctx.strokeStyle = '#1e293b';
    ctx.lineWidth = 2;
    ctx.setLineDash([6, 6]);
    ctx.beginPath();
    ctx.moveTo(this.width / 2, 0);
    ctx.lineTo(this.width / 2, this.height);
    ctx.stroke();
    ctx.setLineDash([]);

    // Paddles
    const padW = 10;
    const padH = 60;
    ctx.fillStyle = '#38bdf8';
    ctx.fillRect(padW, (state?.p1y || 200) - padH / 2, padW, padH);

    ctx.fillStyle = '#ec4899';
    ctx.fillRect(this.width - padW * 2, (state?.p2y || 200) - padH / 2, padW, padH);

    // Ball Phosphor Comet Trails (Issue 07)
    const bx = state?.bx || 200;
    const by = state?.by || 200;
    this.trails.push({ x: bx, y: by, alpha: 0.8 });
    if (this.trails.length > 7) this.trails.shift();

    for (let i = 0; i < this.trails.length - 1; i++) {
      const t = this.trails[i];
      t.alpha *= 0.82;
      ctx.fillStyle = `rgba(56, 189, 248, ${t.alpha * 0.4})`;
      ctx.beginPath();
      ctx.arc(t.x, t.y, 6 * t.alpha, 0, Math.PI * 2);
      ctx.fill();
    }

    // Ball Core with glowing neon shadow
    ctx.save();
    ctx.fillStyle = '#f1f5f9';
    ctx.shadowBlur = 10;
    ctx.shadowColor = '#38bdf8';
    ctx.beginPath();
    ctx.arc(bx, by, 8, 0, Math.PI * 2);
    ctx.fill();
    ctx.restore();
  }

  public handlePointerDown(_x: number, y: number) {
    this.isDragging = true;
    this.processPaddleMovement(y);
  }

  public handlePointerMove(_x: number, y: number) {
    if (this.isDragging) {
      this.processPaddleMovement(y);
    }
  }

  public handlePointerUp(_x: number, _y: number) {
    if (this.isDragging) {
      this.isDragging = false;
      gameSocket.send({ cmd: 'dir', val: 'STOP' });
    }
  }

  private processPaddleMovement(y: number) {
    const paddleTarget = y;
    // Direct vertical drag
    if (paddleTarget < 200 - 15) {
      gameSocket.send({ cmd: 'dir', val: 'UP' });
    } else if (paddleTarget > 200 + 15) {
      gameSocket.send({ cmd: 'dir', val: 'DOWN' });
    } else {
      gameSocket.send({ cmd: 'dir', val: 'STOP' });
    }
    haptic(10);
    sound.move();
  }

  public destroy() {
    this.trails = [];
  }
}
