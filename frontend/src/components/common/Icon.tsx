import {
  X,
  CircleDot,
  Zap,
  Grid2X2,
  Dices,
  Trophy,
  Diamond,
  Medal,
  Award,
  LucideProps
} from 'lucide-preact';

export type IconName =
  | 'domino_solo'
  | 'domino_duel'
  | 'domino_pvp'
  | 'poker_solo'
  | 'poker_table'
  | 'ludo_solo'
  | 'ludo_quick'
  | 'ludo_classic'
  | 'katro_solo'
  | 'katro_pvp'
  | 'cyber_master'
  | 'gold_champion'
  | 'silver_ace'
  | 'bronze'
  | 'close'
  | string;

interface IconProps extends LucideProps {
  name: IconName | string;
  size?: number | string;
}

const ICON_MAP: Record<string, any> = {
  domino_solo: Grid2X2,
  domino_duel: Grid2X2,
  domino_pvp: Grid2X2,
  poker_solo: Diamond,
  poker_table: Trophy,
  poker: Diamond,
  ludo_solo: Dices,
  ludo_quick: Zap,
  ludo_classic: Trophy,
  ludo: Dices,
  katro_solo: CircleDot,
  katro_pvp: CircleDot,
  katro: CircleDot,
  cyber_master: Diamond,
  gold_champion: Trophy,
  silver_ace: Medal,
  bronze: Award,
  close: X
};

export function Icon({ name, size = 18, strokeWidth = 2, ...props }: IconProps) {
  const Component = ICON_MAP[name] || Dices;
  return <Component size={size} strokeWidth={strokeWidth} {...props} />;
}
