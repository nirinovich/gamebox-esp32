# Gamebox

An authoritative headless game engine running on an ESP32-S3 that hosts multiplayer and solo games over a local Wi-Fi captive portal.

## Language

**Host**:
The physical ESP32-S3 microcontroller running the authoritative C++ game engine, HTTP server, and WebSocket transport.
_Avoid_: Board, device, hardware

**Client**:
A connected mobile or desktop web browser rendering game state received via WebSocket and streaming player inputs back to the Host.
_Avoid_: User, player, browser

**Session**:
An active, stateful network connection associated with a single connected Client.
_Avoid_: Connection, socket

**Player**:
A participant entity inside a Game, controlled either by a Session or by an AI agent.
_Avoid_: User, account

**Room**:
An isolated execution environment that pairs one or more Sessions to a single Game instance.
_Avoid_: Lobby, channel, match

**Game**:
A polymorphic stateful domain model encapsulating rules, entities, input handling, and state replication for a specific game mode.
_Avoid_: Engine, minigame, match

**Tick**:
A single fixed-interval execution step of the real-time game loop on the Host.
_Avoid_: Frame, cycle, step

**RoomCode**:
A unique 4-character alphanumeric identifier used by Host and Clients to address a specific Room.
_Avoid_: Pin, key, password

**RoomDirectory**:
The real-time registry of active and joinable Rooms broadcast from the Host to all connected Clients in the Lobby.
_Avoid_: RoomList, ServerBrowser, MatchFinder

**PlayerController**:
An abstract strategy interface providing decision-making or input ingestion for a Player.
_Avoid_: InputHandler, Agent, Driver

**HumanController**:
A PlayerController implementation that consumes inputs queued from a connected Client Session.
_Avoid_: RemotePlayer, NetworkUser

**BotController**:
A PlayerController implementation that autonomously calculates moves on the Host using game-specific heuristics or algorithms.
_Avoid_: AI, CPU, Computer

**RoomLifecycle**:
The lifecycle states of a Room: WAITING (open for Challengers), ACTIVE (authoritative game execution in progress), FINISHED (outcome resolved), and DISSOLVED (cleaned up and unmapped).
_Avoid_: Status, Phase, State

**AutoDetachment**:
The Host mechanism that automatically cleans up and unbinds a Session from any prior Room when the Session initiates, joins, or leaves a Game.
_Avoid_: ForceLeave, ResetConnection

**Leaderboard**:
The dual-layer record system tracking a Client PersonalBest in local storage and the HostRecord in Host memory.
_Avoid_: ScoreTable, HighScoreList

**MalagasyDominoGame**:
A polymorphic Game model implementing Malagasy block-style Dominoes (Tsipy Domino) for 2 to 4 Players with no drawing, turn-passing when blocked, and cumulative round scoring toward a TargetScore.
_Avoid_: DominoGame, DrawDominoes, TileMatch

**TargetScore**:
The cumulative point threshold configured by the Room creator (e.g. 60 or 100 points) required to win a multi-round match.
_Avoid_: PointGoal, WinLimit, MaxScore

**Deadwood**:
The undealt face-down domino tiles set aside in 2- or 3-player matches that cannot be drawn.
_Avoid_: Boneyard, Talon, DrawPile

**PipSum**:
The cumulative point value of dot markings on tiles remaining in a Player hand when the Game is blocked.
_Avoid_: HandValue, TileCount

**Rebon**:
The rollover jackpot point pool in MalagasyDominoGame accumulated when consecutive rounds end in tied blocks.
_Avoid_: Carryover, TieBonus, PotRoll

**TexasHoldemGame**:
A polymorphic Game model implementing No-Limit Texas Hold'em for 2 to 4 Players with private HoleCards, shared CommunityCards, pot accounting, and 7-card rank evaluation.
_Avoid_: PokerGame, CardTable

**HoleCards**:
The two private face-down cards dealt exclusively to a Player and strictly masked from other Sessions until showdown.
_Avoid_: Hand, PocketCards

**CommunityCards**:
The up to five face-up shared cards dealt on the board across Flop, Turn, and River betting rounds.
_Avoid_: BoardCards, TableCards

**Pot**:
The pool of chips wagered by Players across betting rounds awarded to the winning hand or split upon ties.
_Avoid_: Kitty, ChipsPool

**LudoGame**:
A polymorphic Game model implementing 2 to 4 Player cross-and-circle board mechanics featuring pawn collisions, yard rollout on 6, and safe zone navigation.
_Avoid_: Parchisi, Pachisi, TokenRace

**PawnYard**:
The designated corner holding area for a Player's tokens prior to being rolled out onto the active track.
_Avoid_: Base, Spawn, Nest

**SafeSquare**:
A designated track position (marked by a star) where tokens are immune to capture collisions.
_Avoid_: SafeZone, Refuge, StarCell

**VisualEngine**:
The client-side 60 FPS presentation layer in WebAssets.hpp that renders authoritative domain states via Canvas 2D without microcontroller overhead.
_Avoid_: Renderer, GraphicsCore

**JuiceEngine**:
The procedural sensory feedback coordinator managing particle emitters, camera shake matrices, and floating retro text popups.
_Avoid_: VFX, FXManager

**RetinaViewport**:
The high-DPI canvas buffer scaled by clamped devicePixelRatio to ensure sharp vector lines on mobile OLED displays.
_Avoid_: HiDPIScreen, ResolutionScaler

**SeriesScore**:
The persistent match win tally retained inside a Room across consecutive rematches.
_Avoid_: MatchCount, SetScore

**Rematch**:
The protocol transition allowing active players to reinitialize a Game inside the same Room without disconnecting or returning to the Lobby.
_Avoid_: PlayAgain, ResetRoom

**ClientShell**:
The client-side reactive interface orchestrating navigation, Lobby, RoomDirectory, SeriesScore overlays, and Player identity around the VisualEngine.
_Avoid_: UI, Frontend, WebApp

**GameplayCore**:
The zero-heap-allocation C++ value types, compile-time grids, and stream serialization utilities shared across all authoritative Host game implementations.
_Avoid_: EngineUtils, CommonLib, GameHelpers

**CanvasGridLayer**:
The client-side 2D presentation component managing cell dimensioning, coordinate mapping, neon line borders, and pointer hit-detection on the Canvas.
_Avoid_: BoardRenderer, GridComponent, ScreenGrid

**AssetPacker**:
The native host C++ utility in tools/asset_packer.cpp compressing the single-file ClientShell into an RFC 1952 Gzip stream embedded into WebAssets.hpp.
_Avoid_: AssetScript, PythonGenerator, WebBuilder

**CaptiveProbe**:
The automated HTTP request issued by a client operating system upon Wi-Fi association (e.g. `/generate_204`, `/hotspot-detect.html`) intercepted by the Host to trigger captive portal display.
_Avoid_: ProbePing, PortalCheck, HotspotTest

**FlashChunk**:
The contiguous byte array allocated in ESP32 flash memory (`PROGMEM`) representing the gzipped ClientShell payload streamed directly without heap duplication.
_Avoid_: MemoryBlock, BinaryBlob, RomAsset

**GameId**:
A canonical alphanumeric string identifier uniquely designating a registered Game implementation across the Host and ClientShell.
_Avoid_: GameType, GameEnum, GameTag

**GameRegistry**:
The decentralized Host lookup subsystem that maps a GameId to a dynamic game factory instance via static self-registration without centralized enums.
_Avoid_: GameFactory, FactoryTable, GameMap

**ActionStream**:
The authoritative collection of currently legal player actions streamed in Host state snapshots to dynamically template Client interactive controls.
_Avoid_: LegalMoves, ButtonList, ControlSpec

**InputTemplate**:
The declarative capability contract specifying a Game's required client input peripherals (e.g. DPAD, GRID_POINTER, DRAG_SLIDER) to configure the Client presentation layer.
_Avoid_: InputSchema, ControlConfig, LayoutSetting

**ActionModal**:
A transient prompt declaration streamed in an ActionStream to request a discrete branching choice (e.g. end placement or piece selection) rendered generically by the ClientShell.
_Avoid_: ChoiceDialog, PromptPopup, CustomModal

**ClientPlugin**:
A self-contained client presentation bundle under `games/<game>/client/` exporting a renderer and lifecycle bindings discovered at build time via Vite globbing.
_Avoid_: GameFrontend, RendererModule, ClientExtension
