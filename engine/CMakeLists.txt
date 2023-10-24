add_library(engine SHARED
        src/engine.cxx
        include/engine.hxx
        include/buttons.hxx
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