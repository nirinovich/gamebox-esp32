import { useState } from 'preact/hooks';
import {
  showTrophyDrawer,
  totalStats,
  statsStore,
  resetAllStats
} from '../../state/hallOfFameStore';
import { Icon } from '../common/Icon';
import styles from './TrophyDrawer.module.css';

export function TrophyDrawer() {
  const [confirmReset, setConfirmReset] = useState(false);
  const stats = totalStats.value;
  const gameStats = statsStore.value;
  const isOpen = showTrophyDrawer.value;

  if (!isOpen) return null;

  const handleClose = () => {
    showTrophyDrawer.value = false;
    setConfirmReset(false);
  };

  const handleConfirmReset = () => {
    resetAllStats();
    setConfirmReset(false);
  };

  return (
    <div className={styles.overlay} onClick={handleClose}>
      <div className={styles.drawer} onClick={(e) => e.stopPropagation()}>
        <div className={styles.handle} />

        <div className={styles.headerRow}>
          <div className={styles.title}>
            <svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
              <path d="M6 9H4.5a2.5 2.5 0 0 1 0-5H6" />
              <path d="M18 9h1.5a2.5 2.5 0 0 0 0-5H18" />
              <path d="M4 22h16" />
              <path d="M10 14.66V17c0 .55-.45 1-1 1H7" />
              <path d="M14 14.66V17c0 .55.45 1 1 1h2" />
              <path d="M18 2H6v7a6 6 0 0 0 12 0V2Z" />
            </svg>
            <span>Hall of Fame</span>
          </div>
          <button className={styles.closeBtn} onClick={handleClose} title="Close">
            <Icon name="close" size={16} />
          </button>
        </div>

        {/* Tier Card */}
        <div className={styles.tierCard} style={{ borderColor: stats.tierColor }}>
          <div className={styles.tierIcon} style={{ color: stats.tierColor }}>
            <Icon name={stats.tierBadge} size={28} />
          </div>
          <div className={styles.tierMeta}>
            <span className={styles.tierLabel}>Current Rank</span>
            <span className={styles.tierName} style={{ color: stats.tierColor }}>
              {stats.tier}
            </span>
          </div>
        </div>

        {/* Aggregate Stats */}
        <div className={styles.statsGrid}>
          <div className={styles.statBox}>
            <div className={styles.statValue} style={{ color: 'var(--success)' }}>{stats.totalWins}</div>
            <div className={styles.statLabel}>Victories</div>
          </div>
          <div className={styles.statBox}>
            <div className={styles.statValue} style={{ color: 'var(--danger)' }}>{stats.totalLosses}</div>
            <div className={styles.statLabel}>Defeats</div>
          </div>
          <div className={styles.statBox}>
            <div className={styles.statValue} style={{ color: 'var(--p1-cyan)' }}>{stats.winRate}%</div>
            <div className={styles.statLabel}>Win Rate</div>
          </div>
        </div>

        {/* Per-Game Breakdown */}
        {Object.keys(gameStats).length > 0 && (
          <div className={styles.gameList}>
            {Object.keys(gameStats).map((k) => {
              const rec = gameStats[k];
              return (
                <div key={k} className={styles.gameRow}>
                  <span className={styles.gameRowName}>{k.replace('_', ' ').toUpperCase()}</span>
                  <span className={styles.gameRowScore}>
                    {rec.wins}W - {rec.losses}L {rec.draws > 0 ? `(${rec.draws}D)` : ''}
                  </span>
                </div>
              );
            })}
          </div>
        )}

        {/* Two-step reset confirmation */}
        <div className={styles.footerActions}>
          {!confirmReset ? (
            <button className={styles.btnReset} onClick={() => setConfirmReset(true)}>
              Reset Statistics
            </button>
          ) : (
            <div className={styles.confirmRow}>
              <button className={styles.btnConfirm} onClick={handleConfirmReset}>
                Confirm Wipe
              </button>
              <button className={styles.btnCancel} onClick={() => setConfirmReset(false)}>
                Cancel
              </button>
            </div>
          )}
        </div>
      </div>
    </div>
  );
}
