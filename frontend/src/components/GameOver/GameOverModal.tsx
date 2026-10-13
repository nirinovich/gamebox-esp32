import { useEffect, useRef } from 'preact/hooks';
import { activeRoom, gameState } from '../../state/gameStore';
import { recordMatchOutcome } from '../../state/hallOfFameStore';
import { gameSocket } from '../../services/GameSocket';
import { sound } from '../../audio/SoundEngine';
import styles from './GameOverModal.module.css';

export function GameOverModal() {
  const room = activeRoom.value;
  const state = gameState.value;
  const recordedRef = useRef<string | null>(null);

  const isFinished = !!state?.finished && !!room;
  const isSolo = room?.code === 'SOLO';
  const playerNum = room?.playerNum || 1;
  const winner = state?.winner ?? (state?.finished ? 1 : 0);

  useEffect(() => {
    if (isFinished && room) {
      const matchKey = `${room.code}_${room.gameType}_${winner}_${state?.score1 ?? 0}_${state?.score2 ?? 0}`;
      if (recordedRef.current !== matchKey) {
        recordedRef.current = matchKey;
        let outcome: 'WIN' | 'LOSS' | 'DRAW' = 'DRAW';
        if (winner === 0) {
          outcome = 'DRAW';
        } else if (winner === playerNum) {
          outcome = 'WIN';
          sound.victory();
        } else {
          outcome = 'LOSS';
          sound.defeat();
        }
        recordMatchOutcome(room.gameType, outcome);
      }
    } else if (!state?.finished) {
      recordedRef.current = null;
    }
  }, [isFinished, winner, room?.code, room?.gameType]);

  if (!state?.finished || !room) return null;

  let title = 'GAME OVER';
  let titleClass = styles.titleDraw;
  let iconClass = styles.drawIcon;
  let message = 'Match finished.';

  // Vector SVG elements adhering to ADR 0010
  let iconElement = (
    <svg width="32" height="32" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
      <path d="M4 15s1-1 4-1 5 2 8 2 4-1 4-1V3s-1 1-4 1-5-2-8-2-4 1-4 1z" />
      <line x1="4" y1="22" x2="4" y2="15" />
    </svg>
  );

  if (winner === 0) {
    title = 'DRAW';
    titleClass = styles.titleDraw;
    iconClass = styles.drawIcon;
    message = 'Evenly matched duel!';
    iconElement = (
      <svg width="32" height="32" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
        <path d="m11 17 2 2a1 1 0 0 0 1.4 0l4.3-4.3a1 1 0 0 0 0-1.4l-2.6-2.6a1 1 0 0 0-1.4 0L13 12.4" />
        <path d="m13 7.6 1.7-1.7a1 1 0 0 1 1.4 0l2.6 2.6a1 1 0 0 1 0 1.4L17 11.6" />
        <path d="m3 14 3.7-3.7a1 1 0 0 1 1.4 0L10 12.2" />
        <path d="m7 18 2.2 2.2a1 1 0 0 0 1.4 0l1.4-1.4" />
        <path d="m2 17 3-3" />
        <path d="m22 7-3 3" />
      </svg>
    );
  } else if (winner === playerNum) {
    title = 'VICTORY';
    titleClass = styles.titleVictory;
    iconClass = styles.victoryIcon;
    message = 'You emerged triumphant!';
    iconElement = (
      <svg width="32" height="32" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
        <path d="M6 9H4.5a2.5 2.5 0 0 1 0-5H6" />
        <path d="M18 9h1.5a2.5 2.5 0 0 0 0-5H18" />
        <path d="M4 22h16" />
        <path d="M10 14.66V17c0 .55-.45 1-1 1H7" />
        <path d="M14 14.66V17c0 .55.45 1 1 1h2" />
        <path d="M18 2H6v7a6 6 0 0 0 12 0V2Z" />
      </svg>
    );
  } else {
    title = 'DEFEAT';
    titleClass = styles.titleDefeat;
    iconClass = styles.defeatIcon;
    message = 'Better luck next match!';
    iconElement = (
      <svg width="32" height="32" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
        <circle cx="9" cy="12" r="1.5" fill="currentColor" />
        <circle cx="15" cy="12" r="1.5" fill="currentColor" />
        <path d="M8 20v2h8v-2" />
        <path d="M16 20a3 3 0 0 0 3-3V9a7 7 0 1 0-14 0v8a3 3 0 0 0 3 3" />
      </svg>
    );
  }

  const handleRematch = () => {
    if (isSolo) {
      gameSocket.startSolo(room.gameType);
    } else {
      gameSocket.sendRematch();
    }
  };

  const rematchLabel = isSolo
    ? 'Rematch'
    : room.rematchCount
    ? `Rematch (${room.rematchCount}/${room.rematchNeeded || 2})`
    : 'Request Rematch';

  return (
    <div className={styles.modalOverlay}>
      <div className={`${styles.iconWrap} ${iconClass}`}>{iconElement}</div>
      <div className={`${styles.title} ${titleClass}`}>{title}</div>
      <div className={styles.sub}>{message}</div>

      <div className={styles.actions}>
        <button className={styles.btnAction} onClick={handleRematch}>
          {rematchLabel}
        </button>
        <button className={styles.btnSecondary} onClick={() => gameSocket.leaveRoom()}>
          Return to Lobby
        </button>
      </div>
    </div>
  );
}
