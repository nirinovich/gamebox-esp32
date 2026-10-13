// ============================================================================
// KatroRenderer.ts — Moteur de présentation Canvas 2D du Katro malgache
//
// Responsabilités :
//   - Dessine le plateau Katro (4 rangées, colonnes dynamiques 4, 6 ou 8).
//   - Animation fluide du déplacement des graines pas à pas (semis élémentaire).
//   - Animation spectaculaire des captures : éclairs d'énergie, jaillissement
//     de particules depuis le camp adverse à travers la ligne médiane,
//     et intégration au semis de relais.
//   - Particules avec traînées lumineuses (arcs de Bézier).
//   - Bannières flottantes d'action (⚡ Capture, 🔄 Relais, etc.).
//   - Rendu Canvas 2D à 60 FPS avec sound/haptics synchronisés.
// ============================================================================

import { IGameRenderer, ActionDispatcher } from '../../../frontend/src/renderers/IGameRenderer';
import { sound, haptic } from '../../../frontend/src/audio/SoundEngine';

// ============================================================================
// Types d'animation
// ============================================================================

interface AnimEvent {
  t: 'pickup' | 'drop' | 'capture';
  from: number;
  to: number;
  count: number;
}

interface FlyingParticle {
  startX: number;
  startY: number;
  targetX: number;
  targetY: number;
  t: number;          // 0 .. 1
  speed: number;
  arcHeight: number;
  color: string;
  glow: string;
  size: number;
  isCapture: boolean;
  targetCell: number;
  trail: Array<{ x: number; y: number; alpha: number }>;
}

interface CaptureBeam {
  fromX: number;
  fromY: number;
  toX: number;
  toY: number;
  life: number;
  maxLife: number;
}

interface FloatingBanner {
  x: number;
  y: number;
  text: string;
  subText?: string;
  color: string;
  bgColor: string;
  alpha: number;
  life: number;
  maxLife: number;
}

interface SeedPop {
  cellIndex: number;
  frame: number;
}

// ============================================================================
// Constantes de mise en page et palette néon
// ============================================================================

const ROWS        = 4;     ///< 4 rangées (2 Sud, 2 Nord)
const POP_FRAMES  = 14;

const COLOR_BG             = '#080c14';   ///< Fond nuit ébène
const COLOR_SOUTH_OUTER    = '#0d281e';   ///< Sud extérieure (vert forêt sombre)
const COLOR_SOUTH_INNER    = '#123d2c';   ///< Sud intérieure (vert émeraude profond)
const COLOR_NORTH_INNER    = '#2d163d';   ///< Nord intérieure (violet améthyste)
const COLOR_NORTH_OUTER    = '#1b0d26';   ///< Nord extérieure (violet sombre)
const COLOR_SOUTH_HOVER    = '#10b98166'; ///< Surbrillance Sud (néon vert)
const COLOR_NORTH_HOVER    = '#a855f766'; ///< Surbrillance Nord (néon violet)
const COLOR_SEED_SOUTH     = '#34d399';   ///< Graines Sud (émeraude brillant)
const COLOR_SEED_NORTH     = '#c084fc';   ///< Graines Nord (améthyste brillant)
const COLOR_COUNT_SOUTH    = '#6ee7b7';   ///< Texte compteur Sud
const COLOR_COUNT_NORTH    = '#e9d5ff';   ///< Texte compteur Nord
const COLOR_SCORE_TEXT     = '#f59e0b';   ///< Score / Annonces (ambre vif)
const COLOR_CENTER_LINE    = '#38bdf8aa'; ///< Ligne médiane séparatrice (cyan)
const COLOR_CAPTURE_GLOW   = '#fbbf24';   ///< Éclair et particule de capture (or solaire)

// ============================================================================
// KatroRenderer — implémente IGameRenderer
// ============================================================================

export class KatroRenderer implements IGameRenderer {
  private width       = 400;
  private height      = 400;
  private dispatch: ActionDispatcher = () => {};
  private playerNum   = 1;
  private currentCols = 4;

  private hoveredCell = -1;
  private pressedCell = -1;

  // État du plateau synchronisé et affiché
  private displayBoard: number[] = [];
  private lastSeq     = -1;
  private isAnimating = false;

  // File d'événements d'animation
  private eventQueue: AnimEvent[] = [];
  private animTimer   = 0;
  private lastEventPos: { x: number; y: number } | null = null;

  // Particules et effets visuels
  private particles: FlyingParticle[] = [];
  private beams: CaptureBeam[] = [];
  private banners: FloatingBanner[] = [];
  private pops: SeedPop[] = [];
  private shakeFrames = 0;

  // Halo sur la case active lors du relais
  private activeRelayCell = -1;
  private relayHaloFrames = 0;

  public init(
    _canvas?: HTMLCanvasElement,
    _ctx?: CanvasRenderingContext2D,
    dispatch?: ActionDispatcher,
    playerNum?: number
  ): void {
    if (dispatch)  this.dispatch  = dispatch;
    if (playerNum) this.playerNum = playerNum;
    this.displayBoard    = [];
    this.lastSeq         = -1;
    this.isAnimating     = false;
    this.eventQueue      = [];
    this.particles       = [];
    this.beams           = [];
    this.banners         = [];
    this.pops            = [];
    this.shakeFrames     = 0;
    this.activeRelayCell = -1;
    this.relayHaloFrames = 0;
    this.hoveredCell     = -1;
    this.pressedCell     = -1;
    this.currentCols     = 4;
  }

  public setDispatcher(dispatch: ActionDispatcher): void {
    this.dispatch = dispatch;
  }

  public setPlayerNum(playerNum: number): void {
    this.playerNum = playerNum;
  }

  public resize(width: number, height: number): void {
    this.width  = width;
    this.height = height;
  }

  // -------------------------------------------------------------------------
  // render() — Boucle principale de rendu à 60 FPS
  // -------------------------------------------------------------------------

  public render(state: any, ctx: CanvasRenderingContext2D): void {
    if (state?.board && Array.isArray(state.board)) {
      this.currentCols = Math.max(1, Math.floor(state.board.length / ROWS));

      // Gestion de la séquence d'événements reçue du Host
      const seq = state.seq ?? 0;
      if (this.lastSeq === -1) {
        // Initialisation initiale sans animation
        this.displayBoard = [...state.board];
        this.lastSeq = seq;
      } else if (seq > this.lastSeq) {
        this.lastSeq = seq;
        if (state.events && Array.isArray(state.events) && state.events.length > 0) {
          this.eventQueue = [...state.events];
          this.isAnimating = true;
          this.animTimer = 0;
        } else {
          this.displayBoard = [...state.board];
        }
      }
    }

    // Mise à jour de la file d'événements et de la physique des particules
    this._updateAnimation(state);

    // Effet tremblement d'écran lors des captures majeures
    ctx.save();
    if (this.shakeFrames > 0) {
      const sx = (Math.random() - 0.5) * 5;
      const sy = (Math.random() - 0.5) * 5;
      ctx.translate(sx, sy);
      this.shakeFrames--;
    }

    // Fond général
    ctx.fillStyle = COLOR_BG;
    ctx.fillRect(0, 0, this.width, this.height);

    if (!state?.board) {
      this._drawConnecting(ctx);
      ctx.restore();
      return;
    }

    this._drawScoreBar(ctx, state);
    this._drawBoard(ctx, state);
    this._drawBeams(ctx);
    this._drawParticles(ctx);
    this._drawBanners(ctx);
    this._drawTurnIndicator(ctx, state);

    ctx.restore();
  }

  // -------------------------------------------------------------------------
  // _updateAnimation() — Défilement cadencé des événements de semis et captures
  // -------------------------------------------------------------------------

  private _updateAnimation(state: any): void {
    // Avance les particules existantes
    for (let i = this.particles.length - 1; i >= 0; i--) {
      const p = this.particles[i];
      p.t += p.speed;

      // Traînée
      const curX = (1 - p.t) * p.startX + p.t * p.targetX;
      const curY = (1 - p.t) * p.startY + p.t * p.targetY - p.arcHeight * Math.sin(p.t * Math.PI);
      p.trail.push({ x: curX, y: curY, alpha: 1.0 });
      if (p.trail.length > 6) p.trail.shift();

      if (p.t >= 1.0) {
        // Atterrissage de la graine
        if (p.targetCell >= 0 && p.targetCell < this.displayBoard.length) {
          this.displayBoard[p.targetCell]++;
          if (!this.pops.find(pop => pop.cellIndex === p.targetCell)) {
            this.pops.push({ cellIndex: p.targetCell, frame: 0 });
          }
        }
        if (p.isCapture) {
          sound.eat();
          haptic(30);
        } else {
          sound.place();
          haptic(8);
        }
        this.particles.splice(i, 1);
      }
    }

    // Avance les éclairs de capture
    this.beams = this.beams
      .map(b => ({ ...b, life: b.life - 1 }))
      .filter(b => b.life > 0);

    // Avance les bannières
    this.banners = this.banners
      .map(b => ({ ...b, y: b.y - 0.7, life: b.life - 1, alpha: b.life / b.maxLife }))
      .filter(b => b.life > 0);

    // Avance les rebonds pop
    this.pops = this.pops
      .map(p => ({ ...p, frame: p.frame + 1 }))
      .filter(p => p.frame < POP_FRAMES);

    if (this.relayHaloFrames > 0) this.relayHaloFrames--;

    // Traite le prochain événement de la file d'attente
    if (this.eventQueue.length > 0) {
      this.animTimer++;
      const nextEv = this.eventQueue[0];
      // Délai adapté : plus long sur une capture pour savourer le coup
      const delayThreshold = (nextEv.t === 'capture') ? 18 : (nextEv.t === 'pickup' ? 8 : 6);

      if (this.animTimer >= delayThreshold) {
        this.animTimer = 0;
        const ev = this.eventQueue.shift()!;
        this._processAnimEvent(ev);
      }
    } else if (this.particles.length === 0) {
      // Fin de toute l'animation
      this.isAnimating = false;
      this.activeRelayCell = -1;
      if (state?.board) {
        this.displayBoard = [...state.board];
      }
    }
  }

  // -------------------------------------------------------------------------
  // _processAnimEvent() — Exécution d'un événement élémentaire (pickup, drop, capture)
  // -------------------------------------------------------------------------

  private _processAnimEvent(ev: AnimEvent): void {
    if (ev.t === 'pickup') {
      // Ramasse les graines de la case
      if (ev.from >= 0 && ev.from < this.displayBoard.length) {
        this.displayBoard[ev.from] = Math.max(0, this.displayBoard[ev.from] - ev.count);
        this.activeRelayCell = ev.from;
        this.relayHaloFrames = 18;

        const pos = this._getCellCenter(ev.from);
        this.lastEventPos = pos;

        // Si c'est un relais
        if (ev.count > 2) {
          this.banners.push({
            x: pos.x,
            y: pos.y - 14,
            text: `🔄 RELAIS (${ev.count})`,
            color: '#38bdf8',
            bgColor: '#0369a1dd',
            alpha: 1.0,
            life: 28,
            maxLife: 28
          });
        }
      }
    } else if (ev.t === 'drop') {
      // Dépôt d'une graine dans la case suivante
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
          arcHeight: 20,
          color: isSouth ? COLOR_SEED_SOUTH : COLOR_SEED_NORTH,
          glow: isSouth ? '#10b981' : '#a855f7',
          size: 5.5,
          isCapture: false,
          targetCell: ev.to,
          trail: []
        });

        this.lastEventPos = targetPos;
      }
    } else if (ev.t === 'capture') {
      // ⚡ ÉVÉNEMENT MAJEUR : CAPTURE DES GRAINES ADVERSES !
      if (ev.from >= 0 && ev.to >= 0) {
        const fromPos = this._getCellCenter(ev.from);
        const toPos   = this._getCellCenter(ev.to);

        // Vider visuellement la case adverse capturée
        if (ev.from < this.displayBoard.length) {
          this.displayBoard[ev.from] = 0;
        }

        // Faisceau lumineux d'énergie reliant les deux cases face-à-face
        this.beams.push({
          fromX: fromPos.x,
          fromY: fromPos.y,
          toX: toPos.x,
          toY: toPos.y,
          life: 16,
          maxLife: 16
        });

        this.shakeFrames = 7;
        sound.eat();
        haptic(45);

        // Vol en cloche de N graines capturées depuis la case adverse
        const seedCount = Math.max(1, Math.min(ev.count, 8));
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
            color: COLOR_CAPTURE_GLOW,
            glow: '#fbbf24',
            size: 6.5,
            isCapture: true,
            targetCell: ev.to,
            trail: []
          });
        }

        // Bannière dorée spectaculaire
        this.banners.push({
          x: (fromPos.x + toPos.x) / 2,
          y: (fromPos.y + toPos.y) / 2 - 8,
          text: `⚡ PRISE +${ev.count} !`,
          color: '#ffffff',
          bgColor: '#b45309ee',
          alpha: 1.0,
          life: 38,
          maxLife: 38
        });

        this.lastEventPos = toPos;
      }
    }
  }

  // -------------------------------------------------------------------------
  // _drawScoreBar() — Barre de score supérieure
  // -------------------------------------------------------------------------

  private _drawScoreBar(ctx: CanvasRenderingContext2D, state: any): void {
    const barH = 38;
    ctx.fillStyle = '#0f172aee';
    ctx.fillRect(0, 0, this.width, barH);

    ctx.fillStyle = COLOR_SEED_SOUTH;
    ctx.font = 'bold 13px "JetBrains Mono", monospace';
    ctx.textAlign = 'left';
    ctx.fillText(`Sud (P1) ▶ ${state.scores?.[0] ?? 0} graines`, 12, 24);

    ctx.fillStyle = COLOR_SEED_NORTH;
    ctx.textAlign = 'right';
    ctx.fillText(`${state.scores?.[1] ?? 0} graines ◀ Nord (P2)`, this.width - 12, 24);

    if (state.finished) {
      ctx.fillStyle = COLOR_SCORE_TEXT;
      ctx.textAlign = 'center';
      const msg = state.winner === -1
        ? '⚖ Égalité !'
        : `🏆 ${state.winner === 1 ? 'Sud' : 'Nord'} gagne !`;
      ctx.fillText(msg, this.width / 2, 24);
    } else if (this.isAnimating) {
      ctx.fillStyle = '#38bdf8';
      ctx.font = 'italic 11px sans-serif';
      ctx.textAlign = 'center';
      ctx.fillText('⏳ Semis en cours…', this.width / 2, 24);
    }
  }

  // -------------------------------------------------------------------------
  // _drawBoard() — Dessin des 4 rangées
  // -------------------------------------------------------------------------

  private _drawBoard(ctx: CanvasRenderingContext2D, state: any): void {
    const cols      = this.currentCols;
    const topOffset = 42;
    const boardH    = this.height - topOffset - 12;
    const cellW     = this.width / cols;
    const cellH     = boardH / ROWS;

    for (let row = 0; row < ROWS; row++) {
      for (let col = 0; col < cols; col++) {
        const cellIndex = row * cols + col;
        const seeds     = this.displayBoard[cellIndex] ?? (state.board?.[cellIndex] ?? 0);

        const isSouth   = row < 2;
        const isInner   = (row === 1 || row === 2);

        const x = col * cellW;
        const y = topOffset + row * cellH;

        let bgColor = COLOR_SOUTH_OUTER;
        if (isSouth) {
          bgColor = isInner ? COLOR_SOUTH_INNER : COLOR_SOUTH_OUTER;
        } else {
          bgColor = isInner ? COLOR_NORTH_INNER : COLOR_NORTH_OUTER;
        }

        ctx.fillStyle = bgColor;
        ctx.fillRect(x + 2, y + 2, cellW - 4, cellH - 4);

        ctx.strokeStyle = isInner ? '#374151' : '#1f2937';
        ctx.lineWidth = 1;
        ctx.strokeRect(x + 2, y + 2, cellW - 4, cellH - 4);

        // Halo jaune/ambre sur la case active de relais
        if (cellIndex === this.activeRelayCell && this.relayHaloFrames > 0) {
          ctx.strokeStyle = '#f59e0b';
          ctx.lineWidth = 3;
          ctx.strokeRect(x + 2, y + 2, cellW - 4, cellH - 4);
        }

        // Surbrillance au survol (seulement si non animé)
        if (!this.isAnimating && (cellIndex === this.hoveredCell || cellIndex === this.pressedCell)) {
          const myCell = isSouth ? (this.playerNum === 1) : (this.playerNum === 2);
          if (myCell && !state.finished && state.turn === this.playerNum && seeds > 0) {
            ctx.fillStyle = isSouth ? COLOR_SOUTH_HOVER : COLOR_NORTH_HOVER;
            ctx.fillRect(x + 2, y + 2, cellW - 4, cellH - 4);
          }
        }

        // Dessin des graines dans la case
        this._drawSeeds(ctx, x, y, cellW, cellH, seeds, isSouth, cellIndex);

        // Compteur numérique
        if (seeds > 0) {
          ctx.fillStyle = isSouth ? COLOR_COUNT_SOUTH : COLOR_COUNT_NORTH;
          ctx.font = 'bold 11px sans-serif';
          ctx.textAlign = 'center';
          ctx.fillText(String(seeds), x + cellW / 2, y + cellH - 5);
        }
      }
    }

    // Ligne médiane séparatrice de capture face-à-face
    const midY = topOffset + 2 * cellH;
    ctx.strokeStyle = COLOR_CENTER_LINE;
    ctx.lineWidth = 2;
    ctx.beginPath();
    ctx.moveTo(4, midY);
    ctx.lineTo(this.width - 4, midY);
    ctx.stroke();

    ctx.fillStyle = COLOR_CENTER_LINE;
    ctx.font = '10px monospace';
    ctx.textAlign = 'center';
  }

  // -------------------------------------------------------------------------
  // _drawSeeds() — Représentation des graines
  // -------------------------------------------------------------------------

  private _drawSeeds(
    ctx: CanvasRenderingContext2D,
    x: number, y: number,
    cellW: number, cellH: number,
    seeds: number,
    isSouth: boolean,
    cellIndex: number
  ): void {
    const maxDots = Math.min(seeds, 16);
    const cols    = maxDots <= 4 ? 2 : 4;
    const rows    = Math.ceil(maxDots / cols);
    const dotR    = Math.min(cellW, cellH) * 0.085;
    const padX    = cellW * 0.18;
    const padY    = cellH * 0.14;
    const stepX   = (cellW - padX * 2) / Math.max(cols - 1, 1);
    const stepY   = (cellH - padY * 2 - 14) / Math.max(rows - 1, 1);

    let scale = 1.0;
    const pop = this.pops.find(p => p.cellIndex === cellIndex);
    if (pop) {
      scale = 1.0 + 0.35 * Math.sin((pop.frame / POP_FRAMES) * Math.PI);
    }

    ctx.fillStyle = isSouth ? COLOR_SEED_SOUTH : COLOR_SEED_NORTH;

    for (let i = 0; i < maxDots; i++) {
      const c = i % cols;
      const r = Math.floor(i / cols);
      const cx = x + padX + c * stepX;
      const cy = y + padY + r * stepY;

      ctx.beginPath();
      ctx.arc(cx, cy, Math.max(2, dotR * scale), 0, Math.PI * 2);
      ctx.fill();
    }
  }

  // -------------------------------------------------------------------------
  // _drawBeams() — Éclairs d'énergie lors des captures
  // -------------------------------------------------------------------------

  private _drawBeams(ctx: CanvasRenderingContext2D): void {
    for (const b of this.beams) {
      const alpha = b.life / b.maxLife;
      ctx.strokeStyle = `rgba(251, 191, 36, ${alpha})`;
      ctx.lineWidth = 3;
      ctx.beginPath();
      ctx.moveTo(b.fromX, b.fromY);

      // Trait d'éclair avec brisure centrale
      const midX = (b.fromX + b.toX) / 2 + (Math.random() - 0.5) * 12;
      const midY = (b.fromY + b.toY) / 2;
      ctx.lineTo(midX, midY);
      ctx.lineTo(b.toX, b.toY);
      ctx.stroke();

      // Halo externe
      ctx.strokeStyle = `rgba(56, 189, 248, ${alpha * 0.5})`;
      ctx.lineWidth = 6;
      ctx.stroke();
    }
  }

  // -------------------------------------------------------------------------
  // _drawParticles() — Particules de graines volantes en arc
  // -------------------------------------------------------------------------

  private _drawParticles(ctx: CanvasRenderingContext2D): void {
    for (const p of this.particles) {
      // Dessin de la traînée
      for (let i = 0; i < p.trail.length; i++) {
        const pt = p.trail[i];
        const a = (i / p.trail.length) * 0.6;
        ctx.fillStyle = p.isCapture ? `rgba(251, 191, 36, ${a})` : `rgba(52, 211, 153, ${a})`;
        ctx.beginPath();
        ctx.arc(pt.x, pt.y, p.size * 0.6, 0, Math.PI * 2);
        ctx.fill();
      }

      // Position courante
      const curX = (1 - p.t) * p.startX + p.t * p.targetX;
      const curY = (1 - p.t) * p.startY + p.t * p.targetY - p.arcHeight * Math.sin(p.t * Math.PI);

      // Halo de lueur
      ctx.fillStyle = p.glow;
      ctx.shadowColor = p.glow;
      ctx.shadowBlur = p.isCapture ? 12 : 6;

      ctx.beginPath();
      ctx.arc(curX, curY, p.size, 0, Math.PI * 2);
      ctx.fillStyle = p.color;
      ctx.fill();

      ctx.shadowBlur = 0; // Réinitialise l'ombre
    }
  }

  // -------------------------------------------------------------------------
  // _drawBanners() — Bannières flottantes de notification
  // -------------------------------------------------------------------------

  private _drawBanners(ctx: CanvasRenderingContext2D): void {
    for (const b of this.banners) {
      ctx.save();
      ctx.globalAlpha = b.alpha;

      ctx.font = 'bold 12px sans-serif';
      const textWidth = ctx.measureText(b.text).width;
      const padX = 10;
      const padY = 5;

      // Fond de pilule arrondi
      ctx.fillStyle = b.bgColor;
      ctx.beginPath();
      ctx.roundRect(b.x - textWidth / 2 - padX, b.y - 12 - padY, textWidth + padX * 2, 22, 10);
      ctx.fill();

      // Texte
      ctx.fillStyle = b.color;
      ctx.textAlign = 'center';
      ctx.fillText(b.text, b.x, b.y + 3);

      ctx.restore();
    }
  }

  // -------------------------------------------------------------------------
  // _drawTurnIndicator() — Flèche de tour
  // -------------------------------------------------------------------------

  private _drawTurnIndicator(ctx: CanvasRenderingContext2D, state: any): void {
    if (state.finished) return;

    const cellH = (this.height - 42 - 12) / ROWS;
    const isSouthTurn = state.turn === 1;

    const yPos = isSouthTurn
      ? 42 + 1.5 * cellH
      : 42 + 0.5 * cellH;

    ctx.fillStyle = isSouthTurn ? COLOR_SEED_SOUTH : COLOR_SEED_NORTH;
    ctx.font = 'bold 15px sans-serif';
    ctx.textAlign = 'right';
    ctx.fillText('▶', this.width - 6, yPos);
  }

  // -------------------------------------------------------------------------
  // _drawConnecting() — Chargement
  // -------------------------------------------------------------------------

  private _drawConnecting(ctx: CanvasRenderingContext2D): void {
    ctx.fillStyle = '#9ca3af';
    ctx.font = '14px sans-serif';
    ctx.textAlign = 'center';
    ctx.fillText('Chargement du Katro tsotra…', this.width / 2, this.height / 2);
  }

  // -------------------------------------------------------------------------
  // Utilitaires de coordonnées
  // -------------------------------------------------------------------------

  private _getCellCenter(cellIndex: number): { x: number; y: number } {
    const cols      = this.currentCols;
    const topOffset = 42;
    const boardH    = this.height - topOffset - 12;
    const cellW     = this.width / cols;
    const cellH     = boardH / ROWS;

    const row = Math.floor(cellIndex / cols);
    const col = cellIndex % cols;

    return {
      x: col * cellW + cellW / 2,
      y: topOffset + row * cellH + cellH / 2
    };
  }

  private _hitTest(x: number, y: number): number {
    const cols      = this.currentCols;
    const topOffset = 42;
    const boardH    = this.height - topOffset - 12;
    const cellW     = this.width / cols;
    const cellH     = boardH / ROWS;

    if (y < topOffset || y > topOffset + boardH) return -1;
    if (x < 0 || x >= this.width) return -1;

    const col = Math.floor(x / cellW);
    const row = Math.floor((y - topOffset) / cellH);

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
    if (this.isAnimating) return; // Bloqué pendant l'animation pour éviter tout conflit
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
    this.particles    = [];
    this.beams        = [];
    this.banners      = [];
    this.pops         = [];
    this.displayBoard = [];
  }
}
