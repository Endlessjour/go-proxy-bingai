cmake_minimum_required(VERSION 3.20)
project(MonstoriaEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 QUIET)
find_package(tinyxml2 QUIET)
find_package(nlohmann_json QUIET)
find_package(sol2 QUIET)
find_package(Lua QUIET)

if (NOT SDL2_FOUND OR NOT tinyxml2_FOUND)
    find_package(PkgConfig REQUIRED)
    if (NOT SDL2_FOUND)
        pkg_check_modules(SDL2 REQUIRED sdl2)
    endif()
    if (NOT tinyxml2_FOUND)
        pkg_check_modules(TINYXML2 REQUIRED tinyxml2)
    endif()
endif()

add_executable(monstoria
    src/main.cpp
    src/Core/Application.cpp
    src/Core/GameLoop.cpp
    src/Render/RenderSystem.cpp
    src/Game/Game.cpp
    src/World/Map.cpp
    src/Entity/Player.cpp
    src/Encounter/EncounterSystem.cpp
    src/Battle/BattleSystem.cpp
    src/Capture/CaptureSystem.cpp
    src/Capture/PlayerParty.cpp
    src/Data/Species.cpp
    src/Data/MoveDatabase.cpp
    src/Monster/MonsterFactory.cpp
    src/Script/ScriptSystem.cpp
    src/Save/SaveSystem.cpp
)

target_include_directories(monstoria PRIVATE src)

if (TARGET SDL2::SDL2)
    target_link_libraries(monstoria PRIVATE SDL2::SDL2)
    if (WIN32 AND TARGET SDL2::SDL2main)
        target_link_libraries(monstoria PRIVATE SDL2::SDL2main)
    endif()
else()
    target_include_directories(monstoria PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(monstoria PRIVATE ${SDL2_LIBRARIES})
endif()

if (TARGET tinyxml2::tinyxml2)
    target_link_libraries(monstoria PRIVATE tinyxml2::tinyxml2)
else()
    target_include_directories(monstoria PRIVATE ${TINYXML2_INCLUDE_DIRS})
    target_link_libraries(monstoria PRIVATE ${TINYXML2_LIBRARIES})
endif()

if (TARGET nlohmann_json::nlohmann_json)
    target_link_libraries(monstoria PRIVATE nlohmann_json::nlohmann_json)
endif()

if (TARGET sol2::sol2)
    target_link_libraries(monstoria PRIVATE sol2::sol2)
endif()

if (Lua_FOUND)
    target_include_directories(monstoria PRIVATE ${LUA_INCLUDE_DIR})
    target_link_libraries(monstoria PRIVATE ${LUA_LIBRARIES})
endif()
