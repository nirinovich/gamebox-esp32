import { IGameRenderer, ActionDispatcher } from '../../../frontend/src/renderers/IGameRenderer';
import { sound, haptic } from '../../../frontend/src/audio/SoundEngine';
import { CameraJuice } from '../../../frontend/src/renderers/CameraJuice';

export interface PendingMoveChoice {
  from: number;
  to: number;
  approachCaptures: number[];
  withdrawalCaptures: number[];
}

export interface MovingStoneAnim {
  fromNode: number;
  toNode: number;
  startX: number;
  startY: number;
  targetX: number;
  targetY: number;
  startTime: number;
  duration: number;
  player: number;
  captured: number[];
}

export interface CapturedStoneAnim {
  node: number;
  x: number;
  y: number;
  startTime: number;
  duration: number;
  player: number;
}

export interface FanoronaSpark {
  x: number;
  y: number;
  vx: number;
  vy: number;
  life: number;
  decay: number;
  color: string;
  size: number;
}

export class FanoronaRenderer implements IGameRenderer {
  private width = 450;
  private height = 320;
  public selectedNode = -1;
  public pendingChoice: PendingMoveChoice | null = null;
  private lastState: any = null;
  private myPlayerNum = 1;
  private isSolo = false;
  private dispatch: ActionDispatcher = () => {};
  private juice?: CameraJuice;

  // Animation systems
  public movingStone: MovingStoneAnim | null = null;
  public capturedStones: CapturedStoneAnim[] = [];
  public sparks: FanoronaSpark[] = [];
  private lastProcessedMoveId = -1;
  private lastAnimTime = 0;
  private botTimer: any = null;
  private optimisticMove: { from: number; to: number; captured: number[] } | null = null;

  constructor(juice?: CameraJuice) {
    this.juice = juice;
  }

  public init(_canvas?: HTMLCanvasElement, _ctx?: CanvasRenderingContext2D, dispatch?: ActionDispatcher, playerNum?: number) {
    if (playerNum) this.myPlayerNum = playerNum;
    if (dispatch) this.dispatch = dispatch;
    this.destroy();
  }

  public setPlayerNum(playerNum: number) {
    this.myPlayerNum = playerNum;
  }

  public setIsSolo(isSolo: boolean) {
    this.isSolo = isSolo;
  }

  public setDispatcher(dispatch: ActionDispatcher) {
    this.dispatch = dispatch;
  }

  public resize(width: number, height: number) {
    this.width = width;
    this.height = height;
  }

  public isStrongNode(r: number, c: number): boolean {
    return (r + c) % 2 === 0;
  }

  public isConnected(from: number, to: number): boolean {
    if (from < 0 || from >= 45 || to < 0 || to >= 45) return false;
    const r1 = Math.floor(from / 9), c1 = from % 9;
    const r2 = Math.floor(to / 9), c2 = to % 9;
    const dr = Math.abs(r1 - r2);
    const dc = Math.abs(c1 - c2);

    // Orthogonal
    if ((dr === 1 && dc === 0) || (dr === 0 && dc === 1)) return true;

    // Diagonal: ONLY if both are strong nodes (r+c is even)
    if (dr === 1 && dc === 1 && this.isStrongNode(r1, c1)) return true;

    return false;
  }

  public getApproachCaptures(from: number, to: number, board: number[]): number[] {
    const res: number[] = [];
    if (!this.isConnected(from, to) || !board) return res;
    const player = board[from];
    if (player !== 1 && player !== 2) return res;
    const opp = player === 1 ? 2 : 1;

    const r1 = Math.floor(from / 9), c1 = from % 9;
    const r2 = Math.floor(to / 9), c2 = to % 9;
    const dr = r2 - r1;
    const dc = c2 - c1;

    let currR = r2 + dr;
    let currC = c2 + dc;
    while (currR >= 0 && currR < 5 && currC >= 0 && currC < 9) {
      const node = currR * 9 + currC;
      if (board[node] === opp) {
        res.push(node);
        currR += dr;
        currC += dc;
      } else {
        break;
      }
    }
    return res;
  }

  public getWithdrawalCaptures(from: number, to: number, board: number[]): number[] {
    const res: number[] = [];
    if (!this.isConnected(from, to) || !board) return res;
    const player = board[from];
    if (player !== 1 && player !== 2) return res;
    const opp = player === 1 ? 2 : 1;

    const r1 = Math.floor(from / 9), c1 = from % 9;
    const r2 = Math.floor(to / 9), c2 = to % 9;
    const dr = r2 - r1;
    const dc = c2 - c1;

    let currR = r1 - dr;
    let currC = c1 - dc;
    while (currR >= 0 && currR < 5 && currC >= 0 && currC < 9) {
      const node = currR * 9 + currC;
      if (board[node] === opp) {
        res.push(node);
        currR -= dr;
        currC -= dc;
      } else {
        break;
      }
    }
    return res;
  }

  public getBoardLayout() {
    const padX = 36;
    const topPad = 48;
    const botPad = 44;
    const availW = this.width - padX * 2;
    const availH = this.height - topPad - botPad;

    const stepX = availW / 8;
    const stepY = availH / 4;
    const step = Math.min(stepX, stepY);

    const gridW = step * 8;
    const gridH = step * 4;
    const startX = (this.width - gridW) / 2;
    const startY = topPad + (availH - gridH) / 2;

    return { startX, startY, step, radius: Math.max(10, step * 0.38) };
  }

  public drawStoneSphere(ctx: CanvasRenderingContext2D, x: number, y: number, r: number, player: number) {
    if (player === 1) {
      // Player 1: Dark Slate-Blue Ball (top half in image)
      const stoneGrad = ctx.createRadialGradient(x - r * 0.35, y - r * 0.35, r * 0.05, x, y, r);
      stoneGrad.addColorStop(0, '#64748b');
      stoneGrad.addColorStop(0.35, '#334155');
      stoneGrad.addColorStop(0.85, '#1e293b');
      stoneGrad.addColorStop(1, '#0f172a');

      ctx.fillStyle = stoneGrad;
      ctx.beginPath();
      ctx.arc(x, y, r, 0, Math.PI * 2);
      ctx.fill();

      ctx.strokeStyle = '#1e293b';
      ctx.lineWidth = 1;
      ctx.stroke();
    } else if (player === 2) {
      // Player 2: Terracotta / Coral Orange-Red Ball (bottom half in image)
      const stoneGrad = ctx.createRadialGradient(x - r * 0.35, y - r * 0.35, r * 0.05, x, y, r);
      stoneGrad.addColorStop(0, '#fb923c');
      stoneGrad.addColorStop(0.35, '#ea580c');
      stoneGrad.addColorStop(0.85, '#c2410c');
      stoneGrad.addColorStop(1, '#7c2d12');

      ctx.fillStyle = stoneGrad;
      ctx.beginPath();
      ctx.arc(x, y, r, 0, Math.PI * 2);
      ctx.fill();

      ctx.strokeStyle = '#9a3412';
      ctx.lineWidth = 1;
      ctx.stroke();
    }
  }

  public startMoveAnimation(from: number, to: number, player: number, captured: number[]) {
    const { startX, startY, step } = this.getBoardLayout();
    const c1 = from % 9, r1 = Math.floor(from / 9);
    const c2 = to % 9, r2 = Math.floor(to / 9);

    this.movingStone = {
      fromNode: from,
      toNode: to,
      startX: startX + c1 * step,
      startY: startY + r1 * step,
      targetX: startX + c2 * step,
      targetY: startY + r2 * step,
      startTime: performance.now(),
      duration: 220,
      player,
      captured: captured || []
    };

    sound.place();
    haptic(15);
  }

  public triggerCaptures(nodes: number[], capturedPlayer: number) {
    if (!nodes || nodes.length === 0) return;
    const { startX, startY, step } = this.getBoardLayout();
    const now = performance.now();

    sound.eat();
    haptic(25);
    this.juice?.shake(3.5);

    for (const node of nodes) {
      if (this.capturedStones.some(c => c.node === node)) continue;

      const c = node % 9;
      const r = Math.floor(node / 9);
      const nx = startX + c * step;
      const ny = startY + r * step;

      this.capturedStones.push({
        node,
        x: nx,
        y: ny,
        startTime: now,
        duration: 350,
        player: capturedPlayer
      });

      // Spawn bright spark particles
      const sparkCount = 10;
      const baseColor = capturedPlayer === 1 ? '#38bdf8' : '#ea580c';
      const altColor = capturedPlayer === 1 ? '#f8fafc' : '#fef08a';

      for (let s = 0; s < sparkCount; s++) {
        const angle = (Math.PI * 2 * s) / sparkCount + (Math.random() - 0.5) * 0.4;
        const speed = 65 + Math.random() * 80;
        this.sparks.push({
          x: nx,
          y: ny,
          vx: Math.cos(angle) * speed,
          vy: Math.sin(angle) * speed,
          life: 1.0,
          decay: 2.2 + Math.random() * 1.0,
          color: Math.random() > 0.4 ? baseColor : altColor,
          size: 2.5 + Math.random() * 2
        });
      }
    }
  }

  private checkBotTurn(state: any) {
    if (!this.isSolo || !state || state.finished) return;
    if (state.turn === 2 && !this.botTimer) {
      this.botTimer = window.setTimeout(() => {
        this.botTimer = null;
        if (this.lastState && this.lastState.turn === 2 && !this.lastState.finished) {
          this.dispatch({ cmd: 'bot_move' });
        }
      }, 550);
    }
  }

  public render(state: any, ctx: CanvasRenderingContext2D) {
    this.lastState = state;
    const { width, height } = this;
    const now = performance.now();
    const dt = this.lastAnimTime ? Math.min(0.1, (now - this.lastAnimTime) / 1000) : 0.016;
    this.lastAnimTime = now;

    // Process server move updates for animation
    if (state?.last_move && typeof state.last_move.id === 'number') {
      if (this.lastProcessedMoveId === -1) {
        // Initial state on join/load
        this.lastProcessedMoveId = state.last_move.id;
      } else if (state.last_move.id > this.lastProcessedMoveId) {
        this.lastProcessedMoveId = state.last_move.id;
        const lm = state.last_move;

        const isOptimistic = this.optimisticMove &&
          this.optimisticMove.from === lm.from &&
          this.optimisticMove.to === lm.to;

        if (!isOptimistic && lm.from >= 0 && lm.to >= 0) {
          this.startMoveAnimation(lm.from, lm.to, lm.player, lm.captured || []);
        } else if (isOptimistic) {
          this.optimisticMove = null;
        }
      }
    }

    // Schedule solo bot move if applicable
    this.checkBotTurn(state);

    // 1. Sleek Framing with Traditional Warm Canvas Table
    ctx.fillStyle = '#0f172a';
    ctx.fillRect(0, 0, width, height);

    const { startX, startY, step, radius } = this.getBoardLayout();

    // 2. Board Canvas / Parchment Surface
    const boardPad = step * 0.5;
    const boardW = step * 8 + boardPad * 2;
    const boardH = step * 4 + boardPad * 2;
    const boardX = startX - boardPad;
    const boardY = startY - boardPad;

    ctx.save();
    ctx.shadowBlur = 16;
    ctx.shadowColor = 'rgba(0, 0, 0, 0.6)';
    ctx.shadowOffsetY = 4;
    ctx.fillStyle = '#ede8dd';
    ctx.beginPath();
    ctx.roundRect(boardX, boardY, boardW, boardH, 10);
    ctx.fill();
    ctx.strokeStyle = '#d4cbbd';
    ctx.lineWidth = 2;
    ctx.stroke();
    ctx.restore();

    // 3. Grid Lines (Horizontal, Vertical, Traditional Diagonals)
    ctx.lineWidth = 2.0;
    ctx.strokeStyle = '#6d4c30';

    // Horizontal Lines
    for (let r = 0; r < 5; r++) {
      ctx.beginPath();
      ctx.moveTo(startX, startY + r * step);
      ctx.lineTo(startX + 8 * step, startY + r * step);
      ctx.stroke();
    }

    // Vertical Lines
    for (let c = 0; c < 9; c++) {
      ctx.beginPath();
      ctx.moveTo(startX + c * step, startY);
      ctx.lineTo(startX + c * step, startY + 4 * step);
      ctx.stroke();
    }

    // Diagonals (Strictly on Strong Intersections)
    ctx.strokeStyle = '#6d4c30';
    ctx.lineWidth = 1.8;
    for (let r = 0; r < 4; r++) {
      for (let c = 0; c < 8; c++) {
        if (this.isStrongNode(r, c)) {
          // Diagonal Down-Right
          ctx.beginPath();
          ctx.moveTo(startX + c * step, startY + r * step);
          ctx.lineTo(startX + (c + 1) * step, startY + (r + 1) * step);
          ctx.stroke();
        }
        if (this.isStrongNode(r, c + 1)) {
          // Diagonal Down-Left
          ctx.beginPath();
          ctx.moveTo(startX + (c + 1) * step, startY + r * step);
          ctx.lineTo(startX + c * step, startY + (r + 1) * step);
          ctx.stroke();
        }
      }
    }

    // Center node (Lakabe) subtle ring
    const cx = startX + 4 * step;
    const cy = startY + 2 * step;
    ctx.strokeStyle = '#8d633d';
    ctx.lineWidth = 1.2;
    ctx.beginPath();
    ctx.arc(cx, cy, step * 0.22, 0, Math.PI * 2);
    ctx.stroke();

    // 4. Multi-capture visited trail
    if (state?.in_multi && state?.visited && state.visited.length > 1) {
      ctx.strokeStyle = '#eab308';
      ctx.lineWidth = 3.5;
      ctx.setLineDash([5, 4]);
      ctx.beginPath();
      for (let v = 0; v < state.visited.length; v++) {
        const vi = state.visited[v];
        const vx = startX + (vi % 9) * step;
        const vy = startY + Math.floor(vi / 9) * step;
        if (v === 0) ctx.moveTo(vx, vy);
        else ctx.lineTo(vx, vy);
      }
      ctx.stroke();
      ctx.setLineDash([]);
    }

    // 5. Draw Static Stones (suppressing moving & captured stones)
    if (state?.board) {
      for (let i = 0; i < 45; i++) {
        const val = state.board[i];
        if (val === 0) continue;

        // Suppress drawing if stone is actively sliding from or to this node
        if (this.movingStone && (this.movingStone.fromNode === i || this.movingStone.toNode === i)) {
          continue;
        }

        // Suppress drawing if stone is actively disintegrating upon capture
        if (this.capturedStones.some(c => c.node === i)) {
          continue;
        }

        const r = Math.floor(i / 9);
        const c = i % 9;
        const nx = startX + c * step;
        const ny = startY + r * step;

        ctx.save();
        ctx.shadowBlur = 5;
        ctx.shadowColor = 'rgba(20, 15, 10, 0.45)';
        ctx.shadowOffsetX = 1;
        ctx.shadowOffsetY = 3;

        this.drawStoneSphere(ctx, nx, ny, radius, val);
        ctx.restore();
      }
    }

    // 6. Draw Moving Stone Animation
    if (this.movingStone) {
      const p = Math.min(1, (now - this.movingStone.startTime) / this.movingStone.duration);
      const ease = 1 - Math.pow(1 - p, 3);
      const curX = this.movingStone.startX + (this.movingStone.targetX - this.movingStone.startX) * ease;
      const curY = this.movingStone.startY + (this.movingStone.targetY - this.movingStone.startY) * ease;

      ctx.save();
      ctx.shadowBlur = 10;
      ctx.shadowColor = 'rgba(0, 0, 0, 0.55)';
      ctx.shadowOffsetX = 2;
      ctx.shadowOffsetY = 6;

      this.drawStoneSphere(ctx, curX, curY, radius * 1.06, this.movingStone.player);
      ctx.restore();

      if (p >= 1) {
        if (this.movingStone.captured && this.movingStone.captured.length > 0) {
          this.triggerCaptures(this.movingStone.captured, this.movingStone.player === 1 ? 2 : 1);
        }
        this.movingStone = null;
        this.juice?.shake(1.5);
      }
    }

    // 7. Draw Disintegrating Captured Stones
    for (let i = this.capturedStones.length - 1; i >= 0; i--) {
      const c = this.capturedStones[i];
      const p = Math.min(1, (now - c.startTime) / c.duration);
      if (p >= 1) {
        this.capturedStones.splice(i, 1);
        continue;
      }

      const alpha = Math.max(0, 1 - p * 1.15);
      const scale = p < 0.22 ? 1 + p * 0.75 : Math.max(0, 1.16 - (p - 0.22) * 1.48);

      // Expanding glowing shockwave ring
      const ringRadius = radius * (1 + p * 1.5);
      const ringAlpha = (1 - p) * 0.8;
      ctx.save();
      ctx.strokeStyle = c.player === 1 ? `rgba(56, 189, 248, ${ringAlpha})` : `rgba(249, 115, 22, ${ringAlpha})`;
      ctx.lineWidth = 2.5 * (1 - p);
      ctx.beginPath();
      ctx.arc(c.x, c.y, ringRadius, 0, Math.PI * 2);
      ctx.stroke();

      // Shrinking stone
      if (scale > 0.05 && alpha > 0.05) {
        ctx.globalAlpha = alpha;
        this.drawStoneSphere(ctx, c.x, c.y, radius * scale, c.player);
      }
      ctx.restore();
    }

    // 8. Draw and Update Spark Particles
    for (let i = this.sparks.length - 1; i >= 0; i--) {
      const sp = this.sparks[i];
      sp.x += sp.vx * dt;
      sp.y += sp.vy * dt;
      sp.vx *= 0.94;
      sp.vy *= 0.94;
      sp.life -= sp.decay * dt;

      if (sp.life <= 0) {
        this.sparks.splice(i, 1);
        continue;
      }

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

    // 9. Highlight Active Piece or Selected Node
    const activeNode = (state?.in_multi && state?.active_piece >= 0) ? state.active_piece : this.selectedNode;
    if (activeNode >= 0) {
      const ar = Math.floor(activeNode / 9);
      const ac = activeNode % 9;
      const ax = startX + ac * step;
      const ay = startY + ar * step;

      ctx.save();
      ctx.strokeStyle = '#38bdf8';
      ctx.shadowBlur = 10;
      ctx.shadowColor = '#38bdf8';
      ctx.lineWidth = 2.5;
      ctx.beginPath();
      ctx.arc(ax, ay, radius + 4, 0, Math.PI * 2);
      ctx.stroke();
      ctx.restore();

      // Destination targets for active/selected node
      if (state?.board) {
        for (let target = 0; target < 45; target++) {
          if (state.board[target] !== 0) continue;
          if (!this.isConnected(activeNode, target)) continue;

          // In multi-capture constraints
          if (state?.in_multi) {
            if (state.visited?.includes(target)) continue;
          }

          const app = this.getApproachCaptures(activeNode, target, state.board);
          const wit = this.getWithdrawalCaptures(activeNode, target, state.board);
          const hasCap = app.length > 0 || wit.length > 0;

          if (state?.must_capture && !hasCap) continue;

          const tr = Math.floor(target / 9);
          const tc = target % 9;
          const tx = startX + tc * step;
          const ty = startY + tr * step;

          ctx.save();
          ctx.fillStyle = hasCap ? 'rgba(239, 68, 68, 0.4)' : 'rgba(16, 185, 129, 0.4)';
          ctx.strokeStyle = hasCap ? '#ef4444' : '#10b981';
          ctx.lineWidth = 2;
          ctx.beginPath();
          ctx.arc(tx, ty, radius * 0.5, 0, Math.PI * 2);
          ctx.fill();
          ctx.stroke();
          ctx.restore();
        }
      }
    }

    // 10. Dual Capture Choice Modal Overlay
    if (this.pendingChoice) {
      const { approachCaptures, withdrawalCaptures } = this.pendingChoice;
      ctx.fillStyle = 'rgba(0, 0, 0, 0.75)';
      ctx.fillRect(0, 0, width, height);

      const modalW = 280;
      const modalH = 110;
      const mx = (width - modalW) / 2;
      const my = (height - modalH) / 2;

      ctx.fillStyle = '#1e1b18';
      ctx.strokeStyle = '#c29b62';
      ctx.lineWidth = 2;
      ctx.beginPath();
      ctx.roundRect(mx, my, modalW, modalH, 8);
      ctx.fill();
      ctx.stroke();

      ctx.fillStyle = '#f8fafc';
      ctx.font = '600 13px system-ui, sans-serif';
      ctx.textAlign = 'center';
      ctx.fillText('Choisir le mode de capture :', width / 2, my + 26);

      // Button 1: Percussion (Approach)
      const btn1X = mx + 16;
      const btn1Y = my + 44;
      const btnW = 116;
      const btnH = 42;

      ctx.fillStyle = '#b91c1c';
      ctx.beginPath();
      ctx.roundRect(btn1X, btn1Y, btnW, btnH, 6);
      ctx.fill();
      ctx.fillStyle = '#ffffff';
      ctx.font = '600 11px system-ui, sans-serif';
      ctx.fillText('PERCUSSION', btn1X + btnW / 2, btn1Y + 18);
      ctx.font = '400 10px system-ui, sans-serif';
      ctx.fillText(`(${approachCaptures.length} pierre${approachCaptures.length > 1 ? 's' : ''})`, btn1X + btnW / 2, btn1Y + 32);

      // Button 2: Aspiration (Withdrawal)
      const btn2X = mx + modalW - 16 - btnW;
      const btn2Y = my + 44;
      ctx.fillStyle = '#0369a1';
      ctx.beginPath();
      ctx.roundRect(btn2X, btn2Y, btnW, btnH, 6);
      ctx.fill();
      ctx.fillStyle = '#ffffff';
      ctx.font = '600 11px system-ui, sans-serif';
      ctx.fillText('ASPIRATION', btn2X + btnW / 2, btn2Y + 18);
      ctx.font = '400 10px system-ui, sans-serif';
      ctx.fillText(`(${withdrawalCaptures.length} pierre${withdrawalCaptures.length > 1 ? 's' : ''})`, btn2X + btnW / 2, btn2Y + 32);
    }

    // 11. HUD & Stone Counts
    ctx.textAlign = 'left';
    ctx.font = '700 12px system-ui, sans-serif';
    ctx.fillStyle = '#cbd5e1';
    const p1Count = state?.p1_count ?? 22;
    const p2Count = state?.p2_count ?? 22;
    ctx.fillText(`Foncées: ${p1Count}`, 16, 26);

    ctx.textAlign = 'right';
    ctx.fillText(`Claires: ${p2Count}`, width - 16, 26);

    ctx.textAlign = 'center';
    let statusMsg = '';
    if (state?.finished) {
      statusMsg = state.winner === 1 ? 'Victoire Joueur 1 (Foncées) !' : (state.winner === 2 ? 'Victoire Joueur 2 (Claires) !' : 'Partie terminée');
      ctx.fillStyle = '#fbbf24';
    } else if (state?.in_multi) {
      statusMsg = 'Capture successive obligatoire !';
      ctx.fillStyle = '#f59e0b';
    } else if (state?.turn === this.myPlayerNum) {
      statusMsg = 'À votre tour de jouer';
      ctx.fillStyle = '#34d399';
    } else {
      statusMsg = this.isSolo ? 'Le bot réfléchit...' : 'En attente de l\'adversaire...';
      ctx.fillStyle = '#94a3b8';
    }
    ctx.fillText(statusMsg, width / 2, 26);

    // Bottom action bar
    if (state?.last_action) {
      ctx.font = '400 11px system-ui, sans-serif';
      ctx.fillStyle = '#a1a1aa';
      ctx.textAlign = 'center';
      ctx.fillText(state.last_action, width / 2, height - 12);
    }
  }

  public handlePointerDown(x: number, y: number) {
    const state = this.lastState;
    if (!state || state.finished) return;

    const isMyTurn = (state.turn === this.myPlayerNum);
    if (!isMyTurn) return;

    const { width, height } = this;

    // Handle Dual Choice Dialog click
    if (this.pendingChoice) {
      const modalW = 280;
      const modalH = 110;
      const mx = (width - modalW) / 2;
      const my = (height - modalH) / 2;
      const btn1X = mx + 16;
      const btn1Y = my + 44;
      const btnW = 116;
      const btnH = 42;
      const btn2X = mx + modalW - 16 - btnW;
      const btn2Y = my + 44;

      if (x >= btn1X && x <= btn1X + btnW && y >= btn1Y && y <= btn1Y + btnH) {
        // Percussion chosen
        const move = this.pendingChoice;
        this.pendingChoice = null;
        this.selectedNode = -1;
        this.optimisticMove = { from: move.from, to: move.to, captured: move.approachCaptures };
        this.startMoveAnimation(move.from, move.to, this.myPlayerNum, move.approachCaptures);
        this.dispatch({ cmd: 'move', from: move.from, to: move.to, captureType: 'approach' });
        return;
      }

      if (x >= btn2X && x <= btn2X + btnW && y >= btn2Y && y <= btn2Y + btnH) {
        // Aspiration chosen
        const move = this.pendingChoice;
        this.pendingChoice = null;
        this.selectedNode = -1;
        this.optimisticMove = { from: move.from, to: move.to, captured: move.withdrawalCaptures };
        this.startMoveAnimation(move.from, move.to, this.myPlayerNum, move.withdrawalCaptures);
        this.dispatch({ cmd: 'move', from: move.from, to: move.to, captureType: 'withdrawal' });
        return;
      }

      // Clicked outside modal: cancel choice
      this.pendingChoice = null;
      return;
    }

    // Board click detection
    const { startX, startY, step, radius } = this.getBoardLayout();
    let clickedNode = -1;
    let minD = radius * 1.35;

    for (let i = 0; i < 45; i++) {
      const r = Math.floor(i / 9);
      const c = i % 9;
      const nx = startX + c * step;
      const ny = startY + r * step;
      const d = Math.hypot(x - nx, y - ny);
      if (d < minD) {
        minD = d;
        clickedNode = i;
      }
    }

    if (clickedNode === -1) return;

    // In multi-capture: player can only click destinations from active piece
    if (state.in_multi && state.active_piece >= 0) {
      this.selectedNode = state.active_piece;
      if (state.board[clickedNode] === 0 && this.isConnected(this.selectedNode, clickedNode)) {
        this.attemptMove(this.selectedNode, clickedNode, state);
      }
      return;
    }

    // Normal turn:
    if (state.board[clickedNode] === this.myPlayerNum) {
      // Select own stone
      this.selectedNode = clickedNode;
      sound.place();
      haptic(10);
    } else if (this.selectedNode >= 0 && state.board[clickedNode] === 0) {
      // Clicked empty node with selected stone
      if (this.isConnected(this.selectedNode, clickedNode)) {
        this.attemptMove(this.selectedNode, clickedNode, state);
      }
    }
  }

  private attemptMove(from: number, to: number, state: any) {
    const app = this.getApproachCaptures(from, to, state.board);
    const wit = this.getWithdrawalCaptures(from, to, state.board);

    if (app.length > 0 && wit.length > 0) {
      // Dual choice needed!
      this.pendingChoice = { from, to, approachCaptures: app, withdrawalCaptures: wit };
      sound.place();
      haptic(15);
      return;
    }

    const capType = app.length > 0 ? 'approach' : (wit.length > 0 ? 'withdrawal' : '');
    const captured = app.length > 0 ? app : (wit.length > 0 ? wit : []);

    this.selectedNode = -1;
    this.optimisticMove = { from, to, captured };
    this.startMoveAnimation(from, to, this.myPlayerNum, captured);

    this.dispatch({ cmd: 'move', from, to, captureType: capType });
  }

  public handlePointerMove(_x: number, _y: number) {}
  public handlePointerUp(_x: number, _y: number) {}

  public destroy() {
    if (this.botTimer) {
      clearTimeout(this.botTimer);
      this.botTimer = null;
    }
    this.movingStone = null;
    this.capturedStones = [];
    this.sparks = [];
    this.pendingChoice = null;
    this.selectedNode = -1;
    this.lastProcessedMoveId = -1;
    this.optimisticMove = null;
  }
}
