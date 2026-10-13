import { IGameRenderer, ActionDispatcher } from './IGameRenderer';
import { sound, haptic } from '../audio/SoundEngine';

export class TronRenderer implements IGameRenderer {
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
    const grid = 30;
    const sz = this.width / grid;

    ctx.fillStyle = '#070b14';
    ctx.fillRect(0, 0, this.width, this.height);

    if (state?.grid) {
      for (let y = 0; y < grid; y++) {
        for (let x = 0; x < grid; x++) {
          const val = state.grid[y * grid + x];
          if (val === 1) {
            ctx.fillStyle = '#0284c7';
            ctx.fillRect(x * sz, y * sz, sz, sz);
          } else if (val === 2) {
            ctx.fillStyle = '#be185d';
            ctx.fillRect(x * sz, y * sz, sz, sz);
          }
        }
      }
    }

    // Cycle Heads with Neon Bloom
    if (state?.p1 && state.p1.alive) {
      ctx.save();
      ctx.shadowBlur = 10;
      ctx.shadowColor = '#38bdf8';
      ctx.fillStyle = '#38bdf8';
      ctx.fillRect(state.p1.x * sz, state.p1.y * sz, sz, sz);
      ctx.restore();
    }
    if (state?.p2 && state.p2.alive) {
      ctx.save();
      ctx.shadowBlur = 10;
      ctx.shadowColor = '#ec4899';
      ctx.fillStyle = '#ec4899';
      ctx.fillRect(state.p2.x * sz, state.p2.y * sz, sz, sz);
      ctx.restore();
    }
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
