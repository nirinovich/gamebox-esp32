export class CameraJuice {
  private intensity = 0;
  private decay = 0.88;
  public offsetX = 0;
  public offsetY = 0;
  private reduced = false;

  constructor() {
    if (typeof window !== 'undefined' && window.matchMedia) {
      this.reduced = window.matchMedia('(prefers-reduced-motion: reduce)').matches;
    }
  }

  public shake(tier = 4) {
    if (this.reduced) return;
    this.intensity = Math.min(this.intensity + tier, 18);
  }

  public update() {
    if (this.intensity > 0.1) {
      this.offsetX = (Math.random() - 0.5) * 2 * this.intensity;
      this.offsetY = (Math.random() - 0.5) * 2 * this.intensity;
      this.intensity *= this.decay;
    } else {
      this.intensity = 0;
      this.offsetX = 0;
      this.offsetY = 0;
    }
  }

  public apply(ctx: CanvasRenderingContext2D) {
    if (this.intensity > 0) {
      ctx.translate(this.offsetX, this.offsetY);
    }
  }
}
