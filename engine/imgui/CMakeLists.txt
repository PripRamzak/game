add_library(imgui STATIC
        imgui.cpp
        imgui.h
        imgui_demo.cpp
        imgui_draw.cpp
        imgui_impl_opengl3.cpp
        imgui_impl_opengl3.h
        imgui_impl_sdl3.cpp
        imgui_impl_sdl3.h
        imgui_tables.cpp
        imgui_widgets.cpp
        imconfig.h
        imgui_internal.h
        imstb_rectpack.h
        imstb_textedit.h
        imstb_truetype.h)
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_property(TARGET imgui PROPERTY POSITION_INDEPENDENT_CODE ON)