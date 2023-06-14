cmake_minimum_required(VERSION 3.20)

project(dungeon-adventure)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/imgui)

add_library(engine SHARED
        engine.cxx
        engine.hxx
        event.cxx
        event.hxx
        geometry.cxx
        geometry.hxx
        shader_program.cxx
        shader_program.hxx
        glad/glad.c
        glad/glad.h
        KHR/khrplatform.h
        stb_image.h texture.cxx texture.hxx vertex_buffer.cxx vertex_buffer.hxx index_buffer.cxx index_buffer.hxx)
target_compile_features(engine PUBLIC cxx_std_20)
target_include_directories(engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(engine PRIVATE imgui)
target_link_libraries(engine PRIVATE SDL3::SDL3-shared)

add_executable(game game.cxx)
target_compile_features(game PUBLIC cxx_std_20)
target_link_libraries(game PRIVATE engine)
