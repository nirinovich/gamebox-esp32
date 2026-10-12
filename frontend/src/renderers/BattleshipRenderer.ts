import { IGameRenderer, ActionDispatcher } from './IGameRenderer';
import { gameSocket } from '../services/GameSocket';
import { sound, haptic } from '../audio/SoundEngine';
import { ParticleEngine } from './ParticleEngine';
import { CameraJuice } from './CameraJuice';
import { activeRoom } from '../state/gameStore';

export class BattleshipRenderer implements IGameRenderer {
  private width = 400;
  private height = 400;
  private prevEnemyGrid: number[] = [];
  private alertBanner = '';
  private alertTimer = 0;
  private dispatch: ActionDispatcher = (action) => gameSocket.send(action);

  constructor(
    private particles?: ParticleEngine,
    private juice?: CameraJuice
  ) {}

  public init(_canvas?: HTMLCanvasElement, _ctx?: CanvasRenderingContext2D, dispatch?: ActionDispatcher) {
    if (dispatch) this.dispatch = dispatch;
    this.prevEnemyGrid = [];
    this.alertBanner = '';
    this.alertTimer = 0;
  }

  public setDispatcher(dispatch: ActionDispatcher) {
    this.dispatch = dispatch;
  }

  public resize(width: number, height: number) {
    this.width = width;
    this.height = height;
  }

  public render(state: any, ctx: CanvasRenderingContext2D) {
    const sz = this.width / 10;

    ctx.fillStyle = '#070b14';
    ctx.fillRect(0, 0, this.width, this.height);

    // Sonar Grid Lines
    ctx.strokeStyle = '#111e33';
    ctx.lineWidth = 1;
    for (let i = 0; i <= 10; i++) {
      ctx.beginPath();
      ctx.moveTo(i * sz, 0);
      ctx.lineTo(i * sz, this.height);
      ctx.stroke();

      ctx.beginPath();
      ctx.moveTo(0, i * sz);
      ctx.lineTo(this.width, i * sz);
      ctx.stroke();
    }

    // Sonar Range Rings (Issue 07)
    const center = this.width / 2;
    ctx.save();
    ctx.strokeStyle = 'rgba(56, 189, 248, 0.12)';
    ctx.lineWidth = 1.2;
    ctx.beginPath();
    ctx.arc(center, center, 55, 0, Math.PI * 2);
    ctx.arc(center, center, 115, 0, Math.PI * 2);
    ctx.arc(center, center, 175, 0, Math.PI * 2);
    ctx.stroke();
    ctx.restore();

    // Continuous 360 Sonar Radar Sweep Line (Issue 07)
    const sweepAngle = (Date.now() / 1400) % (Math.PI * 2);
    ctx.save();
    ctx.strokeStyle = 'rgba(56, 189, 248, 0.45)';
    ctx.lineWidth = 1.8;
    ctx.beginPath();
    ctx.moveTo(center, center);
    ctx.lineTo(
      center + Math.cos(sweepAngle) * 280,
      center + Math.sin(sweepAngle) * 280
    );
    ctx.stroke();
    ctx.restore();

    const myPlayerNum = activeRoom.value?.playerNum || 1;
    const me = myPlayerNum === 1 ? state?.p1 : state?.p2;

    if (state?.phase === 'placement') {
      this.prevEnemyGrid = [];
      if (me?.own) {
        me.own.forEach((cell: number, idx: number) => {
          const r = Math.floor(idx / 10);
          const c = idx % 10;
          if (cell >= 1) {
            ctx.fillStyle = '#38bdf8';
            ctx.fillRect(c * sz + 2, r * sz + 2, sz - 4, sz - 4);
          }
        });
      }
    } else {
      if (me?.enemy) {
        const enemyCells = me.enemy as number[];
        // Check for new state changes to trigger VFX
        if (this.prevEnemyGrid.length === enemyCells.length) {
          for (let i = 0; i < enemyCells.length; i++) {
            if (this.prevEnemyGrid[i] === 0 && enemyCells[i] !== 0) {
              const r = Math.floor(i / 10);
              const c = i % 10;
              const cx = c * sz + sz / 2;
              const cy = r * sz + sz / 2;
              if (enemyCells[i] === 2) {
                // Direct Hit
                this.particles?.explosion(cx, cy, '#ef4444');
                this.juice?.shake(6);
                sound.hit();
                this.alertBanner = 'DIRECT HIT CONFIRMED!';
                this.alertTimer = 60;
              } else if (enemyCells[i] === 3) {
                // Splash Miss
                this.particles?.ripple(cx, cy, '#38bdf8');
                sound.bounce();
              }
            }
          }
        }
        this.prevEnemyGrid = [...enemyCells];

        enemyCells.forEach((cell: number, idx: number) => {
          const r = Math.floor(idx / 10);
          const c = idx % 10;
          const cx = c * sz + sz / 2;
          const cy = r * sz + sz / 2;

          if (cell === 2) {
            // Hit
            ctx.fillStyle = '#ef4444';
            ctx.fillRect(c * sz + 4, r * sz + 4, sz - 8, sz - 8);
          } else if (cell === 3) {
            // Miss
            ctx.fillStyle = '#475569';
            ctx.beginPath();
            ctx.arc(cx, cy, 3, 0, Math.PI * 2);
            ctx.fill();
          }
        });
      }
    }

    // Destruction & Hit Alert Banner
    if (this.alertTimer > 0) {
      this.alertTimer--;
      ctx.save();
      ctx.fillStyle = 'rgba(239, 68, 68, 0.9)';
      ctx.font = '700 13px system-ui, sans-serif';
      ctx.textAlign = 'center';
      ctx.fillText(this.alertBanner, this.width / 2, 28);
      ctx.restore();
    }
  }

  public handlePointerDown(x: number, y: number) {
    const sz = this.width / 10;
    const col = Math.floor(x / sz);
    const row = Math.floor(y / sz);

    if (col >= 0 && col < 10 && row >= 0 && row < 10) {
      sound.laser();
      haptic(20);
      this.dispatch({ cmd: 'fire', row, col });
    }
  }

  public handlePointerMove(_x: number, _y: number) {}
  public handlePointerUp(_x: number, _y: number) {}
  public destroy() {}
}
