import { describe, it, expect, beforeEach, vi } from 'vitest';
import {
  currentView,
  activeRoom,
  gameState,
  playerName,
  resetRoomState
} from '../state/gameStore';
import { gameSocket } from '../services/GameSocket';
import { Header, showExitPrompt } from '../components/Header/Header';
import { GameHud } from '../components/GameHud/GameHud';
import { MatchExitPrompt } from '../components/common/MatchExitPrompt';

describe('In-Game UX, Navigation Lifecycle, and HUD Turn Guidance', () => {
  beforeEach(() => {
    resetRoomState();
    showExitPrompt.value = false;
    currentView.value = 'LOBBY';
    playerName.value = 'Viper';
    vi.restoreAllMocks();
  });

  describe('Header Contextual Navigation & Profile Bar Lifecycle', () => {
    it('renders GAMEBOX wordmark and profile bar in LOBBY view', () => {
      currentView.value = 'LOBBY';
      const vnode = Header();
      const json = JSON.stringify(vnode);

      expect(json).toContain('GAMEBOX');
      expect(json).not.toContain('⏴ LEAVE');
      expect(json).toContain('profileBar');
      expect(json).toContain('Player Callsign');
    });

    it('replaces logo text with [ ⏴ LEAVE ] and unmounts profile bar in GAME view', () => {
      currentView.value = 'GAME';
      activeRoom.value = {
        code: 'SOLO',
        isHost: true,
        gameType: 'katro_solo',
        playerNum: 1,
        opponentName: 'Bot'
      };

      const vnode = Header();
      const json = JSON.stringify(vnode);

      expect(json).toContain('⏴ LEAVE');
      expect(json).not.toContain('logoText');
      // Profile bar must be completely unmounted from the DOM
      expect(json).not.toContain('profileBar');
      expect(json).not.toContain('Player Callsign');
      expect(json).not.toContain('shuffleBtn');
    });

    it('unmounts profile bar in WAITING view as well', () => {
      currentView.value = 'WAITING';
      const vnode = Header();
      const json = JSON.stringify(vnode);

      expect(json).toContain('⏴ LEAVE');
      expect(json).not.toContain('profileBar');
    });

    it('triggers instant 1-tap exit in Solo AI match', () => {
      const leaveSpy = vi.spyOn(gameSocket, 'leaveRoom').mockImplementation(() => {});
      currentView.value = 'GAME';
      activeRoom.value = {
        code: 'SOLO',
        isHost: true,
        gameType: 'katro_solo',
        playerNum: 1,
        opponentName: 'Bot'
      };

      const vnode = Header();
      // Locate the leave button in the rendered virtual tree and invoke its onClick
      const headerChild = (vnode as any).props.children[0];
      const logoWrap = headerChild.props.children[0];
      const leaveBtn = logoWrap.props.children[1];

      expect(leaveBtn.props['aria-label']).toBe('Leave Match');
      leaveBtn.props.onClick();

      expect(leaveSpy).toHaveBeenCalledTimes(1);
    });

    it('triggers instant 1-tap exit in WAITING pre-game view', () => {
      const leaveSpy = vi.spyOn(gameSocket, 'leaveRoom').mockImplementation(() => {});
      currentView.value = 'WAITING';
      activeRoom.value = {
        code: 'ABCD',
        isHost: true,
        gameType: 'katro_pvp',
        playerNum: 1,
        opponentName: 'Waiting...'
      };

      const vnode = Header();
      const headerChild = (vnode as any).props.children[0];
      const logoWrap = headerChild.props.children[0];
      const leaveBtn = logoWrap.props.children[1];

      leaveBtn.props.onClick();
      expect(leaveSpy).toHaveBeenCalledTimes(1);
    });

    it('intercepts exit with confirmation modal in live multiplayer duel', () => {
      const leaveSpy = vi.spyOn(gameSocket, 'leaveRoom').mockImplementation(() => {});
      currentView.value = 'GAME';
      activeRoom.value = {
        code: 'ABCD',
        isHost: true,
        gameType: 'katro_pvp',
        playerNum: 1,
        opponentName: 'Rocket'
      };

      const vnode = Header();
      const headerChild = (vnode as any).props.children[0];
      const logoWrap = headerChild.props.children[0];
      const leaveBtn = logoWrap.props.children[1];

      // Initial render: no forfeit modal
      expect(JSON.stringify(vnode)).not.toContain('FORFEIT MATCH?');

      // Click leave button -> should NOT call leaveRoom yet
      leaveBtn.props.onClick();
      expect(leaveSpy).not.toHaveBeenCalled();
      expect(showExitPrompt.value).toBe(true);

      // Next render contains MatchExitPrompt component vnode
      const vnodeAfterClick = Header();
      const exitPromptVNode = (vnodeAfterClick as any).props.children[2];
      expect(exitPromptVNode.type).toBe(MatchExitPrompt);
      const renderedPrompt = exitPromptVNode.type(exitPromptVNode.props);
      expect(JSON.stringify(renderedPrompt)).toContain('FORFEIT MATCH?');
    });
  });

  describe('MatchExitPrompt Component', () => {
    it('renders warning heading, explanatory copy, and dual action buttons', () => {
      const onConfirm = vi.fn();
      const onCancel = vi.fn();

      const vnode = MatchExitPrompt({ onConfirm, onCancel });
      const json = JSON.stringify(vnode);

      expect(json).toContain('FORFEIT MATCH?');
      expect(json).toContain('Leaving this active multiplayer duel will concede victory');
      expect(json).toContain('FORFEIT & LEAVE');
      expect(json).toContain('CANCEL');
    });

    it('calls onConfirm when forfeit button clicked', () => {
      const onConfirm = vi.fn();
      const onCancel = vi.fn();

      const vnode = MatchExitPrompt({ onConfirm, onCancel });
      const modalCard = (vnode as any).props.children[1];
      const modalActions = modalCard.props.children[2];
      const forfeitBtn = modalActions.props.children[0];
      const cancelBtn = modalActions.props.children[1];

      forfeitBtn.props.onClick();
      expect(onConfirm).toHaveBeenCalledTimes(1);
      expect(onCancel).not.toHaveBeenCalled();

      cancelBtn.props.onClick();
      expect(onCancel).toHaveBeenCalledTimes(1);
    });
  });

  describe('GameHud Turn Guidance and Anti-Slop Theming', () => {
    it('returns null when activeRoom is null', () => {
      activeRoom.value = null;
      expect(GameHud()).toBeNull();
    });

    it('renders canonical Gamebox tokens and no unbranded cyan or pink', () => {
      activeRoom.value = {
        code: 'SOLO',
        isHost: true,
        gameType: 'katro_solo',
        playerNum: 1,
        opponentName: 'Bot'
      };
      gameState.value = {
        type: 'state',
        scores: [4, 2],
        turn: 1
      };

      const vnode = GameHud();
      const json = JSON.stringify(vnode);

      expect(json).not.toContain('dotCyan');
      expect(json).not.toContain('dotPink');
      expect(json).toContain('Viper');
      expect(json).toContain('Bot');
      expect(json).toContain('4');
      expect(json).toContain('2');
    });

    it('displays YOUR TURN with pulsing radar dot on player turn in Solo mode', () => {
      activeRoom.value = {
        code: 'SOLO',
        isHost: true,
        gameType: 'katro_solo',
        playerNum: 1,
        opponentName: 'Bot'
      };
      gameState.value = {
        type: 'state',
        turn: 1,
        scores: [0, 0]
      };

      const vnode = GameHud();
      const json = JSON.stringify(vnode);

      expect(json).toContain('YOUR TURN');
      expect(json).toContain('radarDot');
    });

    it('displays AI THINKING... on bot turn in Solo mode', () => {
      activeRoom.value = {
        code: 'SOLO',
        isHost: true,
        gameType: 'katro_solo',
        playerNum: 1,
        opponentName: 'Bot'
      };
      gameState.value = {
        type: 'state',
        turn: 2,
        scores: [0, 0]
      };

      const vnode = GameHud();
      const json = JSON.stringify(vnode);

      expect(json).toContain('AI THINKING...');
      expect(json).not.toContain('radarDot');
    });

    it('displays OPPONENT\'S TURN in multiplayer duel when active turn is other player', () => {
      activeRoom.value = {
        code: 'WXYZ',
        isHost: true,
        gameType: 'katro_pvp',
        playerNum: 1,
        opponentName: 'Rocket'
      };
      gameState.value = {
        type: 'state',
        turn: 2,
        scores: [10, 12]
      };

      const vnode = GameHud();
      const json = JSON.stringify(vnode);

      expect(json).toContain("OPPONENT'S TURN");
      expect(json).not.toContain('radarDot');
    });

    it('displays MATCH OVER when game is finished', () => {
      activeRoom.value = {
        code: 'WXYZ',
        isHost: true,
        gameType: 'katro_pvp',
        playerNum: 1,
        opponentName: 'Rocket'
      };
      gameState.value = {
        type: 'state',
        turn: 1,
        finished: true,
        winner: 1,
        scores: [32, 0]
      };

      const vnode = GameHud();
      const json = JSON.stringify(vnode);

      expect(json).toContain('MATCH OVER');
      expect(json).not.toContain('radarDot');
    });

    it('omits Room Code badge in Solo mode', () => {
      activeRoom.value = {
        code: 'SOLO',
        isHost: true,
        gameType: 'domino_solo',
        playerNum: 1,
        opponentName: 'Bot'
      };

      const vnode = GameHud();
      const json = JSON.stringify(vnode);

      expect(json).not.toContain('roomCodeBadge');
    });

    it('renders monospace Room Code badge in multiplayer mode and handles copy click', async () => {
      const writeTextMock = vi.fn().mockResolvedValue(undefined);
      Object.defineProperty(navigator, 'clipboard', {
        value: { writeText: writeTextMock },
        configurable: true,
        writable: true
      });

      activeRoom.value = {
        code: 'KTRX',
        isHost: true,
        gameType: 'katro_pvp',
        playerNum: 1,
        opponentName: 'Rocket'
      };
      gameState.value = {
        type: 'state',
        turn: 1,
        scores: [0, 0]
      };

      const vnode = GameHud();
      const json = JSON.stringify(vnode);

      expect(json).toContain('roomCodeBadge');
      expect(json).toContain('KTRX');

      // Find room code button
      const hud = (vnode as any).props.children[0];
      const hudCenter = hud.props.children[1];
      const roomCodeBtn = hudCenter.props.children[1];

      await roomCodeBtn.props.onClick();
      expect(writeTextMock).toHaveBeenCalledWith('KTRX');
    });
  });
});
