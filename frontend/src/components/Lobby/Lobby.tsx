import { signal } from '@preact/signals';
import {
  selectedTab,
  filteredGames,
  roomDirectory,
  gameCatalogue,
  lastPlayedGame,
  recordPlayedGame
} from '../../state/gameStore';
import { gameSocket } from '../../services/GameSocket';
import { GameCategory, GameMetadata, RoomDirectoryEntry } from '../../types/protocol';
import { Icon } from '../common/Icon';
import { GameboxLogo } from '../common/GameboxLogo';
import styles from './Lobby.module.css';

export const lobbyJoinCode = signal<string>('');

export function Lobby() {
  const handleJoinByCode = (e: Event) => {
    e.preventDefault();
    const clean = lobbyJoinCode.value.trim().toUpperCase();
    if (clean.length === 4) {
      gameSocket.joinRoom(clean);
      lobbyJoinCode.value = '';
    }
  };

  const handleGameSelect = (game: GameMetadata) => {
    recordPlayedGame(game.key);
    if (game.category === 'SOLO') {
      gameSocket.startSolo(game.key);
    } else {
      gameSocket.createRoom(game.key);
    }
  };

  const handleQuickMatch = () => {
    const key = lastPlayedGame.value || 'domino_solo';
    recordPlayedGame(key);
    gameSocket.startSolo(key);
  };

  const featuredGame = gameCatalogue.value.find(g => g.key === lastPlayedGame.value);
  const featuredTitle = featuredGame ? featuredGame.title : 'Dominoes Solo';

  const tabs: { label: string; value: GameCategory }[] = [
    { label: 'Solo AI', value: 'SOLO' },
    { label: '2P Duel', value: 'DUEL' },
    { label: 'Parlor Table', value: 'PARTY' }
  ];

  return (
    <div className={styles.lobby}>
      {/* Quick Match Marquee */}
      <div className={styles.marquee}>
        <div className={styles.marqueeLeft}>
          <div className={styles.marqueeEmblem}>
            <GameboxLogo variant="full" size={46} animated={true} />
          </div>
          <div className={styles.marqueeMeta}>
            <div className={styles.marqueeTitleRow}>
              <span className={styles.marqueeDot} />
              <h2 className={styles.marqueeTitle}>QUICK MATCH</h2>
            </div>
            <p className={styles.marqueeSub}>
              {featuredTitle} · Instant Solo AI
            </p>
          </div>
        </div>
        <button
          className={styles.marqueeCta}
          onClick={handleQuickMatch}
          type="button"
          aria-label="Start Quick Match"
        >
          <span>PLAY NOW</span>
          <span className={styles.ctaArrow}>▶</span>
        </button>
      </div>

      {/* Mode Filter Tabs */}
      <div className={styles.tabBar}>
        {tabs.map(tab => (
          <button
            key={tab.value}
            className={`${styles.tabBtn} ${selectedTab.value === tab.value ? styles.tabBtnActive : ''}`}
            onClick={() => (selectedTab.value = tab.value)}
          >
            {tab.label}
          </button>
        ))}
      </div>

      {/* Universal Join Code Bar (Accessible Across All Tabs) */}
      <form className={styles.joinCodeBar} onSubmit={handleJoinByCode}>
        <span className={styles.codePrompt}>&gt;_</span>
        <input
          className={styles.codeInput}
          type="text"
          maxLength={4}
          placeholder="ROOM CODE"
          value={lobbyJoinCode.value}
          onInput={(e) => (lobbyJoinCode.value = (e.target as HTMLInputElement).value)}
          aria-label="Room Code"
        />
        <button
          className={styles.btnJoin}
          type="submit"
          disabled={lobbyJoinCode.value.trim().length !== 4}
        >
          JOIN
        </button>
      </form>

      {/* Live Room Directory Feed - Auto-collapses completely when 0 active rooms */}
      {roomDirectory.value.length > 0 && (
        <div className={styles.liveFeedSection}>
          <div className={styles.sectionTitle}>
            <span>LIVE MATCH FEED</span>
            <span className={styles.activePill}>{roomDirectory.value.length} ACTIVE</span>
          </div>

          <div className={styles.directoryContainer}>
            {roomDirectory.value.map((room: RoomDirectoryEntry) => (
              <div key={room.code} className={styles.roomItem}>
                <div className={styles.roomMeta}>
                  <span className={styles.roomName}>
                    <span className={styles.radarBeacon} />
                    {room.host || 'Host'} · {room.game}
                  </span>
                  <span className={styles.roomSub}>
                    {room.players || 1}/{room.max || 2} players · Code: {room.code}
                  </span>
                </div>
                <button
                  className={styles.btnSolid}
                  onClick={() => gameSocket.joinRoom(room.code)}
                >
                  Join Match
                </button>
              </div>
            ))}
          </div>
        </div>
      )}

      {/* Unified Cartridge Rack */}
      <div className={styles.sectionTitle}>
        <span>CARTRIDGE RACK</span>
        <span className={styles.activePill}>{filteredGames.value.length} TITLES</span>
      </div>

      <div className={styles.cartridgeRack}>
        {filteredGames.value.length === 0 ? (
          <div className={styles.emptyDirectory}>
            Connecting to Host game directory...
          </div>
        ) : (
          filteredGames.value.map(game => (
            <div
              key={game.key}
              className={styles.cartridgeItem}
              onClick={() => handleGameSelect(game)}
            >
              <div className={styles.cartridgeMain}>
                <div className={`${styles.cartridgeIconBox} ${game.isDuel ? styles.cartridgeIconBoxDuel : ''}`}>
                  <Icon name={game.icon} size={20} />
                </div>
                <div className={styles.cartridgeText}>
                  <span className={styles.cartridgeTitle}>{game.title}</span>
                  <span className={styles.cartridgeDesc}>{game.description}</span>
                </div>
              </div>
              <div className={styles.cartridgeActions}>
                <span className={styles.capacityBadge}>{game.tag}</span>
                <button
                  className={game.category === 'SOLO' ? styles.btnCartridgePlay : styles.btnCartridgeHost}
                  type="button"
                  onClick={(e) => {
                    e.stopPropagation();
                    handleGameSelect(game);
                  }}
                  aria-label={`${game.category === 'SOLO' ? 'Play' : 'Host'} ${game.title}`}
                >
                  <span>{game.category === 'SOLO' ? 'PLAY' : 'HOST'}</span>
                  <span className={styles.btnArrow}>▶</span>
                </button>
              </div>
            </div>
          ))
        )}
      </div>
    </div>
  );
}
