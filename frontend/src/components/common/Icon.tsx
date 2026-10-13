import {
  Activity,
  Swords,
  Circle,
  X,
  CircleDot,
  Columns3,
  SlidersHorizontal,
  Zap,
  Cpu,
  Sparkles,
  Anchor,
  Crosshair,
  Grid2X2,
  Dices,
  Trophy,
  Diamond,
  Medal,
  Award,
  LucideProps
} from 'lucide-preact';

export type IconName =
  | 'snake_solo'
  | 'snake_duel'
  | 'ttt_solo'
  | 'ttt_pvp'
  | 'connect4_solo'
  | 'connect4_pvp'
  | 'pong_solo'
  | 'pong_duel'
  | 'tron_solo'
  | 'tron_duel'
  | 'battleship_solo'
  | 'battleship_pvp'
  | 'domino_solo'
  | 'domino_pvp'
  | 'cyber_master'
  | 'gold_champion'
  | 'silver_ace'
  | 'bronze'
  | 'close';

interface IconProps extends LucideProps {
  name: IconName | string;
  size?: number | string;
}

const ICON_MAP: Record<string, any> = {
  snake_solo: Activity,
  snake_duel: Swords,
  ttt_solo: Circle,
  ttt_pvp: X,
  connect4_solo: CircleDot,
  connect4_pvp: Columns3,
  pong_solo: SlidersHorizontal,
  pong_duel: Zap,
  tron_solo: Cpu,
  tron_duel: Sparkles,
  battleship_solo: Anchor,
  battleship_pvp: Crosshair,
  domino_solo: Grid2X2,
  domino_pvp: Dices,
  cyber_master: Diamond,
  gold_champion: Trophy,
  silver_ace: Medal,
  bronze: Award,
  close: X
};

export function Icon({ name, size = 18, strokeWidth = 2, ...props }: IconProps) {
  const Component = ICON_MAP[name] || Activity;
  return <Component size={size} strokeWidth={strokeWidth} {...props} />;
}
