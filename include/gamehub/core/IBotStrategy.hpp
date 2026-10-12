#pragma once

#include <array>
#include <vector>
#include <utility>
#include <cstdint>

namespace gamehub {
namespace core {

/**
 * @brief GoF Strategy Pattern interface for Connect Four bot decision-making.
 */
class IConnectFourStrategy {
public:
    virtual ~IConnectFourStrategy() = default;
    virtual int computeMove(const std::array<int, 42>& board, int botMark) = 0;
};

/**
 * @brief GoF Strategy Pattern interface for Tron light cycle bot decision-making.
 */
class ITronStrategy {
public:
    virtual ~ITronStrategy() = default;
    virtual std::pair<int, int> computeMove(const std::array<int8_t, 900>& grid, int bx, int by, int curDx, int curDy) = 0;
};

/**
 * @brief GoF Strategy Pattern interface for Pong kinematic bot decision-making.
 */
class IPongStrategy {
public:
    virtual ~IPongStrategy() = default;
    virtual float computeTargetY(float bx, float by, float bvx, float bvy, float paddleX, float courtHeight) = 0;
};

/**
 * @brief GoF Strategy Pattern interface for Battleship targeting strategy.
 */
class IBattleshipStrategy {
public:
    virtual ~IBattleshipStrategy() = default;
    virtual std::pair<int, int> computeShot(const std::array<std::array<bool, 10>, 10>& shotsFired,
                                            std::vector<std::pair<int, int>>& targetQueue) = 0;
};

} // namespace core
} // namespace gamehub
