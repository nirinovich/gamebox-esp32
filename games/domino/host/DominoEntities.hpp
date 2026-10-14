#pragma once

#include <vector>
#include <deque>
#include <algorithm>
#include <random>
#include <string>
#include <utility>

namespace gamehub {
namespace core {

/**
 * @brief Value object representing a double-six domino tile.
 */
struct DominoTile {
    int left{0};
    int right{0};

    int pips() const { return left + right; }
    bool matches(int val) const { return left == val || right == val; }
    void flip() { std::swap(left, right); }
};

/**
 * @brief Domain entity encapsulating a player's private hand of domino tiles.
 */
class DominoHand {
public:
    DominoHand() = default;

    size_t size() const { return m_tiles.size(); }
    bool empty() const { return m_tiles.empty(); }

    const DominoTile& getTile(size_t index) const {
        return m_tiles.at(index);
    }

    void addTile(const DominoTile& tile) {
        m_tiles.push_back(tile);
    }

    DominoTile removeTile(size_t index) {
        if (index >= m_tiles.size()) return {};
        DominoTile tile = m_tiles[index];
        m_tiles.erase(m_tiles.begin() + index);
        return tile;
    }

    int calculateTotalPips() const {
        int total = 0;
        for (const auto& t : m_tiles) {
            total += t.pips();
        }
        return total;
    }

    bool canMatchAny(int leftEnd, int rightEnd) const {
        if (leftEnd == -1 && rightEnd == -1) {
            return !m_tiles.empty(); // Open lead on empty board
        }
        for (const auto& t : m_tiles) {
            if (t.matches(leftEnd) || t.matches(rightEnd)) {
                return true;
            }
        }
        return false;
    }

    const std::vector<DominoTile>& getTiles() const {
        return m_tiles;
    }

    void setTiles(const std::vector<DominoTile>& tiles) {
        m_tiles = tiles;
    }

    void clear() {
        m_tiles.clear();
    }

private:
    std::vector<DominoTile> m_tiles;
};

/**
 * @brief Domain entity managing the active train/chain of domino tiles on the board.
 */
class DominoBoard {
public:
    DominoBoard() = default;

    bool empty() const { return m_board.empty(); }
    size_t size() const { return m_board.size(); }

    int getLeftEnd() const {
        if (m_board.empty()) return -1;
        return m_board.front().left;
    }

    int getRightEnd() const {
        if (m_board.empty()) return -1;
        return m_board.back().right;
    }

    const std::deque<DominoTile>& getTiles() const {
        return m_board;
    }

    void setTiles(const std::deque<DominoTile>& tiles) {
        m_board = tiles;
    }

    void clear() {
        m_board.clear();
    }

    bool canPlaceOnLeft(const DominoTile& tile) const {
        if (m_board.empty()) return true;
        return tile.matches(getLeftEnd());
    }

    bool canPlaceOnRight(const DominoTile& tile) const {
        if (m_board.empty()) return true;
        return tile.matches(getRightEnd());
    }

    bool placeTile(DominoTile tile, const std::string& endChoice) {
        if (m_board.empty()) {
            m_board.push_back(tile);
            return true;
        }

        int lEnd = getLeftEnd();
        int rEnd = getRightEnd();

        bool canL = tile.matches(lEnd);
        bool canR = tile.matches(rEnd);

        if (!canL && !canR) return false;

        std::string chosen = endChoice;
        if (canL && !canR) chosen = "left";
        if (canR && !canL) chosen = "right";

        if (chosen == "left" && canL) {
            if (tile.right != lEnd) {
                tile.flip();
            }
            m_board.push_front(tile);
            return true;
        } else if (chosen == "right" && canR) {
            if (tile.left != rEnd) {
                tile.flip();
            }
            m_board.push_back(tile);
            return true;
        }

        return false;
    }

private:
    std::deque<DominoTile> m_board;
};

/**
 * @brief Domain entity encapsulating the 28-tile double-six domino set and deadwood.
 */
class DominoTileSet {
public:
    static std::vector<DominoTile> createDoubleSixSet() {
        std::vector<DominoTile> set;
        set.reserve(28);
        for (int i = 0; i <= 6; ++i) {
            for (int j = i; j <= 6; ++j) {
                set.push_back(DominoTile{i, j});
            }
        }
        return set;
    }

    void shuffleAndDeal(int numPlayers, std::vector<DominoHand>& hands, std::mt19937& rng) {
        auto deck = createDoubleSixSet();
        std::shuffle(deck.begin(), deck.end(), rng);

        for (auto& hand : hands) {
            hand.clear();
        }
        m_deadwood.clear();

        size_t idx = 0;
        for (int i = 0; i < 7; ++i) {
            for (int p = 0; p < numPlayers; ++p) {
                if (idx < deck.size()) {
                    hands[p].addTile(deck[idx++]);
                }
            }
        }

        while (idx < deck.size()) {
            m_deadwood.push_back(deck[idx++]);
        }
    }

    size_t getDeadwoodCount() const { return m_deadwood.size(); }
    const std::vector<DominoTile>& getDeadwood() const { return m_deadwood; }

private:
    std::vector<DominoTile> m_deadwood;
};

} // namespace core
} // namespace gamehub
