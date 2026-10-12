export interface Particle {
  type: 'confetti' | 'spark' | 'ripple' | 'smoke';
  x: number;
  y: number;
  vx?: number;
  vy?: number;
  color: string;
  size?: number;
  radius?: number;
  maxRadius?: number;
  rot?: number;
  vRot?: number;
  alpha: number;
  decay: number;
  gravity?: number;
}

export class ParticleEngine {
  private particles: Particle[] = [];

  public confetti(x = 200, y = 200, count = 55) {
    const colors = ['#38bdf8', '#ec4899', '#f59e0b', '#22c55e', '#a855f7', '#ffffff'];
    for (let i = 0; i < count; i++) {
      const angle = Math.random() * Math.PI * 2;
      const speed = 2 + Math.random() * 5.5;
      this.particles.push({
        type: 'confetti',
        x,
        y,
        vx: Math.cos(angle) * speed,
        vy: Math.sin(angle) * speed - 2.5,
        color: colors[Math.floor(Math.random() * colors.length)],
        size: 3 + Math.random() * 3.5,
        rot: Math.random() * 360,
        vRot: (Math.random() - 0.5) * 12,
        alpha: 1,
        decay: 0.012 + Math.random() * 0.012,
        gravity: 0.11
      });
    }
  }

  public sparks(x: number, y: number, count = 16, color = '#38bdf8') {
    for (let i = 0; i < count; i++) {
      const angle = Math.random() * Math.PI * 2;
      const speed = 1.2 + Math.random() * 4.2;
      this.particles.push({
        type: 'spark',
        x,
        y,
        vx: Math.cos(angle) * speed,
        vy: Math.sin(angle) * speed,
        color,
        size: 1.5 + Math.random() * 2,
        alpha: 1,
        decay: 0.03 + Math.random() * 0.035,
        gravity: 0.04
      });
    }
  }

  public ripple(x: number, y: number, color = '#38bdf8') {
    this.particles.push({
      type: 'ripple',
      x,
      y,
      radius: 2,
      maxRadius: 26,
      color,
      alpha: 1,
      decay: 0.035
    });
  }

  public explosion(x: number, y: number, color = '#ef4444') {
    this.sparks(x, y, 20, '#f59e0b');
    for (let i = 0; i < 10; i++) {
      const angle = Math.random() * Math.PI * 2;
      const speed = 0.5 + Math.random() * 2;
      this.particles.push({
        type: 'smoke',
        x,
        y,
        vx: Math.cos(angle) * speed,
        vy: Math.sin(angle) * speed - 0.4,
        color,
        size: 4 + Math.random() * 5,
        alpha: 0.8,
        decay: 0.02 + Math.random() * 0.02
      });
    }
  }

  public updateAndRender(ctx: CanvasRenderingContext2D) {
    if (this.particles.length === 0) return;
    for (let i = this.particles.length - 1; i >= 0; i--) {
      const p = this.particles[i];
      p.alpha -= p.decay;
      if (p.alpha <= 0) {
        this.particles.splice(i, 1);
        continue;
      }
      ctx.globalAlpha = Math.max(0, p.alpha);
      if (p.type === 'confetti') {
        p.x += p.vx || 0;
        p.y += p.vy || 0;
        p.vy = (p.vy || 0) + (p.gravity || 0);
        p.rot = (p.rot || 0) + (p.vRot || 0);
        ctx.save();
        ctx.translate(p.x, p.y);
        ctx.rotate(((p.rot || 0) * Math.PI) / 180);
        ctx.fillStyle = p.color;
        const size = p.size || 4;
        ctx.fillRect(-size / 2, -size / 2, size, size * 0.6);
        ctx.restore();
      } else if (p.type === 'spark') {
        p.x += p.vx || 0;
        p.y += p.vy || 0;
        p.vy = (p.vy || 0) + (p.gravity || 0);
        ctx.fillStyle = p.color;
        ctx.beginPath();
        ctx.arc(p.x, p.y, p.size || 2, 0, Math.PI * 2);
        ctx.fill();
      } else if (p.type === 'ripple') {
        p.radius = (p.radius || 2) + 1.2;
        ctx.strokeStyle = p.color;
        ctx.lineWidth = 1.5;
        ctx.beginPath();
        ctx.arc(p.x, p.y, p.radius, 0, Math.PI * 2);
        ctx.stroke();
      } else if (p.type === 'smoke') {
        p.x += p.vx || 0;
        p.y += p.vy || 0;
        p.size = (p.size || 4) + 0.25;
        ctx.fillStyle = p.color;
        ctx.beginPath();
        ctx.arc(p.x, p.y, p.size, 0, Math.PI * 2);
        ctx.fill();
      }
    }
    ctx.globalAlpha = 1.0;
  }

  public clear() {
    this.particles = [];
  }
}
