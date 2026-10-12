import { activeRoom, gameState } from '../../state/gameStore';
import { gameSocket } from '../../services/GameSocket';
import styles from './GameOverModal.module.css';

export function GameOverModal() {
  const room = activeRoom.value;
  const state = gameState.value;

  if (!state?.finished || !room) return null;

  const isSolo = room.code === 'SOLO';
  const playerNum = room.playerNum;
  const winner = state.winner ?? (state.finished ? 1 : 0);

  let title = 'GAME OVER';
  let titleClass = styles.titleDraw;
  let iconClass = styles.drawIcon;
  let icon = '🏁';
  let message = 'Match finished.';

  if (winner === 0) {
    title = 'DRAW';
    titleClass = styles.titleDraw;
    iconClass = styles.drawIcon;
    icon = '🤝';
    message = 'Evenly matched duel!';
  } else if (winner === playerNum) {
    title = 'VICTORY';
    titleClass = styles.titleVictory;
    iconClass = styles.victoryIcon;
    icon = '🏆';
    message = 'You emerged triumphant!';
  } else {
    title = 'DEFEAT';
    titleClass = styles.titleDefeat;
    iconClass = styles.defeatIcon;
    icon = '💀';
    message = 'Better luck next match!';
  }

  const handleRematch = () => {
    if (isSolo) {
      gameSocket.startSolo(room.gameType);
    } else {
      gameSocket.sendRematch();
    }
  };

  const rematchLabel = isSolo
    ? 'Play Again'
    : room.rematchCount
    ? `Rematch (${room.rematchCount}/${room.rematchNeeded || 2})`
    : 'Request Rematch';

  return (
    <div className={styles.modalOverlay}>
      <div className={`${styles.iconWrap} ${iconClass}`}>{icon}</div>
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
