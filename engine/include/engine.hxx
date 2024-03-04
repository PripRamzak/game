#pragma once

#include "animation.hxx"
#include "event.hxx"
#include "index_buffer.hxx"
#include "sound_buffer.hxx"
#include "sprite.hxx"
#include "texture.hxx"
#include "vertex_buffer.hxx"

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
void render(animation* anim_sprite, int direction, float* matrix);
void render(sprite* sprite, int direction, float* matrix);
void render(texture*       texture,
            vertex_buffer* vertex_buffer,
            index_buffer*  index_buffer,
            transform2d    min_uv,
            transform2d    max_uv,
            float*         matrix);
void render(vertex_buffer* vb, index_buffer* ib, float* matrix);
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