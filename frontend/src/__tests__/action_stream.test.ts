import { describe, it, expect, vi, beforeEach } from 'vitest';
import { gameState, resetRoomState } from '../state/gameStore';
import { gameSocket } from '../services/GameSocket';
import { ActionToolbar } from '../components/ActionToolbar/ActionToolbar';
import { ActionModal } from '../components/ActionModal/ActionModal';
import { getGameRenderer, gameRendererMap } from '../registry/gameRegistry';

describe('Authoritative ActionStream and Prompt Protocol', () => {
  beforeEach(() => {
    resetRoomState();
    vi.restoreAllMocks();
  });

  it('ingests available_actions in gameState snapshot', () => {
    gameState.value = {
      type: 'state',
      game: 'domino',
      turn: 1,
      available_actions: [
        { id: 'draw', label: 'Draw Tile (14)', enabled: true, style: 'highlight' },
        { id: 'pass', label: 'Pass Turn', enabled: false }
      ]
    };

    expect(gameState.value.available_actions).toBeDefined();
    expect(gameState.value.available_actions?.length).toBe(2);
    expect(gameState.value.available_actions?.[0].id).toBe('draw');
    expect(gameState.value.available_actions?.[0].style).toBe('highlight');
    expect(gameState.value.available_actions?.[1].enabled).toBe(false);
  });

  it('renders ActionToolbar JSX when available_actions is populated', () => {
    gameState.value = {
      type: 'state',
      game: 'battleship',
      available_actions: [
        { id: 'auto_place', label: 'DEPLOY FLEET', enabled: true, style: 'primary' }
      ]
    };

    const vnode = ActionToolbar();
    expect(vnode).not.toBeNull();
    expect(vnode?.props['data-testid']).toBe('action-toolbar');
    const buttons = vnode?.props.children;
    expect(buttons.length).toBe(1);
    expect(buttons[0].props['data-testid']).toBe('action-auto_place');
    expect(buttons[0].props.disabled).toBe(false);
  });

  it('returns null from ActionToolbar when no actions are available', () => {
    gameState.value = { type: 'state', game: 'snake' };
    const vnode = ActionToolbar();
    expect(vnode).toBeNull();
  });

  it('ingests prompt in gameState snapshot and renders ActionModal', () => {
    gameState.value = {
      type: 'state',
      game: 'domino',
      prompt: {
        title: 'Place Domino',
        choices: [
          { cmd: 'play', label: 'Place Left End', params: { index: 0, end: 'left' } },
          { cmd: 'play', label: 'Place Right End', params: { index: 0, end: 'right' } }
        ]
      }
    };

    expect(gameState.value.prompt).toBeDefined();
    expect(gameState.value.prompt?.title).toBe('Place Domino');

    const modalVnode = ActionModal();
    expect(modalVnode).not.toBeNull();
    expect(modalVnode?.props['data-testid']).toBe('action-modal');
  });

  it('dispatches cmd when action or prompt choice is sent', () => {
    const sendSpy = vi.spyOn(gameSocket, 'send').mockImplementation(() => {});

    // Action execution
    gameSocket.send({ cmd: 'auto_place' });
    expect(sendSpy).toHaveBeenCalledWith({ cmd: 'auto_place' });

    // Choice execution
    gameSocket.send({ cmd: 'play', index: 1, end: 'left' });
    expect(sendSpy).toHaveBeenCalledWith({ cmd: 'play', index: 1, end: 'left' });
  });

  it('dynamically discovers client game packages via Vite glob registry without central switches', () => {
    const registeredKeys = Object.keys(gameRendererMap);
    expect(registeredKeys.length).toBeGreaterThan(0);

    // Verify all 7 core game types resolve
    const expectedGames = [
      'snake_solo',
      'ttt_solo',
      'connect4_solo',
      'pong_solo',
      'tron_solo',
      'battleship_solo',
      'domino_solo'
    ];

    for (const gid of expectedGames) {
      const renderer = getGameRenderer(gid);
      expect(renderer).toBeDefined();
      expect(typeof renderer.render).toBe('function');
      expect(typeof renderer.handlePointerDown).toBe('function');
      expect(typeof renderer.resize).toBe('function');
    }
  });
});
