add_library(engine SHARED
        src/engine.cxx
        include/engine.hxx
        src/animation.cxx
        include/animation.hxx
        include/buttons.hxx
        src/camera.cxx
        include/camera.hxx
        src/collision.cxx
        include/collision.hxx
        include/color.hxx
        src/event.cxx
        include/event.hxx
        include/geometry.hxx
        src/index_buffer.cxx
        include/index_buffer.hxx
        src/memory_buf.cxx
        include/memory_buf.hxx
        src/shader_program.cxx
        include/shader_program.hxx
        src/sound_buffer.cxx
        include/sound_buffer.hxx
        src/sprite.cxx
        include/sprite.hxx
        src/texture.cxx
        include/texture.hxx
        include/transform2d.hxx
        src/types.cxx
        include/types.hxx
        src/vertex_array.cxx
        include/vertex_array.hxx
        src/vertex_buffer.cxx
        include/vertex_buffer.hxx
        include/stb_image.h)
if (NOT ANDROID)
    target_sources(engine PRIVATE
            glad/glad.c
            glad/glad.h
            KHR/khrplatform.h)
endif ()

target_compile_features(engine PRIVATE cxx_std_20)
target_include_directories(engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/imgui)

target_link_libraries(imgui PUBLIC SDL3::SDL3-shared)

target_link_libraries(engine PRIVATE imgui glm::glm tinyxml2::tinyxml2)

if (ANDROID)
    target_link_libraries(engine PRIVATE
            android
            log
            EGL
            GLESv2)
endif()