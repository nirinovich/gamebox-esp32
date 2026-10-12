import {
  connectionStatus,
  latencyMs,
  playerName,
  setPlayerName,
  soundMuted,
  toggleSound,
  crtFilter,
  toggleCrt
} from '../../state/gameStore';
import { gameSocket } from '../../services/GameSocket';
import styles from './Header.module.css';

export function Header() {
  const status = connectionStatus.value;
  const ping = latencyMs.value;

  const handleNameChange = (e: Event) => {
    const val = (e.target as HTMLInputElement).value;
    setPlayerName(val);
    gameSocket.updateName(playerName.value);
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
        : 'Online'
      : status === 'CONNECTING'
      ? 'Connecting'
      : 'Offline';

  return (
    <>
      <header className={styles.header}>
        <div className={styles.logoWrap}>
          <span className={styles.logoIcon}>
            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2.4">
              <rect x="2" y="6" width="20" height="12" rx="4" />
              <path d="M6 12h4m-2-2v4m8-2h.01m3-2h.01" />
            </svg>
          </span>
          <span>Gamebox</span>
        </div>

        <div className={styles.headerControls}>
          <div className={`${styles.badge} ${statusClass}`}>
            <span className={styles.dot} />
            <span>{statusText}</span>
          </div>

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
        <input
          className={styles.profileInput}
          type="text"
          maxLength={14}
          value={playerName.value}
          onInput={handleNameChange}
          placeholder="Enter nickname"
        />
      </div>
    </>
  );
}
