import { IGameRenderer, ActionDispatcher } from './IGameRenderer';
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
  private currentPaddleY = 200;
  private lastSentCmd = '';
  private myPlayerNum = 1;
  private dispatch: ActionDispatcher = () => {};

  public init(_canvas?: HTMLCanvasElement, _ctx?: CanvasRenderingContext2D, dispatch?: ActionDispatcher, playerNum?: number) {
    if (playerNum) this.myPlayerNum = playerNum;
    if (dispatch) this.dispatch = dispatch;
    this.trails = [];
    this.isDragging = false;
    this.currentPaddleY = 200;
    this.lastSentCmd = '';
  }

  public setPlayerNum(playerNum: number) {
    this.myPlayerNum = playerNum;
  }

  public setDispatcher(dispatch: ActionDispatcher) {
    this.dispatch = dispatch;
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
    const p1y = state?.p1y || 200;
    const p2y = state?.p2y || 200;

    const myPlayerNum = this.myPlayerNum;
    this.currentPaddleY = myPlayerNum === 2 ? p2y : p1y;

    ctx.fillStyle = '#38bdf8';
    ctx.fillRect(padW, p1y - padH / 2, padW, padH);

    ctx.fillStyle = '#ec4899';
    ctx.fillRect(this.width - padW * 2, p2y - padH / 2, padW, padH);

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
      this.lastSentCmd = 'STOP';
      this.dispatch({ cmd: 'dir', val: 'STOP' });
    }
  }

  private processPaddleMovement(y: number) {
    const targetY = Math.max(30, Math.min(370, y));
    const deadzone = 14;
    let nextCmd = 'STOP';

    if (targetY < this.currentPaddleY - deadzone) {
      nextCmd = 'UP';
    } else if (targetY > this.currentPaddleY + deadzone) {
      nextCmd = 'DOWN';
    } else {
      nextCmd = 'STOP';
    }

    if (nextCmd !== this.lastSentCmd) {
      this.lastSentCmd = nextCmd;
      this.dispatch({ cmd: 'dir', val: nextCmd });
      haptic(10);
      sound.move();
    }
  }

  public destroy() {
    this.trails = [];
  }
}
