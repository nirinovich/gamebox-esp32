import { gameState } from '../../state/gameStore';
import { gameSocket } from '../../services/GameSocket';
import { AvailableAction } from '../../types/protocol';
import styles from './ActionToolbar.module.css';

export function ActionToolbar() {
  const actions: AvailableAction[] = gameState.value?.available_actions || [];

  if (!actions || actions.length === 0) {
    return null;
  }

  const handleAction = (act: AvailableAction) => {
    if (act.enabled === false) return;
    gameSocket.send({ cmd: act.id, ...(act.params || {}) });
  };

  return (
    <div className={styles.actionToolbar} data-testid="action-toolbar">
      {actions.map((act) => {
        const isHighlight = act.style === 'highlight';
        const isPrimary = act.style === 'primary' || act.style === 'solid';
        const className = `${styles.btnAction} ${
          isHighlight ? styles.btnHighlight : isPrimary ? styles.btnPrimary : ''
        }`;

        return (
          <button
            key={act.id}
            className={className}
            disabled={act.enabled === false}
            onClick={() => handleAction(act)}
            data-testid={`action-${act.id}`}
          >
            {act.label}
          </button>
        );
      })}
    </div>
  );
}
