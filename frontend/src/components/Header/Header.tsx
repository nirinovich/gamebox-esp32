import {
  connectionStatus,
  latencyMs,
  playerName,
  setPlayerName,
  shufflePlayerName,
  soundMuted,
  toggleSound,
  crtFilter,
  toggleCrt
} from '../../state/gameStore';
import { showTrophyDrawer } from '../../state/hallOfFameStore';
import { gameSocket } from '../../services/GameSocket';
import { GameboxLogo } from '../common/GameboxLogo';
import styles from './Header.module.css';

export function Header() {
  const status = connectionStatus.value;
  const ping = latencyMs.value;

  const handleNameInput = (e: Event) => {
    const val = (e.target as HTMLInputElement).value;
    playerName.value = val.slice(0, 14);
    if (val.trim() && val.trim().toLowerCase() !== 'guest') {
      setPlayerName(val);
      gameSocket.updateName(playerName.value);
    }
  };

  const handleNameBlur = () => {
    if (!playerName.value.trim() || playerName.value.trim().toLowerCase() === 'guest') {
      const next = shufflePlayerName();
      gameSocket.updateName(next);
    }
  };

  const handleNameKeyDown = (e: KeyboardEvent) => {
    if (e.key === 'Enter') {
      (e.target as HTMLInputElement).blur();
    }
  };

  const handleShuffle = () => {
    const next = shufflePlayerName();
    gameSocket.updateName(next);
  };

  const statusClass =
    status === 'CONNECTED'
      ? styles.connected
      : status === 'CONNECTING'
      ? styles.connecting
      : styles.disconnected;

  const statusText =
    status === 'CONNECTED'
      ? ping !== null
        ? `${ping} ms`
        : 'ONLINE'
      : status === 'CONNECTING'
      ? 'CONNECTING'
      : 'OFFLINE';

  return (
    <>
      <header className={styles.header}>
        <div className={styles.logoWrap}>
          <span className={styles.logoIcon}>
            <GameboxLogo variant="glyph" size={24} animated={true} />
          </span>
          <span className={styles.logoText}>GAMEBOX</span>
        </div>

        <div className={styles.headerControls}>
          <div className={`${styles.badge} ${statusClass}`}>
            <span className={styles.dot} />
            <span>{statusText}</span>
          </div>

          <button
            className={`${styles.iconBtn} ${showTrophyDrawer.value ? styles.iconBtnActive : ''}`}
            onClick={() => {
              showTrophyDrawer.value = !showTrophyDrawer.value;
            }}
            title="Hall of Fame & Trophies"
          >
            <svg width="15" height="15" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
              <path d="M6 9H4.5a2.5 2.5 0 0 1 0-5H6" />
              <path d="M18 9h1.5a2.5 2.5 0 0 0 0-5H18" />
              <path d="M4 22h16" />
              <path d="M10 14.66V17c0 .55-.45 1-1 1H7" />
              <path d="M14 14.66V17c0 .55.45 1 1 1h2" />
              <path d="M18 2H6v7a6 6 0 0 0 12 0V2Z" />
            </svg>
          </button>

          <button
            className={`${styles.iconBtn} ${crtFilter.value ? styles.iconBtnActive : ''}`}
            onClick={toggleCrt}
            title="Toggle CRT Scanline Filter"
          >
            <svg width="15" height="15" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2">
              <rect x="2" y="4" width="20" height="15" rx="2" />
              <polyline points="17 2 12 7 7 2" />
            </svg>
          </button>

          <button
            className={styles.iconBtn}
            onClick={toggleSound}
            title={soundMuted.value ? 'Unmute Audio' : 'Mute Audio'}
          >
            {soundMuted.value ? (
              <svg width="15" height="15" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2">
                <polygon points="11 5 6 9 2 9 2 15 6 15 11 19 11 5" />
                <line x1="23" y1="9" x2="17" y2="15" />
                <line x1="17" y1="9" x2="23" y2="15" />
              </svg>
            ) : (
              <svg width="15" height="15" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2">
                <polygon points="11 5 6 9 2 9 2 15 6 15 11 19 11 5" />
                <path d="M19.07 4.93a10 10 0 0 1 0 14.14M15.54 8.46a5 5 0 0 1 0 7.07" />
              </svg>
            )}
          </button>
        </div>
      </header>

      <div className={styles.profileBar}>
        <span className={styles.profileLabel}>Player</span>
        <div className={styles.profileInputWrapper}>
          <input
            className={styles.profileInput}
            type="text"
            maxLength={14}
            value={playerName.value}
            onInput={handleNameInput}
            onBlur={handleNameBlur}
            onKeyDown={handleNameKeyDown}
            placeholder="Callsign"
            aria-label="Player Callsign"
          />
          <button
            className={styles.shuffleBtn}
            onClick={handleShuffle}
            type="button"
            title="Shuffle Callsign (↺)"
            aria-label="Shuffle Callsign"
          >
            <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2.5" strokeLinecap="round" strokeLinejoin="round">
              <path d="M21.5 2v6h-6M21.34 15.57a10 10 0 1 1-.57-8.38l5.67-5.67" />
            </svg>
          </button>
        </div>
      </div>
    </>
  );
}
