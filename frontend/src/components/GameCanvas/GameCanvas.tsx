import { useEffect, useRef } from 'preact/hooks';
import { activeRoom, gameState, gameCatalogue } from '../../state/gameStore';
import { gameSocket } from '../../services/GameSocket';
import { IGameRenderer } from '../../renderers/IGameRenderer';
import { ParticleEngine } from '../../renderers/ParticleEngine';
import { CameraJuice } from '../../renderers/CameraJuice';
import { getGameRenderer } from '../../registry/gameRegistry';
import { GameOverModal } from '../GameOver/GameOverModal';
import { VirtualDPad } from '../VirtualDPad/VirtualDPad';
import { ActionToolbar } from '../ActionToolbar/ActionToolbar';
import { ActionModal } from '../ActionModal/ActionModal';
import styles from './GameCanvas.module.css';

export function GameCanvas() {
  const canvasRef = useRef<HTMLCanvasElement | null>(null);
  const rendererRef = useRef<IGameRenderer | null>(null);
  const particlesRef = useRef<ParticleEngine>(new ParticleEngine());
  const juiceRef = useRef<CameraJuice>(new CameraJuice());
  const animFrameRef = useRef<number | null>(null);

  const room = activeRoom.value;
  const gameType = room?.gameType || 'ttt_solo';

  const isFinished = gameState.value?.finished;
  const winner = gameState.value?.winner;
  const myPlayerNum = room?.playerNum || 1;
  const wonRef = useRef(false);

  // Victory particle burst
  useEffect(() => {
    if (isFinished && winner === myPlayerNum && !wonRef.current) {
      wonRef.current = true;
      particlesRef.current.confetti(200, 160, 65);
    } else if (!isFinished) {
      wonRef.current = false;
    }
  }, [isFinished, winner, myPlayerNum]);

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
    const renderer: IGameRenderer = getGameRenderer(gameType, juice);
    if ('particles' in renderer) {
      (renderer as any).particles = particles;
    }

    const isSolo = room?.code === 'SOLO';
    renderer.init(canvas, ctx, (action) => gameSocket.send(action), myPlayerNum);
    if ('setIsSolo' in renderer && typeof (renderer as any).setIsSolo === 'function') {
      (renderer as any).setIsSolo(isSolo);
    }
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

  const meta = gameCatalogue.value.find(g => g.key === gameType);
  const inputs = (gameState.value as any)?.inputs || meta?.inputs || [];
  const hasDpad = inputs.includes('dpad');

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
        <ActionModal />
        <GameOverModal />
      </div>

      <ActionToolbar />

      {hasDpad && <VirtualDPad />}
    </div>
  );
}
