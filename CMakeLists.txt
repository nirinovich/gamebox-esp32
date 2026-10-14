cmake_minimum_required(VERSION 3.16)

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" GAMEBOX_VERSION_STR)
string(STRIP "${GAMEBOX_VERSION_STR}" GAMEBOX_VERSION_STR)

project(ESP32GameHub VERSION ${GAMEBOX_VERSION_STR} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(GAMEBOX_VERSION="${GAMEBOX_VERSION_STR}")
if (MSVC)
    add_compile_options(/FS)
endif()

include_directories(include games)

# Core library (OBJECT library preserves all translation unit static initializers without stripping)
add_library(gamehub_core OBJECT
    src/core/Session.cpp
    src/core/Room.cpp
    src/core/RoomManager.cpp
    src/core/RoomState.cpp
    src/core/HostEngine.cpp
    games/domino/host/DominoGame.cpp
    games/katro/host/KatroGame.cpp
    games/fanorona/host/FanoronaGame.cpp
)
target_include_directories(gamehub_core PUBLIC include games)

# Test executables
# miniz in-tree vendor dependency
add_library(miniz STATIC
    third_party/miniz/miniz.c
    third_party/miniz/miniz_tdef.c
    third_party/miniz/miniz_tinfl.c
    third_party/miniz/miniz_zip.c
)
target_include_directories(miniz PUBLIC third_party/miniz)

add_executable(test_tracer_bullet tests/test_tracer_bullet.cpp)
target_link_libraries(test_tracer_bullet PRIVATE gamehub_core miniz)

add_executable(test_room_manager tests/test_room_manager.cpp)
target_link_libraries(test_room_manager PRIVATE gamehub_core)

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
target_include_directories(test_desktop_server PRIVATE third_party/httplib)

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
add_test(NAME RoomManagerTest COMMAND test_room_manager)
add_test(NAME HostEngineTest COMMAND test_host_engine)
add_test(NAME DesktopServerTest COMMAND test_desktop_server)

# Co-located GameModule tests
add_executable(test_domino games/domino/host/test_domino.cpp)
target_link_libraries(test_domino PRIVATE gamehub_core)
add_test(NAME ColocatedDominoTest COMMAND test_domino)

add_executable(test_katro games/katro/host/test_katro.cpp)
target_link_libraries(test_katro PRIVATE gamehub_core)
add_test(NAME ColocatedKatroTest COMMAND test_katro)

add_executable(test_core_oop_patterns tests/test_core_oop_patterns.cpp)
target_link_libraries(test_core_oop_patterns PRIVATE gamehub_core)
add_test(NAME CoreOopPatternsTest COMMAND test_core_oop_patterns)

add_executable(test_fanorona games/fanorona/host/test_fanorona.cpp)
target_link_libraries(test_fanorona PRIVATE gamehub_core)
add_test(NAME ColocatedFanoronaTest COMMAND test_fanorona)

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
target_include_directories(gamehub_desktop PRIVATE third_party/httplib)

# Native C++ Asset Packer tool
add_executable(asset_packer
    tools/asset_packer.cpp
)
target_link_libraries(asset_packer PRIVATE miniz)
target_include_directories(asset_packer PRIVATE tools include third_party/miniz)

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

add_dependencies(gamehub_desktop generate_assets)
