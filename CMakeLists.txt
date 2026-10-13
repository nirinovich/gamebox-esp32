cmake_minimum_required(VERSION 3.16)

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" GAMEBOX_VERSION_STR)
string(STRIP "${GAMEBOX_VERSION_STR}" GAMEBOX_VERSION_STR)

project(ESP32GameHub VERSION ${GAMEBOX_VERSION_STR} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(GAMEBOX_VERSION="${GAMEBOX_VERSION_STR}")

include_directories(include games)

# Core library (OBJECT library preserves all translation unit static initializers without stripping)
add_library(gamehub_core OBJECT
    src/core/Session.cpp
    src/core/Room.cpp
    src/core/RoomManager.cpp
    src/core/RoomState.cpp
    src/core/HostEngine.cpp
    games/snake/host/SnakeGame.cpp
    games/tictactoe/host/TicTacToeGame.cpp
    games/connectfour/host/ConnectFourGame.cpp
    games/pong/host/PongGame.cpp
    games/tron/host/TronGame.cpp
    games/battleship/host/BattleshipGame.cpp
    games/domino/host/DominoGame.cpp
)
target_include_directories(gamehub_core PUBLIC include games)

# Test executables
add_executable(test_tracer_bullet tests/test_tracer_bullet.cpp)
target_link_libraries(test_tracer_bullet PRIVATE gamehub_core)

add_executable(test_snake_game tests/test_snake_game.cpp)
target_link_libraries(test_snake_game PRIVATE gamehub_core)

add_executable(test_tictactoe_game tests/test_tictactoe_game.cpp)
target_link_libraries(test_tictactoe_game PRIVATE gamehub_core)

add_executable(test_room_manager tests/test_room_manager.cpp)
target_link_libraries(test_room_manager PRIVATE gamehub_core)

add_executable(test_snake_duel tests/test_snake_duel.cpp)
target_link_libraries(test_snake_duel PRIVATE gamehub_core)

add_executable(test_new_games tests/test_new_games.cpp)
target_link_libraries(test_new_games PRIVATE gamehub_core)

add_executable(test_host_engine tests/test_host_engine.cpp)
target_link_libraries(test_host_engine PRIVATE gamehub_core)

add_executable(test_domino_diagnosis tests/test_domino_diagnosis.cpp)
target_link_libraries(test_domino_diagnosis PRIVATE gamehub_core)

add_executable(test_desktop_server
    tests/test_desktop_server.cpp
    src/desktop/DesktopServer.cpp
)
if (WIN32)
    target_link_libraries(test_desktop_server PRIVATE gamehub_core ws2_32)
else()
    target_link_libraries(test_desktop_server PRIVATE gamehub_core pthread)
endif()

add_executable(test_game_registry tests/test_game_registry.cpp)
target_link_libraries(test_game_registry PRIVATE gamehub_core)

add_executable(test_catalog_broadcast tests/test_catalog_broadcast.cpp)
target_link_libraries(test_catalog_broadcast PRIVATE gamehub_core)

add_executable(test_action_stream tests/test_action_stream.cpp)
target_link_libraries(test_action_stream PRIVATE gamehub_core)

enable_testing()
add_test(NAME GameRegistryTest COMMAND test_game_registry)
add_test(NAME CatalogBroadcastTest COMMAND test_catalog_broadcast)
add_test(NAME ActionStreamTest COMMAND test_action_stream)
add_test(NAME TracerBulletTest COMMAND test_tracer_bullet)
add_test(NAME SnakeGameTest COMMAND test_snake_game)
add_test(NAME TicTacToeGameTest COMMAND test_tictactoe_game)
add_test(NAME RoomManagerTest COMMAND test_room_manager)
add_test(NAME SnakeDuelTest COMMAND test_snake_duel)
add_test(NAME NewGamesTest COMMAND test_new_games)
add_test(NAME HostEngineTest COMMAND test_host_engine)
add_test(NAME DesktopServerTest COMMAND test_desktop_server)

# Co-located GameModule tests
add_executable(test_snake games/snake/host/test_snake.cpp)
target_link_libraries(test_snake PRIVATE gamehub_core)
add_test(NAME ColocatedSnakeTest COMMAND test_snake)

add_executable(test_tictactoe games/tictactoe/host/test_tictactoe.cpp)
target_link_libraries(test_tictactoe PRIVATE gamehub_core)
add_test(NAME ColocatedTicTacToeTest COMMAND test_tictactoe)

add_executable(test_pong games/pong/host/test_pong.cpp)
target_link_libraries(test_pong PRIVATE gamehub_core)
add_test(NAME ColocatedPongTest COMMAND test_pong)

add_executable(test_tron games/tron/host/test_tron.cpp)
target_link_libraries(test_tron PRIVATE gamehub_core)
add_test(NAME ColocatedTronTest COMMAND test_tron)

add_executable(test_connectfour games/connectfour/host/test_connectfour.cpp)
target_link_libraries(test_connectfour PRIVATE gamehub_core)
add_test(NAME ColocatedConnectFourTest COMMAND test_connectfour)

add_executable(test_battleship games/battleship/host/test_battleship.cpp)
target_link_libraries(test_battleship PRIVATE gamehub_core)
add_test(NAME ColocatedBattleshipTest COMMAND test_battleship)

add_executable(test_domino games/domino/host/test_domino.cpp)
target_link_libraries(test_domino PRIVATE gamehub_core)
add_test(NAME ColocatedDominoTest COMMAND test_domino)

# Desktop runner executable
if (WIN32)
    add_executable(gamehub_desktop
        src/desktop/main.cpp
        src/desktop/DesktopServer.cpp
    )
    target_link_libraries(gamehub_desktop PRIVATE gamehub_core ws2_32)
else()
    add_executable(gamehub_desktop
        src/desktop/main.cpp
        src/desktop/DesktopServer.cpp
    )
    target_link_libraries(gamehub_desktop PRIVATE gamehub_core pthread)
endif()

# Native C++ Asset Packer tool
add_executable(asset_packer
    tools/asset_packer.cpp
)

target_include_directories(asset_packer PRIVATE tools include)

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/include/gamehub/web/WebAssets.hpp"
    COMMAND $<TARGET_FILE:asset_packer> "${CMAKE_CURRENT_SOURCE_DIR}/frontend/dist/index.html" "${CMAKE_CURRENT_SOURCE_DIR}/include/gamehub/web/WebAssets.hpp"
    DEPENDS asset_packer "${CMAKE_CURRENT_SOURCE_DIR}/frontend/dist/index.html"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Packing web assets with native C++ asset_packer (RFC 1952 Gzip)"
)

add_custom_target(generate_assets
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/gamehub/web/WebAssets.hpp"
)

