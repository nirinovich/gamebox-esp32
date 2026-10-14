#pragma once

#include <string>
#include <memory>
#include <cstdlib>
#include <gamehub/core/ICommand.hpp>

namespace gamehub {
namespace core {

class LudoGame;

class ILudoCommand : public ICommand {
public:
    virtual ~ILudoCommand() = default;
    virtual bool executeOnLudo(LudoGame& game, PlayerId playerId) const = 0;
};

class RollDiceCommand : public ILudoCommand {
public:
    explicit RollDiceCommand(int forcedValue = 0) : m_forcedValue(forcedValue) {}

    bool execute(Game& game, PlayerId playerId) const override;
    bool executeOnLudo(LudoGame& game, PlayerId playerId) const override;
    std::string getType() const override { return "roll"; }

    int getForcedValue() const { return m_forcedValue; }

private:
    int m_forcedValue{0};
};

class MovePawnCommand : public ILudoCommand {
public:
    explicit MovePawnCommand(int pawnIndex) : m_pawnIndex(pawnIndex) {}

    bool execute(Game& game, PlayerId playerId) const override;
    bool executeOnLudo(LudoGame& game, PlayerId playerId) const override;
    std::string getType() const override { return "move"; }

    int getPawnIndex() const { return m_pawnIndex; }

private:
    int m_pawnIndex{0};
};

class LudoCommandFactory : public CommandFactory {
public:
    std::unique_ptr<ICommand> createCommand(const std::string& jsonString) const override {
        if (jsonString.find("\"cmd\":\"roll\"") != std::string::npos ||
            jsonString.find("\"action\":\"roll\"") != std::string::npos) {
            int forcedVal = 0;
            auto posForced = jsonString.find("\"forced\":");
            if (posForced != std::string::npos) {
                forcedVal = std::atoi(jsonString.c_str() + posForced + 9);
            }
            return std::make_unique<RollDiceCommand>(forcedVal);
        }

        if (jsonString.find("\"cmd\":\"move\"") != std::string::npos ||
            jsonString.find("\"action\":\"move\"") != std::string::npos) {
            int pawnIdx = 0;
            auto posPawn = jsonString.find("\"pawn\":");
            if (posPawn != std::string::npos) {
                pawnIdx = std::atoi(jsonString.c_str() + posPawn + 7);
            }
            return std::make_unique<MovePawnCommand>(pawnIdx);
        }

        return nullptr;
    }
};

} // namespace core
} // namespace gamehub
