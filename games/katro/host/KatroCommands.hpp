#pragma once

#include <string>
#include <memory>
#include <cstdlib>
#include <gamehub/core/ICommand.hpp>
#include "KatroEntities.hpp"

namespace gamehub {
namespace core {

class KatroGame;

class IKatroCommand : public ICommand {
public:
    ~IKatroCommand() override = default;

    bool execute(Game& game, PlayerId playerId) const override;
    virtual bool executeOnKatro(KatroGame& game, PlayerId playerId) const = 0;
};

class SowPitCommand : public IKatroCommand {
public:
    explicit SowPitCommand(int cell, KatroDirection dir = KatroDirection::CLOCKWISE, bool hasDir = false)
        : m_cell(cell), m_dir(dir), m_hasDir(hasDir) {}

    bool executeOnKatro(KatroGame& game, PlayerId playerId) const override;
    std::string getType() const override { return "sow"; }

    int getCell() const { return m_cell; }
    KatroDirection getDirection() const { return m_dir; }
    bool hasDirection() const { return m_hasDir; }

private:
    int m_cell;
    KatroDirection m_dir;
    bool m_hasDir;
};

class KatroCommandFactory : public CommandFactory {
public:
    std::unique_ptr<ICommand> createCommand(const std::string& jsonString) const override {
        // Fast extraction of "cell":
        const std::string cellKey = "\"cell\":";
        auto pos = jsonString.find(cellKey);
        if (pos == std::string::npos) {
            return nullptr;
        }

        int cell = -1;
        try {
            cell = std::stoi(jsonString.substr(pos + cellKey.size()));
        } catch (...) {
            return nullptr;
        }

        bool hasDir = false;
        KatroDirection dir = KatroDirection::CLOCKWISE;
        if (jsonString.find("\"ccw\"") != std::string::npos ||
            jsonString.find("\"counter_clockwise\"") != std::string::npos) {
            dir = KatroDirection::COUNTER_CLOCKWISE;
            hasDir = true;
        } else if (jsonString.find("\"cw\"") != std::string::npos ||
                   jsonString.find("\"clockwise\"") != std::string::npos) {
            dir = KatroDirection::CLOCKWISE;
            hasDir = true;
        }

        return std::make_unique<SowPitCommand>(cell, dir, hasDir);
    }
};

} // namespace core
} // namespace gamehub
