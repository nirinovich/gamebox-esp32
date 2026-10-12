import { IGameRenderer } from './IGameRenderer';
import { gameSocket } from '../services/GameSocket';
import { sound, haptic } from '../audio/SoundEngine';
import { ParticleEngine } from './ParticleEngine';
import { CameraJuice } from './CameraJuice';
import { activeRoom } from '../state/gameStore';

export class BattleshipRenderer implements IGameRenderer {
  private width = 400;
  private height = 400;

  constructor(
    _particles?: ParticleEngine,
    _juice?: CameraJuice
  ) {}

  public init() {}

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

    // Continuous 360 Sonar Radar Sweep Line (Issue 07)
    const sweepAngle = (Date.now() / 1400) % (Math.PI * 2);
    ctx.save();
    ctx.strokeStyle = 'rgba(56, 189, 248, 0.35)';
    ctx.lineWidth = 1.8;
    ctx.beginPath();
    ctx.moveTo(this.width / 2, this.height / 2);
    ctx.lineTo(
      this.width / 2 + Math.cos(sweepAngle) * 280,
      this.height / 2 + Math.sin(sweepAngle) * 280
    );
    ctx.stroke();
    ctx.restore();

    const myPlayerNum = activeRoom.value?.playerNum || 1;
    const me = myPlayerNum === 1 ? state?.p1 : state?.p2;

    if (state?.phase === 'placement') {
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
        me.enemy.forEach((cell: number, idx: number) => {
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
  }

  public handlePointerDown(x: number, y: number) {
    const sz = this.width / 10;
    const col = Math.floor(x / sz);
    const row = Math.floor(y / sz);

    if (col >= 0 && col < 10 && row >= 0 && row < 10) {
      sound.laser();
      haptic(20);
      gameSocket.send({ cmd: 'fire', row, col });
    }
  }

  public handlePointerMove(_x: number, _y: number) {}
  public handlePointerUp(_x: number, _y: number) {}
  public destroy() {}
}
