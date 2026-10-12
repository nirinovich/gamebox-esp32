import { useEffect } from 'preact/hooks';
import { gameSocket } from '../../services/GameSocket';
import { activeRoom } from '../../state/gameStore';
import { sound, haptic } from '../../audio/SoundEngine';
import styles from './VirtualDPad.module.css';

export function VirtualDPad() {
  const room = activeRoom.value;
  const gameType = room?.gameType || '';

  const sendDirection = (dir: string) => {
    haptic(15);
    sound.move();
    gameSocket.send({ cmd: 'dir', val: dir });
  };

  const stopMoving = () => {
    if (gameType.includes('pong')) {
      gameSocket.send({ cmd: 'dir', val: 'STOP' });
    }
  };

  useEffect(() => {
    const handleKeyDown = (e: KeyboardEvent) => {
      let dir: string | null = null;
      if (e.key === 'ArrowUp' || e.key === 'w' || e.key === 'W') dir = 'UP';
      else if (e.key === 'ArrowDown' || e.key === 's' || e.key === 'S') dir = 'DOWN';
      else if (e.key === 'ArrowLeft' || e.key === 'a' || e.key === 'A') dir = 'LEFT';
      else if (e.key === 'ArrowRight' || e.key === 'd' || e.key === 'D') dir = 'RIGHT';

      if (dir) {
        e.preventDefault();
        sendDirection(dir);
      }
    };

    const handleKeyUp = (e: KeyboardEvent) => {
      if (gameType.includes('pong')) {
        if (['ArrowUp', 'ArrowDown', 'w', 'W', 's', 'S'].includes(e.key)) {
          stopMoving();
        }
      }
    };

    window.addEventListener('keydown', handleKeyDown);
    window.addEventListener('keyup', handleKeyUp);

    return () => {
      window.removeEventListener('keydown', handleKeyDown);
      window.removeEventListener('keyup', handleKeyUp);
    };
  }, [gameType]);

  return (
    <div className={styles.dpadContainer}>
      <button
        className={`${styles.dpadBtn} ${styles.dpadUp}`}
        onPointerDown={(e) => {
          e.preventDefault();
          sendDirection('UP');
        }}
        onPointerUp={stopMoving}
        onPointerLeave={stopMoving}
      >
        <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2.5">
          <polyline points="18 15 12 9 6 15" />
        </svg>
      </button>

      <button
        className={`${styles.dpadBtn} ${styles.dpadLeft}`}
        onPointerDown={(e) => {
          e.preventDefault();
          sendDirection('LEFT');
        }}
        onPointerUp={stopMoving}
        onPointerLeave={stopMoving}
      >
        <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2.5">
          <polyline points="15 18 9 12 15 6" />
        </svg>
      </button>

      <button
        className={`${styles.dpadBtn} ${styles.dpadRight}`}
        onPointerDown={(e) => {
          e.preventDefault();
          sendDirection('RIGHT');
        }}
        onPointerUp={stopMoving}
        onPointerLeave={stopMoving}
      >
        <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2.5">
          <polyline points="9 18 15 12 9 6" />
        </svg>
      </button>

      <button
        className={`${styles.dpadBtn} ${styles.dpadDown}`}
        onPointerDown={(e) => {
          e.preventDefault();
          sendDirection('DOWN');
        }}
        onPointerUp={stopMoving}
        onPointerLeave={stopMoving}
      >
        <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2.5">
          <polyline points="6 9 12 15 18 9" />
        </svg>
      </button>
    </div>
  );
}
