# Spec: Neo-Arcade Visual Engine and C++ OOP Capstone Overhaul

Status: ready-for-agent

## Problem Statement

Gamebox currently possesses an authoritative headless C++ game engine running across desktop and ESP32-S3 microcontroller targets with a portfolio of 7 games. However, players and academic evaluators encounter critical deficiencies across visual presentation, mobile ergonomics, social party flow, and software design rigor:

1. **Sterile Visual Experience**: Games render as flat, static geometric shapes on an untextured dark canvas. High-DPI mobile screens suffer from pixel blurriness, and players receive zero kinetic or visual feedback upon disc drops, ball impacts, or victories.
2. **Mobile Control Breakage & Friction**: The virtual direction pad suffers from an inverted layout bug where the down button sits in the center row. Touch targets are too small for fast twitch games, and players lack natural touchscreen gestures (direct paddle dragging in Pong, swiping in Tron and Snake).
3. **Multiplayer Party Flow Rupture**: Finishing a competitive match forcefully evicts both players back to the public lobby. Players cannot immediately rematch each other in the same room, killing table-side momentum.
4. **Information Security Leak**: The Dominoes engine broadcasts all participants' secret hand tiles in plain text to every connected client, allowing players to peek at opponents' hands via browser inspection tools.
5. **Academic OOP Strategy Coupling & Embedded Risks**: Bot algorithms are hardcoded inside concrete game classes instead of being polymorphically injected via strategy interfaces. Grid representations incur unnecessary heap allocations on the microcontroller, and string parsing relies on C++ exceptions prohibited on embedded platforms.

## Solution

A comprehensive, dual-layer upgrade uniting client-side procedural visual excellence with academic-grade C++ object-oriented engineering:

1. **Client-Side Neo-Arcade Visual Engine**: A 60 FPS client presentation layer executing high-DPI clamped Retina scaling, a universal multi-emitter particle engine (sparks, water ripples, fire, smoke, victory confetti), a decaying camera screen-shake matrix, and game-specific procedural atmospheric styling (Havana green felt domino table, CRT scanline toggle, mechanical Connect Four disc gravity with elastic bounce, Tron neon bloom trails, Pong phosphor comet trails, Battleship sonar radar sweep, and bioluminescent Snake viper eyes).
2. **Mobile Touch Ergonomics & Glass Shell**: A corrected directional pad with 60px touch targets, direct canvas vertical paddle dragging for Pong, canvas swipe gestures for Tron and Snake, and an animated frosted-glass mobile bottom-sheet Hall of Fame drawer with metallic achievement tiers.
3. **Authoritative Instant Rematch & Rivalry Series**: An in-room rematch protocol enabling players to reset game state seamlessly without returning to the lobby, accompanied by persistent match series score tracking.
4. **Authoritative Hand Fog-of-War**: Per-player network state serialization for Dominoes that masks opponent hands down to tile counts only.
5. **Polymorphic Strategy Decoupling & Embedded Discipline**: Generic bot strategy interfaces decoupled from concrete games, static zero-heap grid allocations, and exception-free string parsing via modern C++ standards.

## User Stories

1. As a mobile player on a high-DPI smartphone, I want the game canvas to render at clamped 2x Retina resolution, so that all grid lines, tokens, and text appear razor-sharp on my screen.
2. As a player in an intense Pong match, I want the ball to leave a fading phosphor comet trail and emit sparks when hitting paddles, so that high-speed volleys feel thrilling and kinetic.
3. As a mobile player controlling Pong, I want to drag my finger vertically on the canvas to move my paddle, so that I can control my paddle with natural 1:1 tactile precision.
4. As a player in Connect Four, I want dropped discs to fall with simulated gravity and an elastic settling bounce, so that placing chips feels physically satisfying.
5. As a mobile player in Connect Four, I want a luminous column targeting beam and drop indicator when touching columns, so that I never misdrop a token due to small touch targets.
6. As a Tron player, I want light cycles to leave luminous glowing neon trails with a cockpit headlight cone, so that the arena feels like an authentic retro synthwave duel.
7. As a mobile player in Tron, I want to swipe in any cardinal direction on the canvas to steer my bike, so that I do not have to look down at virtual buttons during split-second turns.
8. As a mobile player using touch controls, I want the virtual direction pad down button to sit properly below the left and right buttons, so that pressing down is natural and reliable.
9. As a mobile player using touch controls, I want directional pad buttons to measure at least 60px with active press feedback, so that my thumbs do not miss inputs during fast-paced play.
10. As a Battleship player, I want a rotating green sonar radar sweep across the targeting grid, so that the board evokes the atmosphere of a tactical naval war room.
11. As a Battleship player, I want fired torpedoes to trigger expanding water ripples on misses and fire/smoke explosions on hits, so that attack outcomes are visually dramatic.
12. As a Battleship player, I want a bold alert banner and board highlight when an enemy ship is completely destroyed, so that I am clearly notified of strategic milestones.
13. As a Dominoes player, I want the table to render as an emerald green billiard felt surface with warm border trim, so that the game feels like a classic parlor social club.
14. As a Dominoes player, I want dominoes to render as 3D ivory tiles with beveled edge drop shadows, so that pieces look and feel like physical game pieces.
15. As a Dominoes player, I want playable tiles in my hand and matching open ends on the table to pulse with soft glowing halos, so that legal moves are immediately identifiable.
16. As a Dominoes player, I want my opponent's tiles to be hidden from the network payload, so that tech-savvy opponents cannot inspect browser network logs to cheat.
17. As a Snake player, I want my snake head to feature glowing directional robotic eyes, so that I can immediately discern my current heading.
18. As a Snake player, I want the food pellet to pulse with a glowing core and drifting radioactive embers, so that objectives stand out brightly on the grid.
19. As a Tic-Tac-Toe player, I want marks to draw dynamically with laser sparks and winning lines to trigger an overdrive beam, so that even simple games feel exciting.
20. As a winning player in any of the 7 games, I want an animated shower of fluttering confetti particles to erupt across the screen, so that victories are celebrated with visual fanfare.
21. As a player experiencing major impacts, I want the screen to shake with decaying kinetic impulses, so that collisions convey physical weight.
22. As a player sensitive to motion, I want screen shakes to respect my system reduced-motion accessibility preference, so that I can play comfortably without nausea.
23. As a retro arcade fan, I want to toggle a simulated CRT scanline and curved cabinet vignette filter, so that I can enjoy an authentic 1980s coin-op aesthetic.
24. As two rival players finishing a close multiplayer match, I want an instant rematch button in the game over dialog, so that we can immediately restart the match inside the same room.
25. As two rival players playing repeated rematches, I want the room to track our overall series score, so that we can determine the winner of a Best-of-3 or Best-of-5 contest.
26. As a player opening the Hall of Fame, I want an animated bottom-sheet drawer displaying my total wins, losses, win rates, and metallic tier badges, so that I feel proud of my achievements.
27. As a player viewing my records, I want a two-step confirmation before resetting statistics, so that I do not accidentally wipe my achievements with a stray tap.
28. As a lobby browser, I want active multiplayer rooms to display a pulsing live radar beacon, so that I immediately notice when someone is waiting for an opponent.
29. As an academic CS professor evaluating the codebase, I want bot algorithms across all games to implement an abstract strategy pattern, so that artificial intelligence strategies are decoupled from game rules.
30. As an embedded firmware engineer, I want the Tron grid to use static contiguous memory rather than nested vectors, so that SRAM heap fragmentation is eliminated on the microcontroller.
31. As an embedded firmware engineer, I want string-to-number parsing to use non-allocating, non-throwing standard conversions, so that firmware runs safely in exception-free environments.
32. As a firmware maintainer, I want all embedded web asset string chunks to stay safely beneath compiler literal limits, so that desktop and cross-compilation builds succeed without truncation.

## Implementation Decisions

### 1. Client-Side Rendering Architecture
- High-DPI canvas buffer will calculate scale factors clamped to a maximum of 2x, mapping all mouse and touch events back to logical coordinates.
- Universal particle manager will operate as an object pool supporting multiple emitter profiles: confetti ribbons, neon collision sparks, expanding circular water shockwaves, and smoke curls.
- A camera impulse controller will translate canvas rendering coordinates by a decaying offset vector mapped to three impact tiers (Subtle 2px, Kinetic 5px, Cataclysm 9px), bypassing updates when motion reduction is requested.
- Client state ingestion will implement linear interpolation across incoming 15 Hz snapshots inside the browser animation frame loop to produce 60 FPS motion fluidity.
- CRT scanline raster and curved vignette effects will be applied via an overlaid procedural CSS/canvas layer controlled by an arcade header toggle.

### 2. Game-Specific Visual Presentations
- Connect Four will track previous versus incoming board state to trigger procedural vertical falling animations with elastic squash-and-stretch easing on disc arrival.
- Tron light cycles will render white laser core lines with high-radius neon bloom shadows, forward headlight projection cones, and circular collision shockwave explosions.
- Pong will record recent ball positions to render a fading phosphor comet trail that shifts color based on velocity, accompanied by paddle contact sparks.
- Battleship will render an animated continuous 360-degree sonar sweep line, concentric water ripples on misses, and fire/smoke bursts on hits.
- Dominoes will render an emerald green billiard felt radial background with warm wooden borders, beveled 3D ivory tiles with downward drop shadows, and pulsing cyan edge halos on legal matching ends.
- Snake will draw rounded hexagonal heads with directional dual-pip eyes, a glowing radiant apple core with drifting embers, and neon spinal segment outlines.
- Tic-Tac-Toe will render animated drawn stroke paths and a laser strike beam spanning winning triplets.

### 3. Mobile Ergonomics & Shell Interface
- Virtual directional pad CSS grid will reposition the down button to the third row and increase button tap dimensions to 60px with active press scaling.
- Direct pointer event listeners on the canvas will enable 1:1 vertical paddle drag tracking for Pong and cardinal swipe detection for Tron and Snake.
- Hall of Fame drawer will adopt a mobile bottom-sheet sliding presentation with top grab handle, displaying game-by-game wins, losses, win percentages, metallic rank tiers (Bronze, Silver, Gold, Master), and two-step reset confirmation.
- Room directory entries will feature an animated glowing green radar beacon indicating live open sessions.

### 4. Authoritative Domain & Rematch Protocol
- Host domain engine will accept a rematch command during finished game states. When all active room participants submit rematch requests, the room will reinitialize its game model, advance the series match counter, and transition back to active state without disconnecting sessions.
- Dominoes game model will implement player-specific state serialization that masks other players' private tiles, only transmitting tile counts and public board chains.

### 5. C++ Object-Oriented Architecture & Embedded Hygiene
- Abstract bot strategy interfaces will be introduced across all game types, allowing artificial intelligence implementations (Minimax, Voronoi flood-fill, Kinematic trajectory prediction, and Parity target queue) to be injected polymorphically into games.
- Tron grid state will be refactored from nested vectors into a single flat static array representing cells.
- Parsing routines currently using string-to-integer conversion wrapped in exception handling will be replaced with standard non-throwing character conversion utilities.
- Web asset string chunking will be partitioned into distinct functional modules, strictly ensuring no single chunk exceeds 12 KB to prevent compiler string literal limits from triggering.

## Testing Decisions

### What Makes a Good Test
Tests must verify external behavioral contracts and observable state transitions rather than private implementation details. Unit and integration tests must run deterministically without real network sockets or hardware dependencies.

### Modules Tested
1. **Host Domain Rematch Lifecycle**: Verify that submitting rematch commands from all room participants cleanly reinitializes the game state, updates series win counters, and transitions room lifecycle states without unbinding sessions.
2. **Authoritative Dominoes Hand Masking**: Verify that player-specific serialization strictly conceals opponent hand identities while preserving accurate tile counts and public chain structures.
3. **Bot Strategy Polymorphism**: Verify that detached strategy implementations return legal, optimal moves when given board snapshots across Connect Four, Tron, Pong, and Dominoes.
4. **Non-Throwing String Conversion**: Verify that malformed or out-of-range client payloads are handled gracefully without exceptions or crashes.
5. **Static Grid Memory Integrity**: Verify that flat array indexing in Tron behaves identically to two-dimensional indexing for boundary checks and collision detection.

### Prior Art
- `tests/test_host_engine.cpp`: Prior art for testing authoritative session handling, room lifecycle transitions, and player-specific state serialization using mock network adapters.
- `tests/test_new_games.cpp`: Prior art for testing game mechanics, bot move computations, and win condition evaluations.

## Out of Scope
- Native hardware graphics drivers (e.g. driving SPI/I2C TFT displays directly on the ESP32 chip). All visual rendering remains strictly client-side within the browser.
- Audio sample file storage (MP3/WAV). All sound synthesis remains strictly procedural via the Web Audio API.
- Global internet matchmaking or wide-area network routing. The system remains strictly optimized for local SoftAP captive portals and local networks.
- User accounts, cloud databases, or external authentication providers.

## Further Notes
- All visual upgrades run entirely on client devices and impose 0% CPU and 0 bytes RAM overhead on the ESP32-S3 microcontroller.
- This specification corresponds to Architectural Decision Record ADR 0013.
