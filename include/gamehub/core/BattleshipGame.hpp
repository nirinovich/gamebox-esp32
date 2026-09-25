#pragma once
#include "Game.hpp"
#include "GameTypes.hpp"
#include <array>
#include <string>

namespace gamehub {
namespace core {

class BattleshipGame : public Game {
public:
    static constexpr int SIZE = 10;

    BattleshipGame();

    void init() override;
    void update(float dt) override;
    void handleInput(PlayerId player, const std::string& input) override;
    std::string serializeState() const override;
    bool isFinished() const override { return m_finished; }
    int  getWinner() const override  { return m_winner; }

private:
    enum class Phase { PLACEMENT, BATTLE };
    Phase m_phase{Phase::PLACEMENT};

    struct Grid {
        std::array<std::array<int,SIZE>,SIZE>  ships{};  // 0=empty, N=ship id
        std::array<std::array<bool,SIZE>,SIZE> shots{};  // fired here
        int hits{0};
        int totalShipCells{0};
        bool ready{false};
    };

    Grid m_grids[2];
    int  m_turn{1};
    bool m_finished{false};
    int  m_winner{0};
    int  m_nextShipId[2]{1, 1};

    static constexpr int FLEET[5] = {5, 4, 3, 3, 2};

    bool handlePlacement(int pidx, const std::string& input);
    bool handleAutoPlace(int pidx);
    bool handleFire(int pidx, const std::string& input);
    bool placeShip(int pidx, int shipId, int row, int col, int len, bool horiz);
    std::string serializeGrid(int forPlayer) const;
};

} // namespace core
} // namespace gamehub
