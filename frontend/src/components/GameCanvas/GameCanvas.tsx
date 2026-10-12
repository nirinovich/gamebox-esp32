import { useEffect, useRef, useState } from 'preact/hooks';
import { activeRoom, gameState } from '../../state/gameStore';
import { gameSocket } from '../../services/GameSocket';
import { IGameRenderer } from '../../renderers/IGameRenderer';
import { ParticleEngine } from '../../renderers/ParticleEngine';
import { CameraJuice } from '../../renderers/CameraJuice';
import { TicTacToeRenderer } from '../../renderers/TicTacToeRenderer';
import { ConnectFourRenderer } from '../../renderers/ConnectFourRenderer';
import { BattleshipRenderer } from '../../renderers/BattleshipRenderer';
import { SnakeRenderer } from '../../renderers/SnakeRenderer';
import { PongRenderer } from '../../renderers/PongRenderer';
import { TronRenderer } from '../../renderers/TronRenderer';
import { DominoRenderer } from '../../renderers/DominoRenderer';
import { GameOverModal } from '../GameOver/GameOverModal';
import { VirtualDPad } from '../VirtualDPad/VirtualDPad';
import styles from './GameCanvas.module.css';

export function GameCanvas() {
  const canvasRef = useRef<HTMLCanvasElement | null>(null);
  const rendererRef = useRef<IGameRenderer | null>(null);
  const particlesRef = useRef<ParticleEngine>(new ParticleEngine());
  const juiceRef = useRef<CameraJuice>(new CameraJuice());
  const animFrameRef = useRef<number | null>(null);
  const [dominoChoice, setDominoChoice] = useState<number | null>(null);

  const room = activeRoom.value;
  const gameType = room?.gameType || 'ttt_solo';

  // Keep global reference for Domino renderer pointer events and confetti
  useEffect(() => {
    (window as any).__lastGameState = gameState.value;
  }, [gameState.value]);

  useEffect(() => {
    (window as any).__spawnConfetti = () => {
      particlesRef.current.confetti(200, 160, 65);
    };
    return () => {
      delete (window as any).__spawnConfetti;
    };
  }, []);

  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;

    const dpr = Math.min(window.devicePixelRatio || 1, 2);
    canvas.width = 400 * dpr;
    canvas.height = 400 * dpr;
    const ctx = canvas.getContext('2d');
    if (!ctx) return;

    const particles = particlesRef.current;
    const juice = juiceRef.current;
    let renderer: IGameRenderer;

    if (gameType.includes('snake')) {
      renderer = new SnakeRenderer();
    } else if (gameType.includes('pong')) {
      renderer = new PongRenderer();
    } else if (gameType.includes('tron')) {
      renderer = new TronRenderer();
    } else if (gameType.includes('domino')) {
      renderer = new DominoRenderer();
    } else if (gameType.includes('connect4')) {
      renderer = new ConnectFourRenderer(juice);
    } else if (gameType.includes('battleship')) {
      renderer = new BattleshipRenderer(particles, juice);
    } else {
      renderer = new TicTacToeRenderer();
    }

    renderer.init(canvas, ctx, (action) => gameSocket.send(action));
    renderer.resize(400, 400);
    rendererRef.current = renderer;

    const loop = () => {
      juice.update();

      ctx.save();
      ctx.scale(dpr, dpr);
      juice.apply(ctx);

      if (rendererRef.current) {
        rendererRef.current.render(gameState.value, ctx);
      }

      particles.updateAndRender(ctx);
      ctx.restore();

      // Check if domino choice is pending
      if (renderer instanceof DominoRenderer && renderer.choiceNeeded) {
        setDominoChoice(renderer.selectedTileIdx);
      }

      animFrameRef.current = requestAnimationFrame(loop);
    };

    animFrameRef.current = requestAnimationFrame(loop);

    return () => {
      if (animFrameRef.current) {
        cancelAnimationFrame(animFrameRef.current);
      }
      renderer.destroy();
      rendererRef.current = null;
    };
  }, [gameType]);

  const getLogicalCoords = (e: MouseEvent | TouchEvent) => {
    const canvas = canvasRef.current;
    if (!canvas) return { x: 0, y: 0 };
    const rect = canvas.getBoundingClientRect();
    const clientX = 'touches' in e && e.touches.length > 0 ? e.touches[0].clientX : (e as MouseEvent).clientX;
    const clientY = 'touches' in e && e.touches.length > 0 ? e.touches[0].clientY : (e as MouseEvent).clientY;
    const scaleX = 400 / rect.width;
    const scaleY = 400 / rect.height;
    return {
      x: (clientX - rect.left) * scaleX,
      y: (clientY - rect.top) * scaleY
    };
  };

  const handlePointerDown = (e: any) => {
    const { x, y } = getLogicalCoords(e);
    rendererRef.current?.handlePointerDown(x, y);
  };

  const handlePointerMove = (e: any) => {
    const { x, y } = getLogicalCoords(e);
    rendererRef.current?.handlePointerMove(x, y);
  };

  const handlePointerUp = (e: any) => {
    const { x, y } = getLogicalCoords(e);
    rendererRef.current?.handlePointerUp(x, y);
  };

  const handleDominoEndChoice = (end: 'left' | 'right') => {
    if (dominoChoice !== null) {
      gameSocket.send({ cmd: 'play', index: dominoChoice, end });
      if (rendererRef.current instanceof DominoRenderer) {
        rendererRef.current.selectedTileIdx = -1;
        rendererRef.current.choiceNeeded = false;
      }
      setDominoChoice(null);
    }
  };

  const hasDpad =
    gameType.includes('snake') || gameType.includes('pong') || gameType.includes('tron');
  const isDomino = gameType.includes('domino');
  const isBattleshipPlacement =
    gameType.includes('battleship') && gameState.value?.phase === 'placement';

  const myPlayerNum = room?.playerNum || 1;
  const isMyTurn = gameState.value?.turn === myPlayerNum && !gameState.value?.finished;
  const myHand = (gameState.value?.hands && gameState.value.hands[myPlayerNum]) || [];
  const leftEnd = gameState.value?.left_end ?? -1;
  const rightEnd = gameState.value?.right_end ?? -1;
  const hasLegalMove = myHand.some((t: number[]) =>
    leftEnd === -1 || t[0] === leftEnd || t[1] === leftEnd || t[0] === rightEnd || t[1] === rightEnd
  );
  const boneyardCount = gameState.value?.boneyard_count ?? 14;
  const shouldHighlightDraw = isMyTurn && !hasLegalMove && boneyardCount > 0;

  return (
    <div style={{ width: '100%' }}>
      <div className={styles.canvasWrap}>
        <canvas
          ref={canvasRef}
          className={styles.canvas}
          onMouseDown={handlePointerDown}
          onMouseMove={handlePointerMove}
          onMouseUp={handlePointerUp}
          onTouchStart={handlePointerDown}
          onTouchMove={handlePointerMove}
          onTouchEnd={handlePointerUp}
        />
        <GameOverModal />
      </div>

      {/* Battleship Auto-Place */}
      {isBattleshipPlacement && (
        <div className={styles.extraControls}>
          <button
            className={styles.btnSolid}
            onClick={() => gameSocket.send({ cmd: 'auto_place' })}
          >
            Deploy Fleet (Auto-Arrange)
          </button>
        </div>
      )}

      {/* Domino Controls */}
      {isDomino && (
        <div className={styles.extraControls} style={{ marginTop: '8px' }}>
          <button
            className={`${styles.btnSolid} ${shouldHighlightDraw ? styles.btnHighlightPulse : ''}`}
            style={{ flex: 1 }}
            onClick={() => gameSocket.send({ cmd: 'draw' })}
          >
            Draw Tile ({boneyardCount})
          </button>
          <button
            className={styles.btnSolid}
            style={{ flex: 1, background: 'var(--surface)', color: 'var(--text)', border: '1px solid var(--border)' }}
            onClick={() => gameSocket.send({ cmd: 'pass' })}
          >
            Pass Turn
          </button>
        </div>
      )}

      {/* Domino End Choice Modal */}
      {dominoChoice !== null && (
        <div className={styles.extraControls} style={{ marginTop: '6px' }}>
          <button
            className={styles.btnSolid}
            style={{ flex: 1, background: '#0284c7' }}
            onClick={() => handleDominoEndChoice('left')}
          >
            Place Left End
          </button>
          <button
            className={styles.btnSolid}
            style={{ flex: 1, background: '#0284c7' }}
            onClick={() => handleDominoEndChoice('right')}
          >
            Place Right End
          </button>
        </div>
      )}

      {/* Virtual Directional Pad */}
      {hasDpad && <VirtualDPad />}
    </div>
  );
}
