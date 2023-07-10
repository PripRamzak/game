cmake_minimum_required(VERSION 3.20)

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

project(dungeon-adventure)

add_library(engine SHARED
        engine.cxx
        engine.hxx
        buttons.hxx
        event.cxx
        event.hxx
        geometry.hxx
        index_buffer.cxx
        index_buffer.hxx
        memory_buf.cxx
        memory_buf.hxx
        shader_program.cxx
        shader_program.hxx
        sound_buffer.cxx
        sound_buffer.hxx
        sprite.cxx
        sprite.hxx
        texture.cxx
        texture.hxx
        vertex_buffer.cxx
        vertex_buffer.hxx
        stb_image.h)
if (NOT ANDROID)
    target_sources(engine PRIVATE
            glad/glad.c
            glad/glad.h
            KHR/khrplatform.h)
endif ()
target_compile_features(engine PRIVATE cxx_std_20)
target_include_directories(engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/imgui)

if (NOT ANDROID)
    find_package(SDL3 REQUIRED)
    find_package(glm REQUIRED)
endif ()

if (ANDROID)
    target_link_libraries(engine PRIVATE
            SDL3::SDL3-shared
            glm::glm
            android
            log
            EGL
            GLESv2)
else ()
    target_link_libraries(engine PRIVATE
            SDL3::SDL3-shared
            glm::glm)
endif ()
target_link_libraries(engine PRIVATE imgui)

if (ANDROID)
    add_library(game SHARED)
else ()
    add_executable(game)
endif ()

target_sources(game PRIVATE
        game.cxx
        camera.cxx
        camera.hxx
        enemy.cxx
        enemy.hxx
        game_object.cxx
        game_object.hxx
        hero.cxx
        hero.hxx
        map.cxx
        map.hxx)
target_compile_features(game PRIVATE cxx_std_20)
target_link_libraries(game PRIVATE engine SDL3::SDL3-shared glm::glm)
