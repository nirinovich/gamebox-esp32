import { IGameRenderer, ActionDispatcher } from '../../../frontend/src/renderers/IGameRenderer';
import { sound, haptic } from '../../../frontend/src/audio/SoundEngine';

export class SnakeRenderer implements IGameRenderer {
  private width = 400;
  private height = 400;
  private touchStartX = 0;
  private touchStartY = 0;
  private swipeDone = false;
  private dispatch: ActionDispatcher = () => {};

  public init(_canvas?: HTMLCanvasElement, _ctx?: CanvasRenderingContext2D, dispatch?: ActionDispatcher) {
    if (dispatch) this.dispatch = dispatch;
    this.swipeDone = false;
  }

  public setDispatcher(dispatch: ActionDispatcher) {
    this.dispatch = dispatch;
  }

  public resize(width: number, height: number) {
    this.width = width;
    this.height = height;
  }

  public render(state: any, ctx: CanvasRenderingContext2D) {
    const grid = 20;
    const sz = this.width / grid;

    ctx.fillStyle = '#070b14';
    ctx.fillRect(0, 0, this.width, this.height);

    // Grid lines
    ctx.strokeStyle = '#111827';
    ctx.lineWidth = 1;
    for (let i = 0; i <= grid; i++) {
      ctx.beginPath();
      ctx.moveTo(i * sz, 0);
      ctx.lineTo(i * sz, this.height);
      ctx.stroke();

      ctx.beginPath();
      ctx.moveTo(0, i * sz);
      ctx.lineTo(this.width, i * sz);
      ctx.stroke();
    }

    // Food with glowing pulse core & embers
    if (state?.food) {
      const fx = state.food[0] * sz + sz / 2;
      const fy = state.food[1] * sz + sz / 2;
      const pulse = 1 + Math.sin(Date.now() / 150) * 0.15;
      ctx.save();
      ctx.shadowBlur = 12;
      ctx.shadowColor = '#ef4444';
      ctx.fillStyle = '#ef4444';
      ctx.beginPath();
      ctx.arc(fx, fy, (sz / 2 - 2) * pulse, 0, Math.PI * 2);
      ctx.fill();
      ctx.fillStyle = '#fca5a5';
      ctx.beginPath();
      ctx.arc(fx - 1, fy - 1, (sz / 4) * pulse, 0, Math.PI * 2);
      ctx.fill();
      ctx.restore();
    }

    if (state?.body && state.body.length > 0) {
      this.renderSnakeBody(ctx, state.body, '#38bdf8', '#0284c7', sz);
    }
    if (state?.body2 && state.body2.length > 0) {
      this.renderSnakeBody(ctx, state.body2, '#ec4899', '#be185d', sz);
    }
  }

  private renderSnakeBody(
    ctx: CanvasRenderingContext2D,
    body: number[][],
    headColor: string,
    bodyColor: string,
    sz: number
  ) {
    body.forEach((seg, idx) => {
      const rx = seg[0] * sz + 1;
      const ry = seg[1] * sz + 1;
      const rsz = sz - 2;

      ctx.fillStyle = idx === 0 ? headColor : bodyColor;
      ctx.beginPath();
      ctx.rect(rx, ry, rsz, rsz);
      ctx.fill();

      // Directional Robotic Eyes on Head
      if (idx === 0) {
        const hx = rx + rsz / 2;
        const hy = ry + rsz / 2;
        let dx = 1;
        let dy = 0;
        if (body.length > 1) {
          dx = seg[0] - body[1][0];
          dy = seg[1] - body[1][1];
        }
        const eyeOff = 4;
        const perpX = -dy * 3;
        const perpY = dx * 3;
        ctx.fillStyle = '#ffffff';
        ctx.beginPath();
        ctx.arc(hx + dx * eyeOff + perpX, hy + dy * eyeOff + perpY, 2, 0, Math.PI * 2);
        ctx.arc(hx + dx * eyeOff - perpX, hy + dy * eyeOff - perpY, 2, 0, Math.PI * 2);
        ctx.fill();
      }
    });
  }

  public handlePointerDown(x: number, y: number) {
    this.touchStartX = x;
    this.touchStartY = y;
    this.swipeDone = false;
  }

  public handlePointerMove(x: number, y: number) {
    if (this.swipeDone) return;
    const dx = x - this.touchStartX;
    const dy = y - this.touchStartY;
    if (Math.abs(dx) > 15 || Math.abs(dy) > 15) {
      let dir = 'RIGHT';
      if (Math.abs(dx) > Math.abs(dy)) {
        dir = dx > 0 ? 'RIGHT' : 'LEFT';
      } else {
        dir = dy > 0 ? 'DOWN' : 'UP';
      }
      haptic(15);
      sound.move();
      this.dispatch({ cmd: 'dir', val: dir });
      this.swipeDone = true;
      this.touchStartX = x;
      this.touchStartY = y;
    }
  }

  public handlePointerUp(_x: number, _y: number) {
    this.swipeDone = false;
  }

  public destroy() {}
}
