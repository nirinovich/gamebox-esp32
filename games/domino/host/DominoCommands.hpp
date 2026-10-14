#pragma once

#include <string>
#include <memory>
#include <gamehub/core/ICommand.hpp>

namespace gamehub {
namespace core {

class DominoGame;

class PlayTileCommand : public ICommand {
public:
    PlayTileCommand(int index, std::string endChoice = "")
        : m_index(index), m_endChoice(std::move(endChoice)) {}

    bool execute(Game& game, PlayerId playerId) const override;
    std::string getType() const override { return "play"; }

    int getIndex() const { return m_index; }
    const std::string& getEndChoice() const { return m_endChoice; }

private:
    int m_index;
    std::string m_endChoice;
};

class PassTurnCommand : public ICommand {
public:
    PassTurnCommand() = default;

    bool execute(Game& game, PlayerId playerId) const override;
    std::string getType() const override { return "pass"; }
};

class ChooseEndCommand : public ICommand {
public:
    explicit ChooseEndCommand(std::string choice) : m_choice(std::move(choice)) {}

    bool execute(Game& game, PlayerId playerId) const override;
    std::string getType() const override { return "choose_end"; }

    const std::string& getChoice() const { return m_choice; }

private:
    std::string m_choice;
};

class DominoCommandFactory : public CommandFactory {
public:
    std::unique_ptr<ICommand> createCommand(const std::string& jsonString) const override {
        // Fast, robust JSON extraction for Domino commands
        if (jsonString.find("\"cmd\":\"play\"") != std::string::npos ||
            jsonString.find("\"action\":\"play\"") != std::string::npos) {
            
            int idx = 0;
            auto posIdx = jsonString.find("\"index\":");
            if (posIdx != std::string::npos) {
                idx = std::atoi(jsonString.c_str() + posIdx + 8);
            }

            std::string endChoice = "";
            auto posEnd = jsonString.find("\"end\":");
            if (posEnd != std::string::npos) {
                auto start = jsonString.find('\"', posEnd + 6);
                if (start != std::string::npos) {
                    auto end = jsonString.find('\"', start + 1);
                    if (end != std::string::npos) {
                        endChoice = jsonString.substr(start + 1, end - start - 1);
                    }
                }
            }

            return std::make_unique<PlayTileCommand>(idx, endChoice);
        }

        if (jsonString.find("\"cmd\":\"pass\"") != std::string::npos ||
            jsonString.find("\"action\":\"pass\"") != std::string::npos) {
            return std::make_unique<PassTurnCommand>();
        }

        if (jsonString.find("\"cmd\":\"choose_end\"") != std::string::npos ||
            jsonString.find("\"action\":\"choose_end\"") != std::string::npos) {
            
            std::string choice = "right";
            auto posChoice = jsonString.find("\"choice\":");
            if (posChoice != std::string::npos) {
                auto start = jsonString.find('\"', posChoice + 9);
                if (start != std::string::npos) {
                    auto end = jsonString.find('\"', start + 1);
                    if (end != std::string::npos) {
                        choice = jsonString.substr(start + 1, end - start - 1);
                    }
                }
            }
            return std::make_unique<ChooseEndCommand>(choice);
        }

        return nullptr;
    }
};

} // namespace core
} // namespace gamehub
