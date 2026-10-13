#pragma once

// ============================================================================
// KatroGame.hpp — Moteur autoritaire du Katro tsotra malgache (Mancala-IV)
//
// Référence officielle :
//   Luc TIENNOT, "Les jeux de semailles à Madagascar",
//   Bulletin de l'APMEP / Publimath AVM14034, pages 74 à 76.
//
// Règles authentiques du Katro tsotra :
//   1. Plateau : 4 rangées et 4 colonnes par défaut (configurable).
//      - Sud contrôle les deux rangées du bas :
//          * Rangée 0 : Extérieure Sud
//          * Rangée 1 : Intérieure Sud (face au centre / Nord)
//      - Nord contrôle les deux rangées du haut :
//          * Rangée 2 : Intérieure Nord (face au centre / Sud)
//          * Rangée 3 : Extérieure Nord
//      - 2 graines par case au départ (32 graines totales en 4×4).
//
//   2. Circuits indépendants et fermés :
//      - Chaque joueur sème UNIQUEMENT dans ses deux rangées (circuit de 2×C cases).
//      - Deux sens de circulation : HORAIRE (CLOCKWISE, par défaut) ou ANTIHORAIRE.
//      - Le sens choisi reste constant pendant toute la durée du coup.
//
//   3. Semis et relais :
//      - Prise de toutes les graines d'une case non vide du camp du joueur actif.
//      - Distribution une à une dans les cases successives du circuit.
//      - Si le semis boucle, la case de départ peut être traversée et recevoir une graine.
//
//   4. Règle exacte de capture :
//      - À la fin de chaque semis élémentaire :
//        * Sur rangée extérieure : aucune capture possible. Si case occupée → relais ;
//          si case vide → fin de coup.
//        * Sur rangée intérieure :
//          - Si la rangée intérieure adverse n'est pas entièrement vide :
//            si la case intérieure adverse en face (même colonne) a des graines,
//            on capture TOUTES ses graines.
//          - Si TOUTE la rangée intérieure adverse est vide :
//            si la case extérieure adverse en face a des graines,
//            on capture TOUTES ses graines.
//          - Si capture effectuée : les graines capturées sont ajoutées à celles
//            de la case d'arrivée du joueur et constituent la nouvelle collection
//            pour relancer immédiatement un semis élémentaire depuis cette case.
//          - Si aucune capture possible (case adverse en face vide) :
//            règle normale de relais si la case d'arrivée était occupée avant dépôt,
//            ou fin du coup si elle était vide avant dépôt.
//
//   5. Conservation intégrale des graines :
//      - Les graines ne sortent JAMAIS du jeu ; elles changent simplement de camp.
//      - Total des graines strictement constant tout au long de la partie.
//
//   6. Fin de partie :
//      - Détectée dès qu'un joueur n'a plus de graines dans ses deux rangées.
// ============================================================================

#include <gamehub/core/Game.hpp>
#include <vector>
#include <string>
#include <cstdint>

namespace gamehub {
namespace core {

// ---------------------------------------------------------------------------
// Types et Énumérations
// ---------------------------------------------------------------------------

/** Joueur au Katro. */
enum class KatroPlayer {
    SOUTH = 1,  ///< Joueur Sud (rangées 0 et 1, P1)
    NORTH = 2   ///< Joueur Nord (rangées 2 et 3, P2 / Bot)
};

/** Type de rangée dans le camp d'un joueur. */
enum class KatroRowType {
    OUTER,  ///< Rangée extérieure (bord du plateau)
    INNER   ///< Rangée intérieure (face à l'adversaire)
};

/** Sens de circulation dans le circuit du joueur. */
enum class KatroDirection {
    CLOCKWISE,          ///< Sens des aiguilles d'une montre (horaire)
    COUNTER_CLOCKWISE   ///< Sens inverse des aiguilles d'une montre (antihoraire)
};

// ---------------------------------------------------------------------------
// Événements d'animation pour le déplacement et la capture des graines
// ---------------------------------------------------------------------------
struct KatroAnimEvent {
    std::string type;  ///< "pickup", "drop", "capture"
    int from{-1};      ///< Case source (-1 si non applicable)
    int to{-1};        ///< Case destination (-1 si non applicable)
    int count{0};      ///< Nombre de graines concernées
};

// ---------------------------------------------------------------------------
// Constantes par défaut
// ---------------------------------------------------------------------------
static constexpr int KATRO_DEFAULT_ROWS  = 4;  ///< 4 rangées (2 Sud, 2 Nord)
static constexpr int KATRO_DEFAULT_COLS  = 4;  ///< 4 colonnes par défaut (4×4)
static constexpr int KATRO_DEFAULT_SEEDS = 2;  ///< 2 graines par case au départ

// ---------------------------------------------------------------------------
// Classe principale KatroGame
// ---------------------------------------------------------------------------
class KatroGame : public Game {
public:
    /**
     * Constructeur.
     * @param isSolo true si joueur 1 vs Bot (solo), false si PvP.
     * @param cols Nombre de colonnes (4 par défaut, configurable à 6 ou 8).
     * @param seedsPerCell Nombre de graines initiales par case (2 par défaut).
     */
    explicit KatroGame(bool isSolo = true, int cols = KATRO_DEFAULT_COLS, int seedsPerCell = KATRO_DEFAULT_SEEDS);
    ~KatroGame() override = default;

    // -----------------------------------------------------------------------
    // Interface Game
    // -----------------------------------------------------------------------
    void init() override;
    void update(float dt) override;
    void handleInput(PlayerId id, const std::string& commandJson) override;
    std::string serializeState() const override;
    bool isFinished() const override { return m_finished; }
    int  getWinner()  const override { return m_winner;   }

    // -----------------------------------------------------------------------
    // Moteur de jeu Katro
    // -----------------------------------------------------------------------

    /**
     * Joue un coup complet à partir d'une case donnée, dans le sens indiqué.
     * Enchaîne les semis élémentaires, relais et captures jusqu'à l'arrêt.
     * @param cell Index de la case de départ (0 .. m_totalCells - 1).
     * @param player Joueur actif (1 = Sud, 2 = Nord).
     * @param dir Sens de circulation (horaire par défaut).
     * @return true si le coup est valide et a été joué, false sinon.
     */
    bool playTurn(int cell, int player, KatroDirection dir);
    bool playTurn(int cell, int player); // Utilise m_defaultDirection

    // -----------------------------------------------------------------------
    // Géométrie du plateau et identification des cases
    // -----------------------------------------------------------------------

    int getRows() const { return m_rows; }
    int getCols() const { return m_cols; }
    int getTotalCells() const { return m_totalCells; }
    int getInitialSeeds() const { return m_seedsPerCell; }

    int getRow(int cellIndex) const { return cellIndex / m_cols; }
    int getCol(int cellIndex) const { return cellIndex % m_cols; }
    int getCellIndex(int row, int col) const { return row * m_cols + col; }

    bool isPlayerCell(int cellIndex, KatroPlayer player) const;
    bool isPlayerCell(int cellIndex, int playerNum) const;

    bool isInnerRow(int cellIndex, KatroPlayer player) const;
    bool isInnerRow(int cellIndex, int playerNum) const;

    bool isOuterRow(int cellIndex, KatroPlayer player) const;

    /**
     * Retourne la case suivante dans le circuit fermé du joueur,
     * selon le sens de circulation choisi.
     */
    int nextCell(int cellIndex, KatroPlayer player, KatroDirection dir) const;

    /**
     * Retourne la case adverse située sur la rangée INTÉRIEURE dans la même colonne.
     * @return Index de la case, ou -1 si non applicable.
     */
    int getOppositeInner(int cellIndex, KatroPlayer player) const;

    /**
     * Retourne la case adverse située sur la rangée EXTÉRIEURE dans la même colonne.
     * @return Index de la case, ou -1 si non applicable.
     */
    int getOppositeOuter(int cellIndex, KatroPlayer player) const;

    /**
     * Vérifie si toute la rangée intérieure adverse est vide (0 graines).
     */
    bool isOpponentInnerRowEmpty(KatroPlayer player) const;

    // -----------------------------------------------------------------------
    // Accesseurs d'état et méthodes pour les tests
    // -----------------------------------------------------------------------

    int getSeedCount(int cellIndex) const;
    int countPlayerSeeds(KatroPlayer player) const;
    int countPlayerSeeds(int playerNum) const;
    int getTotalSeedsOnBoard() const;

    int getScore(int playerNum) const; // Retourne le nombre de graines du camp du joueur
    int getTurn() const { return m_turn; }

    void setBoard(const std::vector<int>& board);
    void setTurn(int turn) { m_turn = turn; }
    void setDefaultDirection(KatroDirection dir) { m_defaultDirection = dir; }
    KatroDirection getDefaultDirection() const { return m_defaultDirection; }

    void setEarlyStopThreshold(int threshold) { m_earlyStopThreshold = threshold; }
    int  getEarlyStopThreshold() const { return m_earlyStopThreshold; }

    bool canPlayerMove(KatroPlayer player) const;

    const std::vector<KatroAnimEvent>& getLastEvents() const { return m_lastEvents; }
    int getMoveSeq() const override { return m_moveSeq; }

    /**
     * Choisit un coup aléatoire parmi toutes les cases disponibles
     * (contenant des graines) dans le camp du bot (Nord).
     * @return Index de la case choisie, ou -1 si aucune case n'est disponible.
     */
    int computeBotMove() const;

private:
    void checkEndCondition();

    // Paramètres du plateau
    int m_rows;
    int m_cols;
    int m_totalCells;
    int m_seedsPerCell;
    bool m_isSolo;

    // État du jeu
    std::vector<int> m_board;
    int m_turn{1};           ///< 1 = Sud, 2 = Nord
    bool m_finished{false};
    int m_winner{0};         ///< 0 = en cours, 1 = Sud, 2 = Nord, -1 = égalité
    KatroDirection m_defaultDirection{KatroDirection::CLOCKWISE};
    int m_earlyStopThreshold{0}; ///< 0 = arrêt uniquement si 0 graines (par défaut)

    // Animation & événements
    std::vector<KatroAnimEvent> m_lastEvents;
    int m_moveSeq{0};

    float m_botTimer{0.0f};
    static constexpr float BOT_DELAY_SEC = 0.6f;
};

} // namespace core
} // namespace gamehub
