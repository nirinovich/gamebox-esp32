cmake_minimum_required(VERSION 3.16)
project(ESP32GameHub LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# Core library
add_library(gamehub_core STATIC
    src/core/Session.cpp
    src/core/SnakeGame.cpp
    src/core/TicTacToeGame.cpp
    src/core/Room.cpp
    src/core/RoomManager.cpp
    src/core/ConnectFourGame.cpp
    src/core/PongGame.cpp
    src/core/TronGame.cpp
    src/core/BattleshipGame.cpp
)
target_include_directories(gamehub_core PUBLIC include)

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

enable_testing()
add_test(NAME TracerBulletTest COMMAND test_tracer_bullet)
add_test(NAME SnakeGameTest COMMAND test_snake_game)
add_test(NAME TicTacToeGameTest COMMAND test_tictactoe_game)
add_test(NAME RoomManagerTest COMMAND test_room_manager)
add_test(NAME SnakeDuelTest COMMAND test_snake_duel)
add_test(NAME NewGamesTest COMMAND test_new_games)

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
