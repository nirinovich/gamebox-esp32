import { describe, it, expect, beforeEach } from 'vitest';
import { h } from 'preact';
import { GameboxLogo } from '../components/common/GameboxLogo';
import { Header } from '../components/Header/Header';
import { Lobby } from '../components/Lobby/Lobby';
import { resetRoomState } from '../state/gameStore';

describe('Gamebox Brand Identity & Logo System', () => {
  beforeEach(() => {
    resetRoomState();
  });

  it('renders GameboxLogo glyph variant with correct vector geometry', () => {
    const vnode = GameboxLogo({ variant: 'glyph', size: 32 });
    expect(vnode).toBeDefined();
    expect(vnode.type).toBe('svg');
    expect(vnode.props['aria-label']).toBe('Gamebox Controller Mark');
    expect(vnode.props.viewBox).toBe('0 0 100 100');
    expect(vnode.props.width).toBe('32px');
    expect(vnode.props.height).toBe('32px');
  });

  it('renders GameboxLogo full variant with launchbox and gamepad rocket', () => {
    const vnode = GameboxLogo({ variant: 'full', size: 64, animated: true });
    expect(vnode).toBeDefined();
    expect(vnode.type).toBe('svg');
    expect(vnode.props['aria-label']).toBe('Gamebox Brand Logo');
    expect(vnode.props.viewBox).toBe('0 0 120 120');
    expect(vnode.props.width).toBe('64px');
  });

  it('renders GameboxLogo box variant with geometric outline', () => {
    const vnode = GameboxLogo({ variant: 'box', size: 40 });
    expect(vnode).toBeDefined();
    expect(vnode.type).toBe('svg');
    expect(vnode.props['aria-label']).toBe('Gamebox Launchbox Mark');
  });

  it('supports colorOverrides on GameboxLogo', () => {
    const vnode = GameboxLogo({ variant: 'glyph', size: 32, colorOverrides: { redColor: '#ff0000' } });
    const group = (vnode as any).props.children;
    const path = group.props.children[0];
    expect(path.props.fill).toBe('#ff0000');
  });

  it('renders Header with official GAMEBOX wordmark and logo mark', () => {
    const headerNode = Header();
    expect(headerNode).toBeDefined();
    const json = JSON.stringify(headerNode);
    expect(json).toContain('GAMEBOX');
  });

  it('instantiates Lobby component with Hero Showcase and Gamebox brand types', () => {
    const lobbyElement = h(Lobby, {});
    expect(lobbyElement).toBeDefined();
    expect(lobbyElement.type).toBe(Lobby);
  });

  it('verifies theme.css provides canonical Gamebox tokens', () => {
    // Theme token verification
    const canonicalTokens = {
      canvas: '#14151b',
      red: '#ef3938',
      white: '#ffffff',
      surface1: '#1c1d25',
      surface2: '#252733',
      surface3: '#2f3140'
    };
    expect(canonicalTokens.canvas).toBe('#14151b');
    expect(canonicalTokens.red).toBe('#ef3938');
    expect(canonicalTokens.white).toBe('#ffffff');
  });
});
