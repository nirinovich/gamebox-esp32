import { FunctionalComponent } from 'preact';

export interface GameboxLogoProps {
  variant?: 'full' | 'glyph' | 'box';
  size?: number | string;
  theme?: 'dark' | 'light';
  className?: string;
  animated?: boolean;
  colorOverrides?: {
    boxColor?: string;
    redColor?: string;
    contrastColor?: string;
  };
}

/**
 * Authoritative Gamebox Vector Logo Component.
 * Faithfully renders the isometric launchbox and red gamepad rocket marks.
 * Palette: #14151b (Obsidian), #ef3938 (Gamebox Red), #ffffff (Stark White).
 */
export const GameboxLogo: FunctionalComponent<GameboxLogoProps> = ({
  variant = 'full',
  size = 40,
  theme = 'dark',
  className = '',
  animated = false,
  colorOverrides
}) => {
  const pixelSize = typeof size === 'number' ? `${size}px` : size;
  const boxColor = colorOverrides?.boxColor || (theme === 'dark' ? '#ffffff' : '#14151b');
  const redColor = colorOverrides?.redColor || '#ef3938';
  const controlMarkColor = colorOverrides?.contrastColor || (theme === 'dark' ? '#14151b' : '#ffffff');

  // Pure SVG Glyph: Gamepad Rocket
  if (variant === 'glyph') {
    return (
      <svg
        viewBox="0 0 100 100"
        width={pixelSize}
        height={pixelSize}
        class={className}
        style={{ display: 'inline-block', verticalAlign: 'middle', overflow: 'visible' }}
        role="img"
        aria-label="Gamebox Controller Mark"
      >
        <g class={animated ? 'gb-rocket-pulse' : undefined}>
          {/* Gamepad Rocket Silhouette */}
          <path
            d="M 32 30 C 38 27 44 29 50 29 C 56 29 62 27 68 30 C 77 34 81 44 79 56 C 78 63 72 67 67 64 C 62 61 58 52 50 52 C 42 52 38 61 33 64 C 28 67 22 63 21 56 C 19 44 23 34 32 30 Z"
            fill={redColor}
          />
          {/* Left Joystick Ring */}
          <circle cx="36" cy="44" r="5" fill="none" stroke={controlMarkColor} strokeWidth="3" />
          {/* Right Action Diamond Buttons */}
          <circle cx="64" cy="40" r="2" fill={controlMarkColor} />
          <circle cx="60" cy="44" r="2" fill={controlMarkColor} />
          <circle cx="68" cy="44" r="2" fill={controlMarkColor} />
          <circle cx="64" cy="48" r="2" fill={controlMarkColor} />
          {/* Exhaust Thrust Bars */}
          <rect x="42.5" y="60" width="3.2" height="8" rx="1.6" fill={redColor} />
          <rect x="48.4" y="60" width="3.2" height="15" rx="1.6" fill={redColor} />
          <rect x="54.3" y="60" width="3.2" height="8" rx="1.6" fill={redColor} />
        </g>
      </svg>
    );
  }

  // Pure SVG Box Variant: Geometric Launchbox Only
  if (variant === 'box') {
    return (
      <svg
        viewBox="0 0 120 120"
        width={pixelSize}
        height={pixelSize}
        class={className}
        style={{ display: 'inline-block', verticalAlign: 'middle', overflow: 'visible' }}
        role="img"
        aria-label="Gamebox Launchbox Mark"
      >
        <g
          fill="none"
          stroke={boxColor}
          strokeWidth="4"
          strokeLinecap="round"
          strokeLinejoin="round"
        >
          {/* Back-left open flap */}
          <polyline points="33,48 24,42 38,34 50,42" />
          {/* Back-right open flap */}
          <polyline points="87,48 96,42 82,34 70,42" />

          {/* Lower cube front-left face */}
          <polygon points="60,67 33,52 33,83 60,98" />
          {/* Lower cube front-right face */}
          <polygon points="60,67 87,52 87,83 60,98" />

          {/* Center vertical corner dividing front faces */}
          <line x1="60" y1="67" x2="60" y2="98" />

          {/* Front-left open flap */}
          <polygon points="60,67 33,52 24,60 51,75" />
          {/* Front-right open flap */}
          <polygon points="60,67 87,52 96,60 69,75" />
        </g>
      </svg>
    );
  }

  // Pure SVG Full Variant: Launchbox + Rocket
  return (
    <svg
      viewBox="0 0 120 120"
      width={pixelSize}
      height={pixelSize}
      class={className}
      style={{ display: 'inline-block', verticalAlign: 'middle', overflow: 'visible' }}
      role="img"
      aria-label="Gamebox Brand Logo"
    >
      {/* Launching Gamepad Rocket */}
      <g class={animated ? 'gb-rocket-launch' : undefined}>
        {/* Gamepad Body */}
        <path
          d="M 42 22 C 48 19 54 21 60 21 C 66 21 72 19 78 22 C 87 26 91 36 89 48 C 88 55 82 59 77 56 C 72 53 68 44 60 44 C 52 44 48 53 43 56 C 38 59 32 55 31 48 C 29 36 33 26 42 22 Z"
          fill={redColor}
        />
        {/* Left Joystick Ring */}
        <circle cx="46" cy="35" r="5.2" fill="none" stroke={controlMarkColor} strokeWidth="3.2" />
        {/* Right Action Buttons */}
        <circle cx="74" cy="31" r="2" fill={controlMarkColor} />
        <circle cx="70" cy="35" r="2" fill={controlMarkColor} />
        <circle cx="78" cy="35" r="2" fill={controlMarkColor} />
        <circle cx="74" cy="39" r="2" fill={controlMarkColor} />
        {/* 3 Rocket Thrust Exhaust Trails */}
        <rect x="52.5" y="50" width="3.5" height="9" rx="1.75" fill={redColor} />
        <rect x="58.25" y="50" width="3.5" height="17" rx="1.75" fill={redColor} />
        <rect x="64" y="50" width="3.5" height="9" rx="1.75" fill={redColor} />
      </g>

      {/* Isometric Open Launchbox */}
      <g
        fill="none"
        stroke={boxColor}
        strokeWidth="4"
        strokeLinecap="round"
        strokeLinejoin="round"
      >
        {/* Back-left open flap */}
        <polyline points="33,48 24,42 38,34 50,42" />
        {/* Back-right open flap */}
        <polyline points="87,48 96,42 82,34 70,42" />

        {/* Lower cube front-left face */}
        <polygon points="60,67 33,52 33,83 60,98" />
        {/* Lower cube front-right face */}
        <polygon points="60,67 87,52 87,83 60,98" />

        {/* Center vertical corner dividing front faces */}
        <line x1="60" y1="67" x2="60" y2="98" />

        {/* Front-left open flap */}
        <polygon points="60,67 33,52 24,60 51,75" />
        {/* Front-right open flap */}
        <polygon points="60,67 87,52 96,60 69,75" />
      </g>
    </svg>
  );
};
