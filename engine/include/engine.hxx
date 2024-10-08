#pragma once

#include "event.hxx"
#include "sound_buffer.hxx"
#include "texture.hxx"
#include "vertex_array.hxx"

#include <chrono>

namespace prip_engine
{
using timepoint = std::chrono::time_point<std::chrono::high_resolution_clock,
                                          std::chrono::nanoseconds>;

enum class gui_type
{
    menu,
    in_game_menu
};

bool init();
bool read_input(event& e);
void draw_arrays(primitives primitive, int start, int count);
void draw_elements(primitives primitive, int count);
// virtual void      render_buttons(float* matrix)                     = 0;
bool                 render_gui(bool& show_menu_window, gui_type type);
bool                 swap_buffers();
void                 clear();
timepoint            get_time();
[[maybe_unused]] int get_window_width();
[[maybe_unused]] int get_window_height();
int                  get_window_width_pixels();
int                  get_window_height_pixels();
void                 destroy();
} // namespace prip_engine