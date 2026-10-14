import { signal } from '@preact/signals';
import { activeRoom, gameState, playerName } from '../../state/gameStore';
import styles from './GameHud.module.css';

export const roomCodeCopied = signal<boolean>(false);

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

  // Dynamic Turn Guidance
  let statusText = isSolo ? 'SOLO RUN' : 'DUEL ACTIVE';
  let isMyTurn = false;

  if (state?.finished) {
    statusText = 'MATCH OVER';
  } else if (state?.turn !== undefined) {
    const activeTurn = Number(state.turn);
    const myPlayerNum = room.playerNum ?? (room.isHost ? 1 : 2);
    isMyTurn = activeTurn === myPlayerNum;
    if (isSolo) {
      statusText = isMyTurn ? 'YOUR TURN' : 'AI THINKING...';
    } else {
      statusText = isMyTurn ? 'YOUR TURN' : "OPPONENT'S TURN";
    }
  }

  const handleCopyCode = async () => {
    if (!room.code || room.code === 'SOLO') return;
    try {
      if (typeof navigator !== 'undefined' && navigator.clipboard?.writeText) {
        await navigator.clipboard.writeText(room.code);
        roomCodeCopied.value = true;
        setTimeout(() => {
          roomCodeCopied.value = false;
        }, 1500);
      }
    } catch {
      // Do not set copied state if clipboard write rejected
    }
  };

  const activeTurnNum = state?.turn !== undefined ? Number(state.turn) : null;

  return (
    <>
      <div className={styles.gameHud}>
        {/* P1 Handle & Score */}
        <div className={`${styles.hudPlayer} ${styles.hudP1}`}>
          <span
            className={`${styles.turnPip} ${activeTurnNum === 1 && !state?.finished ? styles.turnPipActive : ''}`}
            aria-hidden="true"
          />
          <span className={styles.playerName}>{p1Name}</span>
          <span className={styles.hudScore}>{p1Score}</span>
        </div>

        {/* Status / Turn Guidance / Room Code */}
        <div className={styles.hudStatus}>
          <div className={styles.turnGuidance}>
            {isMyTurn && !state?.finished && (
              <span className={styles.radarDot} aria-hidden="true" />
            )}
            <span className={`${styles.statusText} ${isMyTurn && !state?.finished ? styles.statusActive : ''}`}>
              {statusText}
            </span>
          </div>

          {!isSolo && room.code && (
            <button
              className={styles.roomCodeBadge}
              onClick={handleCopyCode}
              type="button"
              title="Click to copy Room Code"
              aria-label={`Room Code: ${room.code}`}
            >
              <span className={styles.codeText}>{roomCodeCopied.value ? 'COPIED' : room.code}</span>
              <svg width="11" height="11" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2.5" strokeLinecap="round" strokeLinejoin="round" aria-hidden="true">
                <rect x="9" y="9" width="13" height="13" rx="2" ry="2" />
                <path d="M5 15H4a2 2 0 0 1-2-2V4a2 2 0 0 1 2-2h9a2 2 0 0 1 2 2v1" />
              </svg>
            </button>
          )}

          {hasSeries && (
            <span className={styles.seriesScore}>
              Series: {seriesP1} - {seriesP2}
            </span>
          )}
        </div>

        {/* P2 Handle & Score */}
        <div className={`${styles.hudPlayer} ${styles.hudP2}`}>
          <span className={styles.hudScore}>{p2Score}</span>
          <span className={styles.playerName}>{p2Name}</span>
          <span
            className={`${styles.turnPip} ${activeTurnNum === 2 && !state?.finished ? styles.turnPipActive : ''}`}
            aria-hidden="true"
          />
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
