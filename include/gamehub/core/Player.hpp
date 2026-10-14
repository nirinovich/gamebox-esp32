#pragma once

#include <string>
#include <memory>
#include <utility>
#include "GameTypes.hpp"
#include "PlayerController.hpp"

namespace gamehub {
namespace core {

/**
 * @brief Domain entity encapsulating a participant in a Game.
 * Owns a PlayerController Strategy without leaking internal network or bot details.
 */
class Player {
public:
    Player(PlayerId id, std::string name, std::unique_ptr<PlayerController> controller)
        : m_id(id), m_name(std::move(name)), m_controller(std::move(controller)) {}

    PlayerId getId() const { return m_id; }
    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    PlayerController* getController() const { return m_controller.get(); }
    void setController(std::unique_ptr<PlayerController> controller) {
        m_controller = std::move(controller);
    }

    bool isAutonomous() const {
        return m_controller && m_controller->isAutonomous();
    }

private:
    PlayerId m_id;
    std::string m_name;
    std::unique_ptr<PlayerController> m_controller;
};

} // namespace core
} // namespace gamehub
