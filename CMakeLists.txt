cmake_minimum_required(VERSION 3.20)

project(dungeon-adventure)

if (SDL3_SRC_DIR)
    add_subdirectory(
            ${SDL3_SRC_DIR}
            ${CMAKE_CURRENT_BINARY_DIR}/SDL_${ANDROID_ABI})
endif ()

if (GLM_SRC_DIR)
    add_subdirectory(
            ${GLM_SRC_DIR}
            ${CMAKE_CURRENT_BINARY_DIR}/GLM_${ANDROID_ABI})
endif ()

if (NOT ANDROID)
    find_package(SDL3 REQUIRED)
    find_package(glm REQUIRED)
endif ()

add_subdirectory(engine)

if (ANDROID)
    add_library(game SHARED)
else ()
    add_executable(game)
endif ()

target_sources(game PRIVATE
        src/game.cxx
        src/camera.cxx
        include/camera.hxx
        include/collision.hxx
        src/collision.cxx
        src/enemy.cxx
        include/enemy.hxx
        src/game_object.cxx
        include/game_object.hxx
        src/hero.cxx
        include/hero.hxx
        src/interface.cxx
        include/interface.hxx
        src/map.cxx
        include/map.hxx
        src/resources.cxx
        include/resources.hxx)
target_compile_features(game PRIVATE cxx_std_20)
target_link_libraries(game PRIVATE engine SDL3::SDL3-shared glm::glm)
target_include_directories(game PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
