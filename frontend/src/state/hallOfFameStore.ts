import { signal, computed } from '@preact/signals';

export interface GameStatRecord {
  wins: number;
  losses: number;
  draws: number;
}

export type StatsMap = Record<string, GameStatRecord>;

const STORAGE_KEY = 'gamebox_hall_of_fame';

function loadStats(): StatsMap {
  try {
    if (typeof window !== 'undefined' && window.localStorage) {
      const data = window.localStorage.getItem(STORAGE_KEY);
      if (data) return JSON.parse(data);
    }
  } catch {}
  return {};
}

function saveStats(stats: StatsMap) {
  try {
    if (typeof window !== 'undefined' && window.localStorage) {
      window.localStorage.setItem(STORAGE_KEY, JSON.stringify(stats));
    }
  } catch {}
}

export const statsStore = signal<StatsMap>(loadStats());
export const showTrophyDrawer = signal<boolean>(false);

export function recordMatchOutcome(gameKey: string, outcome: 'WIN' | 'LOSS' | 'DRAW') {
  const current = { ...statsStore.value };
  const rec = current[gameKey] || { wins: 0, losses: 0, draws: 0 };

  if (outcome === 'WIN') rec.wins++;
  else if (outcome === 'LOSS') rec.losses++;
  else rec.draws++;

  current[gameKey] = rec;
  statsStore.value = current;
  saveStats(current);
}

export function resetAllStats() {
  statsStore.value = {};
  saveStats({});
}

export const totalStats = computed(() => {
  let totalWins = 0;
  let totalLosses = 0;
  let totalDraws = 0;

  for (const key of Object.keys(statsStore.value)) {
    const rec = statsStore.value[key];
    totalWins += rec.wins;
    totalLosses += rec.losses;
    totalDraws += rec.draws;
  }

  const totalPlayed = totalWins + totalLosses + totalDraws;
  const winRate = totalPlayed > 0 ? Math.round((totalWins / totalPlayed) * 100) : 0;

  let tier = 'Bronze';
  let tierColor = '#cd7f32';
  let tierBadge = 'bronze';

  if (totalWins >= 16) {
    tier = 'Cyber Master';
    tierColor = '#38bdf8';
    tierBadge = 'cyber_master';
  } else if (totalWins >= 8) {
    tier = 'Gold Champion';
    tierColor = '#eab308';
    tierBadge = 'gold_champion';
  } else if (totalWins >= 3) {
    tier = 'Silver Ace';
    tierColor = '#94a3b8';
    tierBadge = 'silver_ace';
  }

  return {
    totalWins,
    totalLosses,
    totalDraws,
    totalPlayed,
    winRate,
    tier,
    tierColor,
    tierBadge
  };
});
