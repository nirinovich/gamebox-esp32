import { useEffect } from 'preact/hooks';
import { currentView, crtFilter } from './state/gameStore';
import { gameSocket } from './services/GameSocket';
import { Header } from './components/Header/Header';
import { Lobby } from './components/Lobby/Lobby';
import { WaitingRoom } from './components/WaitingRoom/WaitingRoom';
import { GameHud } from './components/GameHud/GameHud';
import { GameCanvas } from './components/GameCanvas/GameCanvas';
import { TrophyDrawer } from './components/TrophyDrawer/TrophyDrawer';

export function App() {
  useEffect(() => {
    gameSocket.init();
  }, []);

  useEffect(() => {
    if (crtFilter.value) {
      document.body.classList.add('crt-active');
    } else {
      document.body.classList.remove('crt-active');
    }
  }, [crtFilter.value]);

  return (
    <>
      <Header />
      {currentView.value === 'LOBBY' && <Lobby />}
      {currentView.value === 'WAITING' && <WaitingRoom />}
      {currentView.value === 'GAME' && (
        <div style={{ width: '100%', display: 'flex', flexDirection: 'column', gap: '8px' }}>
          <GameHud />
          <GameCanvas />
        </div>
      )}
      <TrophyDrawer />
    </>
  );
}
