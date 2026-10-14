#pragma once

#include <vector>
#include <string>

namespace gamehub {
namespace core {

/**
 * @brief Joueur au Katro.
 */
enum class KatroPlayer {
    SOUTH = 1,  ///< Joueur Sud (rangées 0 et 1, P1)
    NORTH = 2   ///< Joueur Nord (rangées 2 et 3, P2 / Bot)
};

/**
 * @brief Type de rangée dans le camp d'un joueur.
 */
enum class KatroRowType {
    OUTER,  ///< Rangée extérieure (bord du plateau)
    INNER   ///< Rangée intérieure (face à l'adversaire)
};

/**
 * @brief Sens de circulation dans le circuit fermé du joueur.
 */
enum class KatroDirection {
    CLOCKWISE,          ///< Sens des aiguilles d'une montre (horaire)
    COUNTER_CLOCKWISE   ///< Sens inverse des aiguilles d'une montre (antihoraire)
};

/**
 * @brief Événement d'animation pour les semis et captures.
 */
struct KatroAnimEvent {
    std::string type;  ///< "pickup", "drop", "capture"
    int from{-1};      ///< Case source (-1 si non applicable)
    int to{-1};        ///< Case destination (-1 si non applicable)
    int count{0};      ///< Nombre de graines concernées
};

static constexpr int KATRO_DEFAULT_ROWS  = 4;  ///< 4 rangées (2 Sud, 2 Nord)
static constexpr int KATRO_DEFAULT_COLS  = 4;  ///< 4 colonnes par défaut (4×4)
static constexpr int KATRO_DEFAULT_SEEDS = 2;  ///< 2 graines par case au départ

/**
 * @brief Domain entity encapsulating an individual pit (case) of the Katro board.
 */
class KatroPit {
public:
    KatroPit() = default;
    KatroPit(int index, int seeds, KatroPlayer owner, KatroRowType rowType)
        : m_index(index), m_seeds(seeds), m_owner(owner), m_rowType(rowType) {}

    int getIndex() const { return m_index; }
    int getSeeds() const { return m_seeds; }
    void setSeeds(int s) { m_seeds = s; }
    void addSeeds(int s) { m_seeds += s; }
    int takeAllSeeds() {
        int s = m_seeds;
        m_seeds = 0;
        return s;
    }

    bool isEmpty() const { return m_seeds <= 0; }
    KatroPlayer getOwner() const { return m_owner; }
    KatroRowType getRowType() const { return m_rowType; }

private:
    int m_index{0};
    int m_seeds{0};
    KatroPlayer m_owner{KatroPlayer::SOUTH};
    KatroRowType m_rowType{KatroRowType::OUTER};
};

/**
 * @brief Domain entity representing the 4-row Katro board, managing geometry,
 * closed player circuits, and seed distributions.
 */
class KatroBoard {
public:
    KatroBoard(int rows = KATRO_DEFAULT_ROWS, int cols = KATRO_DEFAULT_COLS, int seedsPerCell = KATRO_DEFAULT_SEEDS)
        : m_rows(rows), m_cols(cols), m_totalCells(rows * cols), m_seedsPerCell(seedsPerCell) {
        initPits();
    }

    void reset() {
        initPits();
    }

    int getRows() const { return m_rows; }
    int getCols() const { return m_cols; }
    int getTotalCells() const { return m_totalCells; }
    int getInitialSeeds() const { return m_seedsPerCell; }

    int getRow(int cellIndex) const { return cellIndex / m_cols; }
    int getCol(int cellIndex) const { return cellIndex % m_cols; }
    int getCellIndex(int row, int col) const { return row * m_cols + col; }

    KatroPit& getPit(int index) { return m_pits[index]; }
    const KatroPit& getPit(int index) const { return m_pits[index]; }

    int getSeedCount(int cellIndex) const {
        if (cellIndex < 0 || cellIndex >= m_totalCells) return -1;
        return m_pits[cellIndex].getSeeds();
    }

    void setSeedCount(int cellIndex, int seeds) {
        if (cellIndex >= 0 && cellIndex < m_totalCells) {
            m_pits[cellIndex].setSeeds(seeds);
        }
    }

    void setBoard(const std::vector<int>& board) {
        if (static_cast<int>(board.size()) == m_totalCells) {
            for (int i = 0; i < m_totalCells; ++i) {
                m_pits[i].setSeeds(board[i]);
            }
        }
    }

    std::vector<int> toSeedVector() const {
        std::vector<int> vec(m_totalCells);
        for (int i = 0; i < m_totalCells; ++i) {
            vec[i] = m_pits[i].getSeeds();
        }
        return vec;
    }

    bool isPlayerCell(int cellIndex, KatroPlayer player) const {
        if (cellIndex < 0 || cellIndex >= m_totalCells) return false;
        if (player == KatroPlayer::SOUTH) {
            return cellIndex < 2 * m_cols;
        } else {
            return cellIndex >= 2 * m_cols && cellIndex < m_totalCells;
        }
    }

    bool isInnerRow(int cellIndex, KatroPlayer player) const {
        if (cellIndex < 0 || cellIndex >= m_totalCells) return false;
        if (player == KatroPlayer::SOUTH) {
            return cellIndex >= m_cols && cellIndex < 2 * m_cols;  // Rangée 1
        } else {
            return cellIndex >= 2 * m_cols && cellIndex < 3 * m_cols;  // Rangée 2
        }
    }

    bool isOuterRow(int cellIndex, KatroPlayer player) const {
        if (cellIndex < 0 || cellIndex >= m_totalCells) return false;
        if (player == KatroPlayer::SOUTH) {
            return cellIndex < m_cols;  // Rangée 0
        } else {
            return cellIndex >= 3 * m_cols && cellIndex < m_totalCells;  // Rangée 3
        }
    }

    int nextCell(int cellIndex, KatroPlayer player, KatroDirection dir) const {
        int row = cellIndex / m_cols;
        int col = cellIndex % m_cols;

        if (player == KatroPlayer::SOUTH) {
            if (dir == KatroDirection::CLOCKWISE) {
                if (row == 0) {
                    if (col > 0) return cellIndex - 1;
                    else         return m_cols;  // (1, 0)
                } else { // row == 1
                    if (col < m_cols - 1) return cellIndex + 1;
                    else                  return m_cols - 1;  // (0, C-1)
                }
            } else { // COUNTER_CLOCKWISE
                if (row == 0) {
                    if (col < m_cols - 1) return cellIndex + 1;
                    else                  return 2 * m_cols - 1;  // (1, C-1)
                } else { // row == 1
                    if (col > 0) return cellIndex - 1;
                    else         return 0;  // (0, 0)
                }
            }
        } else { // KatroPlayer::NORTH
            if (dir == KatroDirection::CLOCKWISE) {
                if (row == 2) {
                    if (col > 0) return cellIndex - 1;
                    else         return 3 * m_cols;  // (3, 0)
                } else { // row == 3
                    if (col < m_cols - 1) return cellIndex + 1;
                    else                  return 3 * m_cols - 1;  // (2, C-1)
                }
            } else { // COUNTER_CLOCKWISE
                if (row == 2) {
                    if (col < m_cols - 1) return cellIndex + 1;
                    else                  return 4 * m_cols - 1;  // (3, C-1)
                } else { // row == 3
                    if (col > 0) return cellIndex - 1;
                    else         return 2 * m_cols;  // (2, 0)
                }
            }
        }
    }

    int getOppositeInner(int cellIndex, KatroPlayer player) const {
        if (!isInnerRow(cellIndex, player)) return -1;
        int col = cellIndex % m_cols;
        if (player == KatroPlayer::SOUTH) {
            return 2 * m_cols + col;  // Rangée 2, colonne col
        } else {
            return 1 * m_cols + col;  // Rangée 1, colonne col
        }
    }

    int getOppositeOuter(int cellIndex, KatroPlayer player) const {
        if (!isInnerRow(cellIndex, player)) return -1;
        int col = cellIndex % m_cols;
        if (player == KatroPlayer::SOUTH) {
            return 3 * m_cols + col;  // Rangée 3, colonne col
        } else {
            return 0 * m_cols + col;  // Rangée 0, colonne col
        }
    }

    bool isOpponentInnerRowEmpty(KatroPlayer player) const {
        int start = (player == KatroPlayer::SOUTH) ? (2 * m_cols) : (1 * m_cols);
        int end   = start + m_cols;
        for (int i = start; i < end; ++i) {
            if (m_pits[i].getSeeds() > 0) return false;
        }
        return true;
    }

    int countPlayerSeeds(KatroPlayer player) const {
        int start = (player == KatroPlayer::SOUTH) ? 0 : (2 * m_cols);
        int end   = start + (2 * m_cols);
        int sum   = 0;
        for (int i = start; i < end; ++i) {
            sum += m_pits[i].getSeeds();
        }
        return sum;
    }

    int getTotalSeeds() const {
        int sum = 0;
        for (const auto& pit : m_pits) {
            sum += pit.getSeeds();
        }
        return sum;
    }

    bool canPlayerMove(KatroPlayer player) const {
        int start = (player == KatroPlayer::SOUTH) ? 0 : (2 * m_cols);
        int end   = start + (2 * m_cols);
        for (int i = start; i < end; ++i) {
            if (m_pits[i].getSeeds() > 0) return true;
        }
        return false;
    }

private:
    void initPits() {
        m_pits.clear();
        m_pits.reserve(m_totalCells);
        for (int i = 0; i < m_totalCells; ++i) {
            int row = i / m_cols;
            KatroPlayer owner = (row < 2) ? KatroPlayer::SOUTH : KatroPlayer::NORTH;
            KatroRowType rowType = (row == 0 || row == 3) ? KatroRowType::OUTER : KatroRowType::INNER;
            m_pits.emplace_back(i, m_seedsPerCell, owner, rowType);
        }
    }

    int m_rows{4};
    int m_cols{4};
    int m_totalCells{16};
    int m_seedsPerCell{2};
    std::vector<KatroPit> m_pits;
};

} // namespace core
} // namespace gamehub
