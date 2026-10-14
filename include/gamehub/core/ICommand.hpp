#pragma once

#include <string>
#include <memory>
#include "GameTypes.hpp"

namespace gamehub {
namespace core {

class Game;

/**
 * @brief GoF Command Pattern interface.
 * Encapsulates a player action as an object, decoupling transport/wire-format from domain execution.
 */
class ICommand {
public:
    virtual ~ICommand() = default;

    /**
     * @brief Execute the command against the target Game and PlayerId.
     * @return true if the command was valid and executed, false otherwise.
     */
    virtual bool execute(Game& game, PlayerId playerId) const = 0;

    /**
     * @brief Canonical command type identifier (e.g. "play_tile", "roll_dice", "sow_pit").
     */
    virtual std::string getType() const = 0;
};

/**
 * @brief GoF Factory interface for deserializing wire-format JSON strings into ICommand instances.
 */
class CommandFactory {
public:
    virtual ~CommandFactory() = default;

    /**
     * @brief Parse JSON input into a concrete ICommand subclass.
     * @return std::unique_ptr<ICommand> on success, nullptr on invalid/malformed command.
     */
    virtual std::unique_ptr<ICommand> createCommand(const std::string& jsonString) const = 0;
};

} // namespace core
} // namespace gamehub
