import { gameState } from '../../state/gameStore';
import { gameSocket } from '../../services/GameSocket';
import { ActionPrompt, ActionChoice } from '../../types/protocol';
import styles from './ActionModal.module.css';

export function ActionModal() {
  const prompt: ActionPrompt | undefined = gameState.value?.prompt;

  if (!prompt || !prompt.choices || prompt.choices.length === 0) {
    return null;
  }

  const handleChoice = (choice: ActionChoice) => {
    gameSocket.send({ cmd: choice.cmd, ...(choice.params || {}) });
  };

  return (
    <div className={styles.modalOverlay} data-testid="action-modal">
      <div className={styles.modalCard}>
        <h3 className={styles.promptTitle}>{prompt.title}</h3>
        <div className={styles.choicesGrid}>
          {prompt.choices.map((choice, idx) => (
            <button
              key={idx}
              className={styles.choiceBtn}
              onClick={() => handleChoice(choice)}
              data-testid={`choice-${idx}`}
            >
              {choice.label}
            </button>
          ))}
        </div>
      </div>
    </div>
  );
}
