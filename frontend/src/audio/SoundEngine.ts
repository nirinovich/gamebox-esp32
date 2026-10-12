import { soundMuted } from '../state/gameStore';

export class SoundEngine {
  private ctx: AudioContext | null = null;

  public init() {
    if (!this.ctx && typeof window !== 'undefined') {
      const AudioCtx = window.AudioContext || (window as any).webkitAudioContext;
      if (AudioCtx) this.ctx = new AudioCtx();
    }
    if (this.ctx && this.ctx.state === 'suspended') {
      this.ctx.resume();
    }
  }

  public playTone(freq: number, type: OscillatorType, duration: number, vol = 0.1) {
    if (soundMuted.value) return;
    this.init();
    if (!this.ctx) return;
    try {
      const osc = this.ctx.createOscillator();
      const gain = this.ctx.createGain();
      osc.type = type;
      osc.frequency.setValueAtTime(freq, this.ctx.currentTime);
      gain.gain.setValueAtTime(vol, this.ctx.currentTime);
      gain.gain.exponentialRampToValueAtTime(0.0001, this.ctx.currentTime + duration);
      osc.connect(gain);
      gain.connect(this.ctx.destination);
      osc.start();
      osc.stop(this.ctx.currentTime + duration);
    } catch {}
  }

  public click() {
    this.playTone(600, 'square', 0.04, 0.05);
  }

  public move() {
    this.playTone(220, 'triangle', 0.05, 0.06);
  }

  public bounce() {
    this.playTone(440, 'sine', 0.05, 0.08);
  }

  public laser() {
    if (soundMuted.value) return;
    this.init();
    if (!this.ctx) return;
    try {
      const osc = this.ctx.createOscillator();
      const gain = this.ctx.createGain();
      osc.type = 'sawtooth';
      osc.frequency.setValueAtTime(880, this.ctx.currentTime);
      osc.frequency.exponentialRampToValueAtTime(110, this.ctx.currentTime + 0.1);
      gain.gain.setValueAtTime(0.09, this.ctx.currentTime);
      gain.gain.exponentialRampToValueAtTime(0.001, this.ctx.currentTime + 0.1);
      osc.connect(gain);
      gain.connect(this.ctx.destination);
      osc.start();
      osc.stop(this.ctx.currentTime + 0.1);
    } catch {}
  }

  public hit() {
    if (soundMuted.value) return;
    this.init();
    if (!this.ctx) return;
    try {
      const osc = this.ctx.createOscillator();
      const gain = this.ctx.createGain();
      osc.type = 'square';
      osc.frequency.setValueAtTime(160, this.ctx.currentTime);
      osc.frequency.linearRampToValueAtTime(30, this.ctx.currentTime + 0.14);
      gain.gain.setValueAtTime(0.16, this.ctx.currentTime);
      gain.gain.exponentialRampToValueAtTime(0.001, this.ctx.currentTime + 0.14);
      osc.connect(gain);
      gain.connect(this.ctx.destination);
      osc.start();
      osc.stop(this.ctx.currentTime + 0.14);
    } catch {}
  }

  public eat() {
    this.playTone(523.25, 'sine', 0.08, 0.12);
    setTimeout(() => this.playTone(659.25, 'sine', 0.08, 0.12), 60);
    setTimeout(() => this.playTone(783.99, 'sine', 0.12, 0.12), 120);
  }

  public place() {
    this.playTone(320, 'triangle', 0.04, 0.12);
    setTimeout(() => this.playTone(480, 'sine', 0.03, 0.1), 30);
  }

  public defeat() {
    this.playTone(293.66, 'sawtooth', 0.15, 0.15);
    setTimeout(() => this.playTone(220.0, 'sawtooth', 0.2, 0.15), 100);
    setTimeout(() => this.playTone(146.83, 'sawtooth', 0.35, 0.18), 220);
  }

  public victory() {
    const notes = [523.25, 659.25, 783.99, 1046.5];
    notes.forEach((f, i) => {
      setTimeout(() => this.playTone(f, 'square', 0.15, 0.1), i * 90);
    });
  }
}

export const sound = new SoundEngine();

export function haptic(ms = 15) {
  if (typeof navigator !== 'undefined' && navigator.vibrate) {
    try {
      navigator.vibrate(ms);
    } catch {}
  }
}
