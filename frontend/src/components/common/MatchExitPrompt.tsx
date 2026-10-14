import styles from './MatchExitPrompt.module.css';

interface MatchExitPromptProps {
  onConfirm: () => void;
  onCancel: () => void;
}

export function MatchExitPrompt({ onConfirm, onCancel }: MatchExitPromptProps) {
  return (
    <div className={styles.modalOverlay}>
      <div className={styles.modalBackdrop} onClick={onCancel} />
      <div className={styles.modalCard} role="dialog" aria-modal="true" aria-labelledby="forfeit-title">
        <div className={styles.modalHeader}>
          <div className={styles.alertIconBox}>
            <svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2.2" strokeLinecap="round" strokeLinejoin="round">
              <path d="m21.73 18-8-14a2 2 0 0 0-3.48 0l-8 14A2 2 0 0 0 4 21h16a2 2 0 0 0 1.73-3Z" />
              <line x1="12" y1="9" x2="12" y2="13" />
              <line x1="12" y1="17" x2="12.01" y2="17" />
            </svg>
          </div>
          <h3 id="forfeit-title" className={styles.modalTitle}>FORFEIT MATCH?</h3>
        </div>
        <p className={styles.modalDesc}>
          Leaving will concede victory to your opponent.
        </p>
        <div className={styles.modalActions}>
          <button className={styles.btnForfeit} onClick={onConfirm} type="button">
            FORFEIT & LEAVE
          </button>
          <button className={styles.btnCancel} onClick={onCancel} type="button">
            CANCEL
          </button>
        </div>
      </div>
    </div>
  );
}
