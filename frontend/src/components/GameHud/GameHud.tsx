import { activeRoom, gameState, playerName } from '../../state/gameStore';
import styles from './GameHud.module.css';

export function GameHud() {
  const room = activeRoom.value;
  const state = gameState.value;

  if (!room) return null;

  const isSolo = room.code === 'SOLO';
  const p1Name = room.isHost ? playerName.value : room.opponentName;
  const p2Name = room.isHost ? room.opponentName : playerName.value;

  // Standardized scores array streamed from authoritative Host state
  const scores = (Array.isArray(state?.scores) ? state.scores : []) as number[];
  const p1Score = scores[0] ?? state?.p1_score ?? state?.score1 ?? state?.score ?? 0;
  const p2Score = scores[1] ?? state?.p2_score ?? state?.score2 ?? state?.highScore ?? 0;
  const p3Score = scores[2] ?? state?.p3_score ?? 0;
  const hasP3 = scores.length >= 3 || state?.p3_active === true || state?.num_players === 3;

  const seriesP1 = state?.series_p1 ?? room.seriesScore?.p1 ?? 0;
  const seriesP2 = state?.series_p2 ?? room.seriesScore?.p2 ?? 0;
  const hasSeries = !isSolo && (seriesP1 > 0 || seriesP2 > 0);

  return (
    <>
      <div className={styles.gameHud}>
        {/* P1 Handle & Score */}
        <div className={`${styles.hudPlayer} ${styles.hudP1}`}>
          <span className={styles.dotCyan} />
          <span>{p1Name}</span>
          <span className={styles.hudScore}>{p1Score}</span>
        </div>

        {/* Status / Series */}
        <div className={styles.hudStatus}>
          <span className={styles.statusText}>
            {state?.finished ? 'Match Over' : isSolo ? 'Solo Run' : 'Duel Active'}
          </span>
          {hasSeries && (
            <span className={styles.seriesScore}>
              Series: {seriesP1} - {seriesP2}
            </span>
          )}
        </div>

        {/* P2 Handle & Score */}
        <div className={`${styles.hudPlayer} ${styles.hudP2}`}>
          <span className={styles.hudScore}>{p2Score}</span>
          <span>{p2Name}</span>
          <span className={isSolo ? styles.botDot : styles.dotPink} />
        </div>
      </div>

      {/* 3P Player Bar */}
      {hasP3 && (
        <div className={styles.hudP3Wrap}>
          <span className={styles.p3Dot} />
          <span>P3 (Challenger): {p3Score}</span>
        </div>
      )}
    </>
  );
}
