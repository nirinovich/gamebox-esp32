#pragma once

#include <cstdint>
#include <string_view>
#include <memory>
#include "Game.hpp"

namespace gamehub {
namespace core {

struct GameId {
    char value[24]{};

    constexpr GameId() = default;

    constexpr GameId(const char* str) {
        if (!str) return;
        size_t i = 0;
        while (str[i] != '\0' && i < sizeof(value) - 1) {
            value[i] = str[i];
            ++i;
        }
        value[i] = '\0';
    }

    GameId(std::string_view sv) {
        size_t len = sv.size() < sizeof(value) - 1 ? sv.size() : sizeof(value) - 1;
        for (size_t i = 0; i < len; ++i) {
            value[i] = sv[i];
        }
        value[len] = '\0';
    }

    constexpr std::string_view view() const {
        return std::string_view(value);
    }

    constexpr const char* c_str() const {
        return value;
    }

    bool operator==(const GameId& other) const {
        return view() == other.view();
    }

    bool operator==(std::string_view other) const {
        return view() == other;
    }

    bool operator==(const char* other) const {
        return view() == (other ? other : "");
    }

    bool operator!=(const GameId& other) const {
        return !(*this == other);
    }

    bool operator!=(std::string_view other) const {
        return view() != other;
    }

    bool operator!=(const char* other) const {
        return view() != (other ? other : "");
    }
};

enum class InputFlag : uint32_t {
    NONE = 0,
    DPAD = 1 << 0,
    GRID_POINTER = 1 << 1,
    DRAG_SLIDER = 1 << 2,
    ACTION_BUTTONS = 1 << 3
};

inline constexpr InputFlag operator|(InputFlag a, InputFlag b) {
    return static_cast<InputFlag>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline constexpr InputFlag operator&(InputFlag a, InputFlag b) {
    return static_cast<InputFlag>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

inline constexpr bool hasInputFlag(InputFlag mask, InputFlag flag) {
    return (static_cast<uint32_t>(mask) & static_cast<uint32_t>(flag)) != 0;
}

struct GameDescriptor {
    GameId id{};
    const char* title{""};
    const char* category{"SOLO"}; // "SOLO", "DUEL", "PARTY"
    const char* description{""};
    int minPlayers{1};
    int maxPlayers{2};
    InputFlag inputTemplate{InputFlag::NONE};
    const char* icon{""};
    bool isDuel{false};
};

using GameCreateFn = std::unique_ptr<Game>(*)();

class GameRegistry {
public:
    struct Entry {
        GameDescriptor descriptor;
        GameCreateFn factory{nullptr};
    };

    static GameRegistry& instance() {
        static GameRegistry s_instance;
        return s_instance;
    }

    bool registerGame(const GameDescriptor& desc, GameCreateFn factory) {
        if (!factory) return false;
        // Check for existing ID
        for (size_t i = 0; i < m_count; ++i) {
            if (m_entries[i].descriptor.id == desc.id) {
                m_entries[i] = {desc, factory};
                return true;
            }
        }
        if (m_count < MAX_GAMES) {
            m_entries[m_count++] = {desc, factory};
            return true;
        }
        return false;
    }

    const Entry* find(std::string_view id) const {
        for (size_t i = 0; i < m_count; ++i) {
            if (m_entries[i].descriptor.id == id) {
                return &m_entries[i];
            }
        }
        return nullptr;
    }

    std::unique_ptr<Game> create(std::string_view id) const {
        const auto* entry = find(id);
        if (entry && entry->factory) {
            return entry->factory();
        }
        return nullptr;
    }

    size_t count() const { return m_count; }
    const Entry& get(size_t index) const { return m_entries[index]; }

    void clear() { m_count = 0; }

private:
    GameRegistry() = default;
    static constexpr size_t MAX_GAMES = 32;
    Entry m_entries[MAX_GAMES]{};
    size_t m_count{0};
};

#define REGISTER_GAME(idStr, regTag, factoryFn, ...) \
    namespace { \
    static const bool _reg_##regTag = []() { \
        ::gamehub::core::GameDescriptor desc __VA_ARGS__; \
        desc.id = ::gamehub::core::GameId(idStr); \
        return ::gamehub::core::GameRegistry::instance().registerGame(desc, factoryFn); \
    }(); \
    }

} // namespace core
} // namespace gamehub
