// ============================================================================
// KatroRenderer.ts — Moteur de rendu Canvas 2D du Katro malgache
//
// Fidèle au design system Gamebox (DESIGN.md) et aux composants du jeu Fanorona :
//   - Toile de fond Obsidian Gamebox (#14151b).
//   - Plateau traditionnel en bois sculpté / parchemin chaud (#ede8dd).
//   - Pierres sphériques 3D identiques à Fanorona :
//       • Sud (Joueur 1 / Foncées) : Dégradé sphérique ardoise sombre (#64748b -> #0f172a).
//       • Nord (Joueur 2 / Claires) : Dégradé sphérique terracotta corail (#fb923c -> #7c2d12).
//   - Éléments de capture à fort impact : éclairs cinétiques, gerbes d'étincelles
//     (sparks) aux couleurs du joueur et secousses d'écran CameraJuice.
//   - Bannières d'action tactiques console (Surfaces Gamebox #1c1d25, liseré #ef3938).
//   - Badges de comptage monospace et surbrillances nettes sans halo flou.
// ============================================================================

import { IGameRenderer, ActionDispatcher } from '../../../frontend/src/renderers/IGameRenderer';
import { sound, haptic } from '../../../frontend/src/audio/SoundEngine';
import { CameraJuice } from '../../../frontend/src/renderers/CameraJuice';

// ============================================================================
// Types d'animation et de particules
// ============================================================================

export interface AnimEvent {
  t: 'pickup' | 'drop' | 'capture';
  from: number;
  to: number;
  count: number;
}

export interface FlyingParticle {
  startX: number;
  startY: number;
  targetX: number;
  targetY: number;
  t: number;          // 0 .. 1
  speed: number;
  arcHeight: number;
  player: number;     // 1 = Sud (slate-blue), 2 = Nord (terracotta)
  size: number;
  isCapture: boolean;
  targetCell: number;
  trail: Array<{ x: number; y: number; alpha: number }>;
}

export interface KatroSpark {
  x: number;
  y: number;
  vx: number;
  vy: number;
  life: number;
  decay: number;
  color: string;
  size: number;
}

export interface CaptureBeam {
  fromX: number;
  fromY: number;
  toX: number;
  toY: number;
  life: number;
  maxLife: number;
}

export interface FloatingBanner {
  x: number;
  y: number;
  text: string;
  color: string;
  borderColor: string;
  alpha: number;
  life: number;
  maxLife: number;
}

export interface SeedPop {
  cellIndex: number;
  frame: number;
}

// ============================================================================
// Tokens graphiques & palette canonique Gamebox / Fanorona
// ============================================================================

const ROWS        = 4;     // 4 rangées (2 Sud, 2 Nord)
const POP_FRAMES  = 14;

// 1. Toile et surfaces (strictement conformes à DESIGN.md et Fanorona)
const COLOR_CANVAS_BASE    = '#14151b'; // Fond principal Obsidian
const COLOR_BOARD_SURFACE  = '#ede8dd'; // Surface bois/parchemin traditionnel Fanorona
const COLOR_BOARD_BORDER   = '#d4cbbd'; // Liseré clair du plateau
const COLOR_WOOD_LINE      = '#6d4c30'; // Lignes de gravure traditionnelles
const COLOR_WOOD_SUBTLE    = '#8d633d'; // Rainures et repères secondaires

// 2. Creux sculptés (Pits de Katro)
const COLOR_PIT_BG_TOP     = '#ded5c4';
const COLOR_PIT_BG_BOT     = '#cfc4af';
const COLOR_PIT_BORDER     = '#c4b9a5';

// 3. Joueur 1 (Sud / Foncées) — Ardoise Sombre Fanorona
const P1_COLOR_LIGHT       = '#64748b';
const P1_COLOR_MID         = '#334155';
const P1_COLOR_DEEP        = '#1e293b';
const P1_COLOR_BASE        = '#0f172a';
const P1_SPARK_BASE        = '#38bdf8';
const P1_SPARK_ALT         = '#f8fafc';

// 4. Joueur 2 (Nord / Claires) — Terracotta Corail Fanorona
const P2_COLOR_LIGHT       = '#fb923c';
const P2_COLOR_MID         = '#ea580c';
const P2_COLOR_DEEP        = '#c2410c';
const P2_COLOR_BASE        = '#7c2d12';
const P2_SPARK_BASE        = '#ea580c';
const P2_SPARK_ALT         = '#fef08a';

// 5. Gamebox Brand Accents
const GB_RED               = '#ef3938'; // Rouge signature Gamebox
const GB_RED_RING          = 'rgba(239, 57, 56, 0.40)';
const GB_RED_DIM           = 'rgba(239, 57, 56, 0.14)';
const GB_SURFACE_1         = '#1c1d25';
const GB_TEXT_PRIMARY      = '#ffffff';
const GB_TEXT_SECONDARY    = '#9e9fa8';
const GB_TEXT_TERTIARY     = '#656775';

// ============================================================================
// KatroRenderer — Implémente IGameRenderer
// ============================================================================

export class KatroRenderer implements IGameRenderer {
  private width       = 400;
  private height      = 400;
  private dispatch: ActionDispatcher = () => {};
  private playerNum   = 1;
  private isSolo      = false;
  private currentCols = 4;
  private juice?: CameraJuice;

  private hoveredCell = -1;
  private pressedCell = -1;

  // État du plateau synchronisé et tamponné
  private displayBoard: number[] = [];
  private lastSeq     = -1;
  private isAnimating = false;

  // File d'états serveur en attente d'animation
  private pendingStateQueue: Array<{ board: number[]; events: AnimEvent[] }> = [];

  // Événements d'animation et particules
  private eventQueue: AnimEvent[] = [];
  private animTimer   = 0;
  private lastEventPos: { x: number; y: number } | null = null;
  private lastAnimTime = 0;

  private particles: FlyingParticle[] = [];
  private sparks: KatroSpark[] = [];
  private beams: CaptureBeam[] = [];
  private banners: FloatingBanner[] = [];
  private pops: SeedPop[] = [];
  private shakeFrames = 0;

  // Case active lors du relais
  private activeRelayCell = -1;
  private relayHaloFrames = 0;

  constructor(juice?: CameraJuice) {
    this.juice = juice;
  }

  public init(
    _canvas?: HTMLCanvasElement,
    _ctx?: CanvasRenderingContext2D,
    dispatch?: ActionDispatcher,
    playerNum?: number
  ): void {
    if (dispatch)  this.dispatch  = dispatch;
    if (playerNum) this.playerNum = playerNum;

    this.displayBoard      = [];
    this.lastSeq           = -1;
    this.isAnimating       = false;
    this.pendingStateQueue = [];
    this.eventQueue        = [];
    this.particles         = [];
    this.sparks            = [];
    this.beams             = [];
    this.banners           = [];
    this.pops              = [];
    this.shakeFrames       = 0;
    this.activeRelayCell   = -1;
    this.relayHaloFrames   = 0;
    this.hoveredCell       = -1;
    this.pressedCell       = -1;
    this.currentCols       = 4;
    this.lastAnimTime      = 0;
  }

  public setDispatcher(dispatch: ActionDispatcher): void {
    this.dispatch = dispatch;
  }

  public setPlayerNum(playerNum: number): void {
    this.playerNum = playerNum;
  }

  public setIsSolo(isSolo: boolean): void {
    this.isSolo = isSolo;
  }

  public resize(width: number, height: number): void {
    this.width  = width;
    this.height = height;
  }

  // -------------------------------------------------------------------------
  // Géométrie & disposition du plateau (Calibrée pour tests et affichage)
  // -------------------------------------------------------------------------

  public getBoardLayout() {
    const cols = this.currentCols;
    const topOffset = 42;
    const botOffset = 24;
    const padX = 12;

    const availW = this.width - padX * 2;
    const availH = this.height - topOffset - botOffset;

    const cellW = availW / cols;
    const cellH = availH / ROWS;

    return {
      startX: padX,
      startY: topOffset,
      boardW: availW,
      boardH: availH,
      cellW,
      cellH,
      cols
    };
  }

  // -------------------------------------------------------------------------
  // Dessin d'une pierre 3D sphérique (Exactement identique à Fanorona)
  // -------------------------------------------------------------------------

  public drawStoneSphere(
    ctx: CanvasRenderingContext2D,
    x: number,
    y: number,
    r: number,
    player: number
  ): void {
    if (r <= 0) return;

    if (player === 1) {
      // Joueur 1 (Sud) : Sphère Ardoise Sombre
      let fillApplied = false;
      if (ctx.createRadialGradient) {
        try {
          const grad = ctx.createRadialGradient(x - r * 0.35, y - r * 0.35, r * 0.05, x, y, r);
          if (grad && typeof grad.addColorStop === 'function') {
            grad.addColorStop(0, P1_COLOR_LIGHT);
            grad.addColorStop(0.35, P1_COLOR_MID);
            grad.addColorStop(0.85, P1_COLOR_DEEP);
            grad.addColorStop(1, P1_COLOR_BASE);
            ctx.fillStyle = grad;
            fillApplied = true;
          }
        } catch (_) {}
      }
      if (!fillApplied) {
        ctx.fillStyle = P1_COLOR_MID;
      }

      ctx.beginPath();
      ctx.arc(x, y, r, 0, Math.PI * 2);
      ctx.fill();

      ctx.strokeStyle = P1_COLOR_DEEP;
      ctx.lineWidth = 1;
      ctx.stroke();
    } else {
      // Joueur 2 (Nord) : Sphère Terracotta Corail
      let fillApplied = false;
      if (ctx.createRadialGradient) {
        try {
          const grad = ctx.createRadialGradient(x - r * 0.35, y - r * 0.35, r * 0.05, x, y, r);
          if (grad && typeof grad.addColorStop === 'function') {
            grad.addColorStop(0, P2_COLOR_LIGHT);
            grad.addColorStop(0.35, P2_COLOR_MID);
            grad.addColorStop(0.85, P2_COLOR_DEEP);
            grad.addColorStop(1, P2_COLOR_BASE);
            ctx.fillStyle = grad;
            fillApplied = true;
          }
        } catch (_) {}
      }
      if (!fillApplied) {
        ctx.fillStyle = P2_COLOR_MID;
      }

      ctx.beginPath();
      ctx.arc(x, y, r, 0, Math.PI * 2);
      ctx.fill();

      ctx.strokeStyle = '#9a3412';
      ctx.lineWidth = 1;
      ctx.stroke();
    }
  }

  // -------------------------------------------------------------------------
  // render() — Boucle principale de rendu à 60 FPS
  // -------------------------------------------------------------------------

  public render(state: any, ctx: CanvasRenderingContext2D): void {
    const now = (typeof performance !== 'undefined') ? performance.now() : Date.now();
    const dt = this.lastAnimTime ? Math.min(0.1, (now - this.lastAnimTime) / 1000) : 0.016;
    this.lastAnimTime = now;

    if (state?.board && Array.isArray(state.board)) {
      this.currentCols = Math.max(1, Math.floor(state.board.length / ROWS));

      const seq = state.seq ?? 0;
      if (this.lastSeq === -1) {
        this.displayBoard = [...state.board];
        this.lastSeq = seq;
      } else if (seq > this.lastSeq) {
        this.lastSeq = seq;
        const events = (state.events && Array.isArray(state.events))
          ? state.events as AnimEvent[]
          : [];
        if (!this.isAnimating && this.eventQueue.length === 0 && this.particles.length === 0) {
          if (events.length > 0) {
            this.eventQueue = events;
            this.isAnimating = true;
            this.animTimer = 0;
            this.pendingStateQueue.push({ board: [...state.board], events: [] });
          } else {
            this.displayBoard = [...state.board];
          }
        } else {
          this.pendingStateQueue.push({ board: [...state.board], events });
        }
      }
    }

    // Mise à jour de la dynamique des particules et de la file d'événements
    this._updateAnimation(state, dt);

    // 1. Fond Obsidian Gamebox
    ctx.fillStyle = COLOR_CANVAS_BASE;
    ctx.fillRect(0, 0, this.width, this.height);

    ctx.save();

    // Application de CameraJuice ou tremblement d'écran
    if (this.juice) {
      this.juice.update();
      this.juice.apply(ctx);
    } else if (this.shakeFrames > 0) {
      const sx = (Math.random() - 0.5) * 5;
      const sy = (Math.random() - 0.5) * 5;
      ctx.translate(sx, sy);
      this.shakeFrames--;
    }

    if (!state?.board) {
      this._drawConnecting(ctx);
      ctx.restore();
      return;
    }

    // 2. Éléments du plateau et HUD
    this._drawBoardCanvas(ctx);
    this._drawPits(ctx, state);
    this._drawBeams(ctx);
    this._drawParticles(ctx);
    this._drawSparks(ctx);
    this._drawBanners(ctx);

    ctx.restore();

    // 3. HUD supérieur et inférieur fixes
    this._drawScoreBar(ctx, state);
    this._drawBottomBar(ctx, state);
  }

  // -------------------------------------------------------------------------
  // _updateAnimation() — Mise à jour des animations, particules et relais
  // -------------------------------------------------------------------------

  private _updateAnimation(state: any, dt: number): void {
    // 1. Mise à jour des graines volantes
    for (let i = this.particles.length - 1; i >= 0; i--) {
      const p = this.particles[i];
      p.t += p.speed;

      const curX = (1 - p.t) * p.startX + p.t * p.targetX;
      const curY = (1 - p.t) * p.startY + p.t * p.targetY - p.arcHeight * Math.sin(p.t * Math.PI);
      p.trail.push({ x: curX, y: curY, alpha: 1.0 });
      if (p.trail.length > 5) p.trail.shift();

      if (p.t >= 1.0) {
        if (p.targetCell >= 0 && p.targetCell < this.displayBoard.length) {
          this.displayBoard[p.targetCell]++;
          if (!this.pops.find(pop => pop.cellIndex === p.targetCell)) {
            this.pops.push({ cellIndex: p.targetCell, frame: 0 });
          }
        }
        if (p.isCapture) {
          sound.eat();
          haptic(30);
          this.juice?.shake(2.5);
        } else {
          sound.place();
          haptic(8);
        }
        this.particles.splice(i, 1);
      }
    }

    // 2. Mise à jour des étincelles (sparks) de capture
    for (let i = this.sparks.length - 1; i >= 0; i--) {
      const sp = this.sparks[i];
      sp.x += sp.vx * dt;
      sp.y += sp.vy * dt;
      sp.vx *= 0.94;
      sp.vy *= 0.94;
      sp.life -= sp.decay * dt;

      if (sp.life <= 0) {
        this.sparks.splice(i, 1);
      }
    }

    // 3. Faisceaux d'énergie de capture
    this.beams = this.beams
      .map(b => ({ ...b, life: b.life - 1 }))
      .filter(b => b.life > 0);

    // 4. Bannières flottantes
    this.banners = this.banners
      .map(b => ({ ...b, y: b.y - 0.6, life: b.life - 1, alpha: b.life / b.maxLife }))
      .filter(b => b.life > 0);

    // 5. Rebonds d'arrivée (pops)
    this.pops = this.pops
      .map(p => ({ ...p, frame: p.frame + 1 }))
      .filter(p => p.frame < POP_FRAMES);

    if (this.relayHaloFrames > 0) this.relayHaloFrames--;

    // 6. Déroulement cadencé des événements de semis
    if (this.eventQueue.length > 0) {
      this.animTimer++;
      const nextEv = this.eventQueue[0];
      const delayThreshold = (nextEv.t === 'capture') ? 18 : (nextEv.t === 'pickup' ? 8 : 10);

      if (this.animTimer >= delayThreshold) {
        this.animTimer = 0;
        const ev = this.eventQueue.shift()!;
        this._processAnimEvent(ev);
      }
    } else if (this.particles.length === 0) {
      this.isAnimating = false;
      this.activeRelayCell = -1;

      if (this.pendingStateQueue.length > 0) {
        const next = this.pendingStateQueue.shift()!;
        if (next.events.length > 0) {
          this.eventQueue  = next.events;
          this.isAnimating = true;
          this.animTimer   = 0;
          if (this.pendingStateQueue.length === 0) {
            this.pendingStateQueue.push({ board: next.board, events: [] });
          }
        } else {
          this.displayBoard = [...next.board];
        }
      } else if (state?.board) {
        this.displayBoard = [...state.board];
      }
    }
  }

  // -------------------------------------------------------------------------
  // _processAnimEvent() — Déclenchement d'un événement (pickup, drop, capture)
  // -------------------------------------------------------------------------

  private _processAnimEvent(ev: AnimEvent): void {
    if (ev.t === 'pickup') {
      if (ev.from >= 0 && ev.from < this.displayBoard.length) {
        this.displayBoard[ev.from] = Math.max(0, this.displayBoard[ev.from] - ev.count);
        this.activeRelayCell = ev.from;
        this.relayHaloFrames = 18;

        const pos = this._getCellCenter(ev.from);
        this.lastEventPos = pos;

        if (ev.count > 2) {
          this.banners.push({
            x: pos.x,
            y: pos.y - 14,
            text: `RELAIS (${ev.count})`,
            color: '#38bdf8',
            borderColor: 'rgba(56, 189, 248, 0.4)',
            alpha: 1.0,
            life: 28,
            maxLife: 28
          });
        }
      }
    } else if (ev.t === 'drop') {
      if (ev.to >= 0 && ev.to < this.displayBoard.length) {
        const targetPos = this._getCellCenter(ev.to);
        const startPos  = this.lastEventPos || targetPos;
        const isSouth   = ev.to < this.currentCols * 2;

        this.particles.push({
          startX: startPos.x,
          startY: startPos.y,
          targetX: targetPos.x,
          targetY: targetPos.y,
          t: 0,
          speed: 0.22,
          arcHeight: 22,
          player: isSouth ? 1 : 2,
          size: 6,
          isCapture: false,
          targetCell: ev.to,
          trail: []
        });

        this.lastEventPos = targetPos;
      }
    } else if (ev.t === 'capture') {
      if (ev.from >= 0 && ev.to >= 0) {
        const fromPos = this._getCellCenter(ev.from);
        const toPos   = this._getCellCenter(ev.to);

        if (ev.from < this.displayBoard.length) {
          this.displayBoard[ev.from] = 0;
        }

        // Faisceau d'énergie rouge Gamebox à travers la ligne médiane
        this.beams.push({
          fromX: fromPos.x,
          fromY: fromPos.y,
          toX: toPos.x,
          toY: toPos.y,
          life: 16,
          maxLife: 16
        });

        this.shakeFrames = 7;
        this.juice?.shake(4.0);
        sound.eat();
        haptic(45);

        // Gerbe d'étincelles Fanorona
        const capturedPlayer = (ev.from < this.currentCols * 2) ? 1 : 2;
        const baseColor = capturedPlayer === 1 ? P1_SPARK_BASE : P2_SPARK_BASE;
        const altColor  = capturedPlayer === 1 ? P1_SPARK_ALT : P2_SPARK_ALT;

        for (let s = 0; s < 12; s++) {
          const angle = (Math.PI * 2 * s) / 12 + (Math.random() - 0.5) * 0.4;
          const speed = 70 + Math.random() * 85;
          this.sparks.push({
            x: fromPos.x,
            y: fromPos.y,
            vx: Math.cos(angle) * speed,
            vy: Math.sin(angle) * speed,
            life: 1.0,
            decay: 2.2 + Math.random() * 1.0,
            color: Math.random() > 0.4 ? (Math.random() > 0.3 ? baseColor : GB_RED) : altColor,
            size: 2.5 + Math.random() * 2
          });
        }

        // Vol en arc des graines capturées
        const seedCount = Math.max(1, Math.min(ev.count, 8));
        const toSouth   = ev.to < this.currentCols * 2;
        for (let i = 0; i < seedCount; i++) {
          const spreadX = (Math.random() - 0.5) * 16;
          const spreadY = (Math.random() - 0.5) * 12;

          this.particles.push({
            startX: fromPos.x + spreadX,
            startY: fromPos.y + spreadY,
            targetX: toPos.x + spreadX,
            targetY: toPos.y + spreadY,
            t: 0,
            speed: 0.14 + (i * 0.015),
            arcHeight: 45 + (i * 4),
            player: toSouth ? 1 : 2,
            size: 6.5,
            isCapture: true,
            targetCell: ev.to,
            trail: []
          });
        }

        // Bannière de capture Gamebox
        this.banners.push({
          x: (fromPos.x + toPos.x) / 2,
          y: (fromPos.y + toPos.y) / 2 - 8,
          text: `PRISE +${ev.count}`,
          color: GB_TEXT_PRIMARY,
          borderColor: GB_RED,
          alpha: 1.0,
          life: 38,
          maxLife: 38
        });

        this.lastEventPos = toPos;
      }
    }
  }

  // -------------------------------------------------------------------------
  // _drawBoardCanvas() — Surface en bois / parchemin chaud (Fanorona)
  // -------------------------------------------------------------------------

  private _drawBoardCanvas(ctx: CanvasRenderingContext2D): void {
    const { startX, startY, boardW, boardH, cellH } = this.getBoardLayout();

    // 1. Ombre portée réaliste du plateau
    ctx.save();
    ctx.shadowBlur = 16;
    ctx.shadowColor = 'rgba(0, 0, 0, 0.6)';
    ctx.shadowOffsetY = 4;
    ctx.fillStyle = COLOR_BOARD_SURFACE;
    ctx.beginPath();
    ctx.roundRect(startX, startY, boardW, boardH, 10);
    ctx.fill();

    ctx.strokeStyle = COLOR_BOARD_BORDER;
    ctx.lineWidth = 2;
    ctx.stroke();
    ctx.restore();

    // 2. Ligne médiane traditionnelle (Séparation sculptée Sud / Nord)
    const midY = startY + 2 * cellH;
    ctx.save();
    ctx.strokeStyle = COLOR_WOOD_LINE;
    ctx.lineWidth = 2.0;
    ctx.beginPath();
    ctx.moveTo(startX + 8, midY - 1);
    ctx.lineTo(startX + boardW - 8, midY - 1);
    ctx.stroke();

    ctx.strokeStyle = 'rgba(255, 255, 255, 0.45)';
    ctx.lineWidth = 1.0;
    ctx.beginPath();
    ctx.moveTo(startX + 8, midY + 1);
    ctx.lineTo(startX + boardW - 8, midY + 1);
    ctx.stroke();

    // Repère central sculpté (Style Lakabe de Fanorona)
    const cx = startX + boardW / 2;
    ctx.strokeStyle = COLOR_WOOD_SUBTLE;
    ctx.lineWidth = 1.5;
    ctx.beginPath();
    ctx.arc(cx, midY, 6, 0, Math.PI * 2);
    ctx.stroke();
    ctx.restore();
  }

  // -------------------------------------------------------------------------
  // _drawPits() — Dessin des creux sculptés et des graines
  // -------------------------------------------------------------------------

  private _drawPits(ctx: CanvasRenderingContext2D, state: any): void {
    const { startX, startY, cellW, cellH, cols } = this.getBoardLayout();
    const pitPad = 5;

    for (let row = 0; row < ROWS; row++) {
      for (let col = 0; col < cols; col++) {
        const cellIndex = row * cols + col;
        const seeds     = this.displayBoard[cellIndex] ?? (state.board?.[cellIndex] ?? 0);
        const isSouth   = row < 2;

        const px = startX + col * cellW + pitPad;
        const py = startY + row * cellH + pitPad;
        const pw = cellW - pitPad * 2;
        const ph = cellH - pitPad * 2;

        // 1. Fond du creux en dégradé creusé
        let pitFill: any = COLOR_PIT_BG_TOP;
        if (ctx.createLinearGradient) {
          try {
            const pitGrad = ctx.createLinearGradient(px, py, px, py + ph);
            if (pitGrad && typeof pitGrad.addColorStop === 'function') {
              pitGrad.addColorStop(0, COLOR_PIT_BG_TOP);
              pitGrad.addColorStop(1, COLOR_PIT_BG_BOT);
              pitFill = pitGrad;
            }
          } catch (_) {}
        }

        ctx.save();
        ctx.fillStyle = pitFill;
        ctx.beginPath();
        if (ctx.roundRect) {
          ctx.roundRect(px, py, pw, ph, 8);
        } else {
          ctx.rect(px, py, pw, ph);
        }
        ctx.fill();

        ctx.strokeStyle = COLOR_PIT_BORDER;
        ctx.lineWidth = 1.2;
        ctx.stroke();

        // 2. Surbrillance de la case de relais
        if (cellIndex === this.activeRelayCell && this.relayHaloFrames > 0) {
          ctx.strokeStyle = GB_RED;
          ctx.lineWidth = 2.5;
          ctx.beginPath();
          ctx.roundRect(px - 1, py - 1, pw + 2, ph + 2, 8);
          ctx.stroke();
        }

        // 3. Surbrillance au survol / sélection (Rouge Gamebox net)
        if (!this.isAnimating && (cellIndex === this.hoveredCell || cellIndex === this.pressedCell)) {
          const myCell = isSouth ? (this.playerNum === 1) : (this.playerNum === 2);
          if (myCell && !state.finished && state.turn === this.playerNum && seeds > 0) {
            ctx.fillStyle = GB_RED_DIM;
            ctx.fill();

            // Double anneau net sans flou (Impeccable Craft Floor)
            ctx.strokeStyle = GB_RED;
            ctx.lineWidth = 2.0;
            ctx.stroke();

            ctx.strokeStyle = GB_RED_RING;
            ctx.lineWidth = 1.0;
            ctx.beginPath();
            ctx.roundRect(px - 3, py - 3, pw + 6, ph + 6, 10);
            ctx.stroke();
          }
        }

        ctx.restore();

        // 4. Dessin des graines dans le creux
        this._drawPitSeeds(ctx, px, py, pw, ph, seeds, isSouth ? 1 : 2, cellIndex);
      }
    }
  }

  // -------------------------------------------------------------------------
  // _drawPitSeeds() — Représentation des pierres et badge monospace
  // -------------------------------------------------------------------------

  private _drawPitSeeds(
    ctx: CanvasRenderingContext2D,
    px: number, py: number,
    pw: number, ph: number,
    seeds: number,
    player: number,
    cellIndex: number
  ): void {
    if (seeds <= 0) return;

    const cx = px + pw / 2;
    const cy = py + ph / 2;
    const stoneR = Math.max(5, Math.min(pw, ph) * 0.135);

    let scale = 1.0;
    const pop = this.pops.find(p => p.cellIndex === cellIndex);
    if (pop) {
      scale = 1.0 + 0.32 * Math.sin((pop.frame / POP_FRAMES) * Math.PI);
    }

    const r = stoneR * scale;
    const offsetY = -4; // Décalage vers le haut pour laisser la place au badge

    // Dessin géométrique soigné des pierres sphériques 3D
    ctx.save();
    ctx.shadowBlur = 4;
    ctx.shadowColor = 'rgba(20, 15, 10, 0.40)';
    ctx.shadowOffsetY = 2;

    if (seeds === 1) {
      this.drawStoneSphere(ctx, cx, cy + offsetY, r, player);
    } else if (seeds === 2) {
      this.drawStoneSphere(ctx, cx - r * 1.05, cy + offsetY, r, player);
      this.drawStoneSphere(ctx, cx + r * 1.05, cy + offsetY, r, player);
    } else if (seeds === 3) {
      this.drawStoneSphere(ctx, cx, cy - r * 0.95 + offsetY, r, player);
      this.drawStoneSphere(ctx, cx - r * 1.05, cy + r * 0.75 + offsetY, r, player);
      this.drawStoneSphere(ctx, cx + r * 1.05, cy + r * 0.75 + offsetY, r, player);
    } else if (seeds === 4) {
      this.drawStoneSphere(ctx, cx - r * 1.0, cy - r * 0.9 + offsetY, r, player);
      this.drawStoneSphere(ctx, cx + r * 1.0, cy - r * 0.9 + offsetY, r, player);
      this.drawStoneSphere(ctx, cx - r * 1.0, cy + r * 0.9 + offsetY, r, player);
      this.drawStoneSphere(ctx, cx + r * 1.0, cy + r * 0.9 + offsetY, r, player);
    } else {
      // 5+ graines : groupe harmonieux de 5 pierres représentatives
      this.drawStoneSphere(ctx, cx, cy + offsetY, r * 0.95, player);
      this.drawStoneSphere(ctx, cx - r * 1.1, cy - r * 0.7 + offsetY, r * 0.95, player);
      this.drawStoneSphere(ctx, cx + r * 1.1, cy - r * 0.7 + offsetY, r * 0.95, player);
      this.drawStoneSphere(ctx, cx - r * 1.0, cy + r * 0.8 + offsetY, r * 0.95, player);
      this.drawStoneSphere(ctx, cx + r * 1.0, cy + r * 0.8 + offsetY, r * 0.95, player);
    }
    ctx.restore();

    // Badge monospace lisible indiquant le compte exact (Style Gamebox)
    const badgeW = 24;
    const badgeH = 15;
    const bx = cx - badgeW / 2;
    const by = py + ph - badgeH - 3;

    ctx.save();
    ctx.fillStyle = 'rgba(20, 21, 27, 0.75)';
    ctx.beginPath();
    ctx.roundRect(bx, by, badgeW, badgeH, 4);
    ctx.fill();

    ctx.strokeStyle = 'rgba(255, 255, 255, 0.16)';
    ctx.lineWidth = 1;
    ctx.stroke();

    ctx.fillStyle = GB_TEXT_PRIMARY;
    ctx.font = '800 10px ui-monospace, SFMono-Regular, monospace';
    ctx.textAlign = 'center';
    ctx.textBaseline = 'middle';
    ctx.fillText(String(seeds), cx, by + badgeH / 2 + 0.5);
    ctx.restore();
  }

  // -------------------------------------------------------------------------
  // _drawBeams() — Éclairs d'énergie cinétique lors des captures
  // -------------------------------------------------------------------------

  private _drawBeams(ctx: CanvasRenderingContext2D): void {
    for (const b of this.beams) {
      const alpha = b.life / b.maxLife;

      ctx.save();
      ctx.strokeStyle = `rgba(239, 57, 56, ${alpha})`;
      ctx.lineWidth = 3.5;
      ctx.beginPath();
      ctx.moveTo(b.fromX, b.fromY);

      const midX = (b.fromX + b.toX) / 2 + (Math.random() - 0.5) * 10;
      const midY = (b.fromY + b.toY) / 2;
      ctx.lineTo(midX, midY);
      ctx.lineTo(b.toX, b.toY);
      ctx.stroke();

      // Cœur lumineux blanc pur
      ctx.strokeStyle = `rgba(255, 255, 255, ${alpha * 0.85})`;
      ctx.lineWidth = 1.5;
      ctx.stroke();
      ctx.restore();
    }
  }

  // -------------------------------------------------------------------------
  // _drawParticles() — Pierres 3D volantes en trajectoire parabolique
  // -------------------------------------------------------------------------

  private _drawParticles(ctx: CanvasRenderingContext2D): void {
    for (const p of this.particles) {
      // 1. Traînée lumineuse subtile
      for (let i = 0; i < p.trail.length; i++) {
        const pt = p.trail[i];
        const a = (i / p.trail.length) * 0.45;
        ctx.fillStyle = p.isCapture
          ? `rgba(239, 57, 56, ${a})`
          : (p.player === 1 ? `rgba(56, 189, 248, ${a})` : `rgba(234, 88, 12, ${a})`);
        ctx.beginPath();
        ctx.arc(pt.x, pt.y, p.size * 0.5, 0, Math.PI * 2);
        ctx.fill();
      }

      // 2. Position de la pierre
      const curX = (1 - p.t) * p.startX + p.t * p.targetX;
      const curY = (1 - p.t) * p.startY + p.t * p.targetY - p.arcHeight * Math.sin(p.t * Math.PI);

      ctx.save();
      ctx.shadowBlur = 8;
      ctx.shadowColor = 'rgba(0, 0, 0, 0.5)';
      ctx.shadowOffsetY = 4;
      this.drawStoneSphere(ctx, curX, curY, p.size, p.player);
      ctx.restore();
    }
  }

  // -------------------------------------------------------------------------
  // _drawSparks() — Étincelles de dispersion (Fanorona)
  // -------------------------------------------------------------------------

  private _drawSparks(ctx: CanvasRenderingContext2D): void {
    for (const sp of this.sparks) {
      ctx.save();
      ctx.globalAlpha = Math.max(0, Math.min(1, sp.life));
      ctx.fillStyle = sp.color;
      ctx.shadowBlur = 6;
      ctx.shadowColor = sp.color;
      ctx.beginPath();
      ctx.arc(sp.x, sp.y, sp.size * sp.life, 0, Math.PI * 2);
      ctx.fill();
      ctx.restore();
    }
  }

  // -------------------------------------------------------------------------
  // _drawBanners() — Bannières console flottantes
  // -------------------------------------------------------------------------

  private _drawBanners(ctx: CanvasRenderingContext2D): void {
    for (const b of this.banners) {
      ctx.save();
      ctx.globalAlpha = b.alpha;

      ctx.font = '900 11px -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif';
      const textWidth = ctx.measureText(b.text).width;
      const padX = 12;
      const pillW = textWidth + padX * 2;
      const pillH = 22;

      ctx.fillStyle = GB_SURFACE_1;
      ctx.beginPath();
      ctx.roundRect(b.x - pillW / 2, b.y - pillH / 2, pillW, pillH, 4);
      ctx.fill();

      ctx.strokeStyle = b.borderColor;
      ctx.lineWidth = 1;
      ctx.stroke();

      ctx.fillStyle = b.color;
      ctx.textAlign = 'center';
      ctx.textBaseline = 'middle';
      ctx.fillText(b.text, b.x, b.y);

      ctx.restore();
    }
  }

  // -------------------------------------------------------------------------
  // _drawScoreBar() — HUD Supérieur fidèle à Fanorona
  // -------------------------------------------------------------------------

  private _drawScoreBar(ctx: CanvasRenderingContext2D, state: any): void {
    const barH = 36;
    ctx.save();
    ctx.fillStyle = COLOR_CANVAS_BASE;
    ctx.fillRect(0, 0, this.width, barH);

    const sScores = state.scores?.[0] ?? 0;
    const nScores = state.scores?.[1] ?? 0;

    // 1. Joueur Sud (Gauche) : Indicateur Ardoise Sombre + Score
    this.drawStoneSphere(ctx, 16, 18, 5, 1);
    ctx.font = '800 11px ui-monospace, SFMono-Regular, monospace';
    ctx.textAlign = 'left';
    ctx.fillStyle = state.turn === 1 ? GB_TEXT_PRIMARY : GB_TEXT_SECONDARY;
    ctx.fillText(`SUD: ${sScores}`, 28, 22);

    // 2. Joueur Nord (Droite) : Indicateur Terracotta Corail + Score
    this.drawStoneSphere(ctx, this.width - 16, 18, 5, 2);
    ctx.textAlign = 'right';
    ctx.fillStyle = state.turn === 2 ? GB_TEXT_PRIMARY : GB_TEXT_SECONDARY;
    ctx.fillText(`NORD: ${nScores}`, this.width - 28, 22);

    // 3. Statut Central
    ctx.textAlign = 'center';
    let statusMsg = '';
    if (state.finished) {
      statusMsg = state.winner === 1
        ? 'VICTOIRE SUD (FONCÉES)'
        : (state.winner === 2 ? 'VICTOIRE NORD (CLAIRES)' : 'ÉGALITÉ');
      ctx.fillStyle = GB_TEXT_PRIMARY;
    } else if (this.isAnimating) {
      statusMsg = 'SEMIS EN COURS...';
      ctx.fillStyle = GB_RED;
    } else if (state.turn === this.playerNum) {
      statusMsg = 'À VOTRE TOUR';
      ctx.fillStyle = GB_TEXT_PRIMARY;
    } else {
      statusMsg = this.isSolo ? 'LE BOT RÉFLÉCHIT...' : 'TOUR ADVERSE';
      ctx.fillStyle = GB_TEXT_SECONDARY;
    }

    ctx.font = '800 11px -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif';
    ctx.fillText(statusMsg, this.width / 2, 22);

    // Ligne de délimitation inférieure subtile
    ctx.strokeStyle = 'rgba(255, 255, 255, 0.08)';
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.moveTo(0, barH);
    ctx.lineTo(this.width, barH);
    ctx.stroke();
    ctx.restore();
  }

  // -------------------------------------------------------------------------
  // _drawBottomBar() — Pied de page et indice tactique
  // -------------------------------------------------------------------------

  private _drawBottomBar(ctx: CanvasRenderingContext2D, state: any): void {
    ctx.save();
    ctx.font = '600 10px -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif';
    ctx.textAlign = 'center';

    if (state.finished) {
      ctx.fillStyle = GB_TEXT_TERTIARY;
      ctx.fillText('PARTIE TERMINÉE', this.width / 2, this.height - 7);
    } else if (this.isAnimating) {
      ctx.fillStyle = GB_RED;
      ctx.fillText('RELAIS & CAPTURES EN COURS', this.width / 2, this.height - 7);
    } else if (state.turn === this.playerNum) {
      ctx.fillStyle = GB_TEXT_TERTIARY;
      ctx.fillText('CLIQUEZ SUR UNE DE VOS CASES POUR SEMER (SENS HORAIRE)', this.width / 2, this.height - 7);
    } else {
      ctx.fillStyle = GB_TEXT_TERTIARY;
      ctx.fillText(this.isSolo ? 'BOT EN ACTION' : 'EN ATTENTE DE L\'ADVERSAIRE', this.width / 2, this.height - 7);
    }
    ctx.restore();
  }

  // -------------------------------------------------------------------------
  // _drawConnecting() — Écran de chargement Gamebox
  // -------------------------------------------------------------------------

  private _drawConnecting(ctx: CanvasRenderingContext2D): void {
    ctx.fillStyle = GB_TEXT_SECONDARY;
    ctx.font = '800 11px ui-monospace, SFMono-Regular, monospace';
    ctx.textAlign = 'center';
    ctx.fillText('CHARGEMENT DU KATRO...', this.width / 2, this.height / 2);
  }

  // -------------------------------------------------------------------------
  // Utilitaires de coordonnées
  // -------------------------------------------------------------------------

  private _getCellCenter(cellIndex: number): { x: number; y: number } {
    const { startX, startY, cellW, cellH, cols } = this.getBoardLayout();
    const row = Math.floor(cellIndex / cols);
    const col = cellIndex % cols;

    return {
      x: startX + col * cellW + cellW / 2,
      y: startY + row * cellH + cellH / 2
    };
  }

  private _hitTest(x: number, y: number): number {
    const { startX, startY, boardW, boardH, cellW, cellH, cols } = this.getBoardLayout();

    if (x < startX || x > startX + boardW) return -1;
    if (y < startY || y > startY + boardH) return -1;

    const col = Math.floor((x - startX) / cellW);
    const row = Math.floor((y - startY) / cellH);

    if (col < 0 || col >= cols || row < 0 || row >= ROWS) return -1;
    return row * cols + col;
  }

  private _isMyCell(cellIndex: number): boolean {
    const half = this.currentCols * 2;
    if (this.playerNum === 1) return cellIndex < half;
    if (this.playerNum === 2) return cellIndex >= half;
    return false;
  }

  // -------------------------------------------------------------------------
  // Événements tactiles et souris
  // -------------------------------------------------------------------------

  public handlePointerDown(x: number, y: number): void {
    if (this.isAnimating) return;
    const cell = this._hitTest(x, y);
    if (cell >= 0 && this._isMyCell(cell)) {
      this.pressedCell = cell;
    }
  }

  public handlePointerMove(x: number, y: number): void {
    if (this.isAnimating) {
      this.hoveredCell = -1;
      return;
    }
    this.hoveredCell = this._hitTest(x, y);
  }

  public handlePointerUp(x: number, y: number): void {
    if (this.isAnimating) return;
    const cell = this._hitTest(x, y);
    if (cell >= 0 && cell === this.pressedCell && this._isMyCell(cell)) {
      this.dispatch({ cmd: 'sow', cell, dir: 'cw' });
      sound.click();
      haptic(20);
    }
    this.pressedCell = -1;
  }

  public destroy(): void {
    this.particles         = [];
    this.sparks            = [];
    this.beams             = [];
    this.banners           = [];
    this.pops              = [];
    this.displayBoard      = [];
    this.eventQueue        = [];
    this.pendingStateQueue = [];
    this.isAnimating       = false;
    this.activeRelayCell   = -1;
    this.hoveredCell       = -1;
    this.pressedCell       = -1;
  }
}
