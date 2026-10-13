import { useState } from 'preact/hooks';
import { activeRoom } from '../../state/gameStore';
import { gameSocket } from '../../services/GameSocket';
import styles from './WaitingRoom.module.css';

export function WaitingRoom() {
  const room = activeRoom.value;
  const [copied, setCopied] = useState(false);

  if (!room) return null;

  const handleCopy = () => {
    if (navigator.clipboard) {
      navigator.clipboard.writeText(room.code);
      setCopied(true);
      setTimeout(() => setCopied(false), 2000);
    }
  };

  const count = room.waitingCount || 1;
  const max = room.waitingMax || 2;
  const canStartEarly = room.isHost && count >= 2;

  return (
    <div className={styles.waitingBox}>
      <div className={styles.radarPulse}>
        <svg width="32" height="32" viewBox="0 0 24 24" fill="none" stroke="var(--p1-cyan)" strokeWidth="2.2">
          <path d="M12 2a10 10 0 1 0 10 10A10 10 0 0 0 12 2zm0 18a8 8 0 1 1 8-8 8 8 0 0 1-8 8z" />
          <path d="M12 6a6 6 0 1 0 6 6 6 6 0 0 0-6-6zm0 10a4 4 0 1 1 4-4 4 4 0 0 1-4 4z" />
        </svg>
      </div>

      <div className={styles.titleArea}>
        <span className={styles.gameTitle}>Waiting Room</span>
        <span className={styles.hostMsg}>
          {count} / {max} players joined. Visible in public match feed.
        </span>
      </div>

      <div className={styles.codeWrap} onClick={handleCopy} title="Click to copy Room Code">
        <span className={styles.roomCodeTag}>{room.code}</span>
        <span className={styles.copyLabel}>{copied ? 'COPIED!' : 'CLICK TO COPY'}</span>
      </div>

      {canStartEarly && (
        <button
          className={styles.btnSolid}
          onClick={() => gameSocket.send({ cmd: 'start_early' })}
        >
          Start Match Now ({count} Players)
        </button>
      )}

      <button className={styles.btnCancel} onClick={() => gameSocket.leaveRoom()}>
        Cancel & Leave Room
      </button>
    </div>
  );
}
