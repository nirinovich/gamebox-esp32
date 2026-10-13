import { useState } from 'preact/hooks';
import {
  selectedTab,
  filteredGames,
  roomDirectory
} from '../../state/gameStore';
import { gameSocket } from '../../services/GameSocket';
import { GameCategory, GameMetadata, RoomDirectoryEntry } from '../../types/protocol';
import { Icon } from '../common/Icon';
import styles from './Lobby.module.css';

export function Lobby() {
  const [joinCode, setJoinCode] = useState('');

  const handleJoinByCode = (e: Event) => {
    e.preventDefault();
    const clean = joinCode.trim().toUpperCase();
    if (clean.length === 4) {
      gameSocket.joinRoom(clean);
      setJoinCode('');
    }
  };

  const handleGameSelect = (game: GameMetadata) => {
    if (game.category === 'SOLO') {
      gameSocket.startSolo(game.key);
    } else {
      gameSocket.createRoom(game.key);
    }
  };

  const tabs: { label: string; value: GameCategory }[] = [
    { label: 'All Modes', value: 'ALL' },
    { label: 'Solo AI', value: 'SOLO' },
    { label: '2P Duel', value: 'DUEL' },
    { label: 'Parlor Table', value: 'PARTY' }
  ];

  return (
    <div className={styles.lobby}>
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

      {/* Join with 4-letter Code */}
      <form className={styles.joinCodeBar} onSubmit={handleJoinByCode}>
        <input
          className={styles.codeInput}
          type="text"
          maxLength={4}
          placeholder="ROOM CODE"
          value={joinCode}
          onInput={(e) => setJoinCode((e.target as HTMLInputElement).value)}
        />
        <button
          className={styles.btnJoin}
          type="submit"
          disabled={joinCode.trim().length !== 4}
        >
          JOIN
        </button>
      </form>

      {/* Live Room Directory Feed */}
      <div className={styles.sectionTitle}>
        <span>LIVE MATCH FEED</span>
        <span>{roomDirectory.value.length} ACTIVE</span>
      </div>

      <div className={styles.directoryContainer}>
        {roomDirectory.value.length === 0 ? (
          <div className={styles.emptyDirectory}>
            No open matches. Select a multiplayer game below to host!
          </div>
        ) : (
          roomDirectory.value.map((room: RoomDirectoryEntry) => (
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
          ))
        )}
      </div>

      {/* Game Catalogue */}
      <div className={styles.sectionTitle}>
        <span>GAME DIRECTORY</span>
      </div>

      <div className={styles.gameList}>
        {filteredGames.value.length === 0 ? (
          <div className={styles.emptyDirectory}>
            Connecting to Host game directory...
          </div>
        ) : (
          filteredGames.value.map(game => (
          <div
            key={game.key}
            className={styles.gameItem}
            onClick={() => handleGameSelect(game)}
          >
            <div className={styles.gameInfo}>
              <div className={`${styles.gameIconBox} ${game.isDuel ? styles.gameIconBoxDuel : ''}`}>
                <Icon name={game.icon} size={20} />
              </div>
              <div className={styles.gameText}>
                <span className={styles.gameTitle}>{game.title}</span>
                <span className={styles.gameDesc}>{game.description}</span>
              </div>
            </div>
            <span className={styles.gameTag}>{game.tag}</span>
          </div>
        )))}
      </div>
    </div>
  );
}
