import { IGameRenderer, ActionDispatcher } from './IGameRenderer';
import { sound, haptic } from '../audio/SoundEngine';

export class DominoRenderer implements IGameRenderer {
  private width = 400;
  private height = 400;
  public selectedTileIdx = -1;
  public choiceNeeded = false;
  private lastState: any = null;
  private myPlayerNum = 1;
  private isSolo = false;
  private dispatch: ActionDispatcher = () => {};

  public init(_canvas?: HTMLCanvasElement, _ctx?: CanvasRenderingContext2D, dispatch?: ActionDispatcher, playerNum?: number) {
    if (playerNum) this.myPlayerNum = playerNum;
    if (dispatch) this.dispatch = dispatch;
    this.selectedTileIdx = -1;
    this.choiceNeeded = false;
    this.lastState = null;
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

  public render(state: any, ctx: CanvasRenderingContext2D) {
    this.lastState = state;
    const myPlayerNum = this.myPlayerNum;

    // Havana Emerald Felt Gaming Table Radial Gradient (Issue 07)
    const grad = ctx.createRadialGradient(200, 200, 20, 200, 200, 260);
    grad.addColorStop(0, '#0f382a');
    grad.addColorStop(1, '#051811');
    ctx.fillStyle = grad;
    ctx.fillRect(0, 0, this.width, this.height);

    ctx.strokeStyle = '#143c2e';
    ctx.lineWidth = 2;
    ctx.strokeRect(6, 6, this.width - 12, this.height - 12);

    // Opponent Hand Counts (Fog of War)
    ctx.fillStyle = '#94a3b8';
    ctx.font = '600 11px system-ui, sans-serif';
    ctx.textAlign = 'left';

    let oppStr = '';
    if (this.isSolo) {
      oppStr = `Bot: ${state?.p2_count || 0} tiles`;
    } else if (state?.num_players === 3) {
      oppStr = `P1: ${state?.p1_count} · P2: ${state?.p2_count} · P3: ${state?.p3_count}`;
    } else {
      oppStr = myPlayerNum === 1
        ? `Opponent: ${state?.p2_count || 0} tiles`
        : `Opponent: ${state?.p1_count || 0} tiles`;
    }
    ctx.fillText(oppStr, 14, 24);

    // Boneyard / Reserve Badge
    ctx.textAlign = 'right';
    ctx.fillStyle = '#38bdf8';
    const boneyardLabel = `Reserve: ${state?.boneyard_count || 0}`;
    ctx.fillText(boneyardLabel, 386, 24);

    // Open End Indicators
    ctx.font = '700 11px system-ui, sans-serif';
    ctx.textAlign = 'left';
    if (state?.left_end !== undefined && state.left_end !== -1) {
      ctx.fillStyle = '#38bdf8';
      ctx.fillText(`◄ Left: ${state.left_end}`, 14, 42);
    }
    ctx.textAlign = 'right';
    if (state?.right_end !== undefined && state.right_end !== -1) {
      ctx.fillStyle = '#fbbf24';
      ctx.fillText(`Right: ${state.right_end} ►`, 386, 42);
    }

    // Board Chain
    const board = state?.board || [];
    if (board.length === 0) {
      ctx.textAlign = 'center';
      ctx.fillStyle = '#64748b';
      ctx.font = '500 13px system-ui, sans-serif';
      ctx.fillText('Board is open. Tap any tile in hand to lead!', 200, 160);
    } else {
      const maxPerRow = 7;
      const bTileW = 38;
      const bTileH = 22;

      board.forEach((t: number[], i: number) => {
        const row = Math.floor(i / maxPerRow);
        const colInRow = i % maxPerRow;
        const isEvenRow = row % 2 === 0;
        const col = isEvenRow ? colInRow : maxPerRow - 1 - colInRow;

        const bx = 22 + col * (bTileW + 8);
        const by = 56 + row * (bTileH + 16);

        // 3D Ivory Tile Body
        ctx.save();
        ctx.shadowColor = 'rgba(0, 0, 0, 0.45)';
        ctx.shadowBlur = 5;
        ctx.shadowOffsetY = 2;
        ctx.fillStyle = '#fffbeb';
        ctx.beginPath();
        ctx.rect(bx, by, bTileW, bTileH);
        ctx.fill();
        ctx.restore();

        ctx.strokeStyle = '#334155';
        ctx.lineWidth = 1.2;
        ctx.strokeRect(bx, by, bTileW, bTileH);

        // Center divider
        ctx.strokeStyle = '#475569';
        ctx.beginPath();
        ctx.moveTo(bx + bTileW / 2, by + 2);
        ctx.lineTo(bx + bTileW / 2, by + bTileH - 2);
        ctx.stroke();

        // Pips
        const pipsLeft = isEvenRow ? t[0] : t[1];
        const pipsRight = isEvenRow ? t[1] : t[0];
        this.drawPips(ctx, pipsLeft, bx, by, bTileW / 2, bTileH);
        this.drawPips(ctx, pipsRight, bx + bTileW / 2, by, bTileW / 2, bTileH);

        // Pulsing open end halo
        const haloPulse = 0.5 + 0.5 * Math.sin(Date.now() / 200);
        if (i === 0) {
          ctx.strokeStyle = `rgba(56, 189, 248, ${0.6 + haloPulse * 0.4})`;
          ctx.lineWidth = 2.5;
          ctx.strokeRect(bx - 2, by - 2, bTileW + 4, bTileH + 4);
        } else if (i === board.length - 1) {
          ctx.strokeStyle = `rgba(251, 191, 36, ${0.6 + haloPulse * 0.4})`;
          ctx.lineWidth = 2.5;
          ctx.strokeRect(bx - 2, by - 2, bTileW + 4, bTileH + 4);
        }
      });
    }

    // Divider line
    ctx.strokeStyle = '#143c2e';
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.moveTo(10, 290);
    ctx.lineTo(390, 290);
    ctx.stroke();

    // Player Hand Tiles
    const myHand = (state?.hands && state.hands[myPlayerNum]) || [];
    const count = myHand.length;

    ctx.textAlign = 'left';
    ctx.fillStyle = '#94a3b8';
    ctx.font = '600 11px system-ui, sans-serif';
    ctx.fillText(`Your Hand (${count} tile${count !== 1 ? 's' : ''}):`, 14, 306);

    if (count > 0) {
      const tileW = Math.min(42, Math.max(20, Math.floor(376 / count)));
      const tileH = 68;
      const totalW = count * tileW;
      const startX = Math.max(12, Math.floor((400 - totalW) / 2));
      const isMyTurn = state?.turn === myPlayerNum && !state?.finished;

      myHand.forEach((t: number[], i: number) => {
        const tx = startX + i * tileW;
        let ty = 314;
        const isSelected = this.selectedTileIdx === i;
        if (isSelected) ty -= 8;

        const canPlay =
          state?.left_end === undefined ||
          state?.left_end === -1 ||
          t[0] === state?.left_end ||
          t[1] === state?.left_end ||
          t[0] === state?.right_end ||
          t[1] === state?.right_end;

        // 3D Ivory Body with Drop Shadow
        ctx.save();
        ctx.shadowColor = 'rgba(0, 0, 0, 0.45)';
        ctx.shadowBlur = 6;
        ctx.shadowOffsetY = 3;
        ctx.fillStyle = isSelected ? '#ffffff' : '#fffbeb';
        ctx.beginPath();
        ctx.rect(tx + 2, ty, tileW - 4, tileH);
        ctx.fill();
        ctx.restore();

        // Border highlight
        if (isSelected) {
          ctx.strokeStyle = '#fbbf24';
          ctx.lineWidth = 2.5;
        } else if (isMyTurn && canPlay) {
          ctx.strokeStyle = '#38bdf8';
          ctx.lineWidth = 2;
        } else {
          ctx.strokeStyle = '#334155';
          ctx.lineWidth = 1.2;
        }
        ctx.strokeRect(tx + 2, ty, tileW - 4, tileH);

        // Divider
        ctx.strokeStyle = '#475569';
        ctx.beginPath();
        ctx.moveTo(tx + 4, ty + tileH / 2);
        ctx.lineTo(tx + tileW - 4, ty + tileH / 2);
        ctx.stroke();

        this.drawPips(ctx, t[0], tx + 2, ty, tileW - 4, tileH / 2);
        this.drawPips(ctx, t[1], tx + 2, ty + tileH / 2, tileW - 4, tileH / 2);
      });
    }
  }

  private drawPips(c: CanvasRenderingContext2D, val: number, x: number, y: number, w: number, h: number) {
    if (val <= 0) return;
    const r = Math.max(1.6, Math.min(2.8, w * 0.08));
    c.fillStyle = '#0f172a';

    const c1 = x + w * 0.26;
    const c2 = x + w * 0.5;
    const c3 = x + w * 0.74;

    const r1 = y + h * 0.26;
    const r2 = y + h * 0.5;
    const r3 = y + h * 0.74;

    const dot = (px: number, py: number) => {
      c.beginPath();
      c.arc(px, py, r, 0, Math.PI * 2);
      c.fill();
    };

    if (val === 1) {
      dot(c2, r2);
    } else if (val === 2) {
      dot(c1, r1);
      dot(c3, r3);
    } else if (val === 3) {
      dot(c1, r1);
      dot(c2, r2);
      dot(c3, r3);
    } else if (val === 4) {
      dot(c1, r1);
      dot(c3, r1);
      dot(c1, r3);
      dot(c3, r3);
    } else if (val === 5) {
      dot(c1, r1);
      dot(c3, r1);
      dot(c2, r2);
      dot(c1, r3);
      dot(c3, r3);
    } else if (val === 6) {
      dot(c1, r1);
      dot(c3, r1);
      dot(c1, r2);
      dot(c3, r2);
      dot(c1, r3);
      dot(c3, r3);
    }
  }

  public handlePointerDown(x: number, y: number) {
    if (y < 300) return; // Only process hand clicks
    const state = this.lastState;
    const myPlayerNum = this.myPlayerNum;
    const myHand = (state?.hands && state.hands[myPlayerNum]) || [];
    const count = myHand.length;
    if (count === 0) return;

    const tileW = Math.min(42, Math.max(20, Math.floor(376 / count)));
    const totalW = count * tileW;
    const startX = Math.max(12, Math.floor((400 - totalW) / 2));
    const idx = Math.floor((x - startX) / tileW);

    if (idx >= 0 && idx < count) {
      const tile = myHand[idx];
      const lEnd = state?.left_end;
      const rEnd = state?.right_end;

      sound.click();
      haptic(15);

      if (lEnd === undefined || lEnd === -1) {
        // First tile on board
        this.dispatch({ cmd: 'play', index: idx, end: 'left' });
        this.selectedTileIdx = -1;
      } else {
        const mLeft = tile[0] === lEnd || tile[1] === lEnd;
        const mRight = tile[0] === rEnd || tile[1] === rEnd;

        if (mLeft && mRight && lEnd !== rEnd) {
          // Can play on both ends
          this.selectedTileIdx = idx;
          this.choiceNeeded = true;
        } else if (mLeft) {
          this.dispatch({ cmd: 'play', index: idx, end: 'left' });
          this.selectedTileIdx = -1;
        } else if (mRight) {
          this.dispatch({ cmd: 'play', index: idx, end: 'right' });
          this.selectedTileIdx = -1;
        }
      }
    }
  }

  public handlePointerMove(_x: number, _y: number) {}
  public handlePointerUp(_x: number, _y: number) {}
  public destroy() {
    this.selectedTileIdx = -1;
    this.choiceNeeded = false;
  }
}
