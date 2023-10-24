#pragma once

#include "event.hxx"
#include "index_buffer.hxx"
#include "sound_buffer.hxx"
#include "sprite.hxx"
#include "texture.hxx"
#include "vertex_buffer.hxx"

#include <iostream>
#include <vector>

enum class gui_type
{
    menu,
    in_game_menu
};

class engine
{
public:
    virtual bool          initialize()                               = 0;
    virtual bool          read_input(event& e)                       = 0;
    virtual bool          check_key(key key_)                        = 0;
    virtual sound_buffer* create_sound_buffer(const char* file_path) = 0;
    // clang-format off
    virtual void render(vertex_buffer* vertex_buffer,
                        index_buffer*  index_buffer,
                        sprite*       sprite,
                        int direction,
                        float* matrix_first_value)   = 0;
    virtual void render(vertex_buffer* vertex_buffer,
                        index_buffer*  index_buffer,
                        texture*       texture,
                        float* matrix_first_value)   = 0;
    // clang-format on
    virtual void  render_buttons(float* matrix_first_value)         = 0;
    virtual bool  render_gui(bool& show_menu_window, gui_type type) = 0;
    virtual bool  swap_buffers()                                    = 0;
    virtual void  clear()                                           = 0;
    virtual float get_time()                                        = 0;
    [[maybe_unused]] virtual int get_window_width()                 = 0;
    [[maybe_unused]] virtual int get_window_height()                = 0;
    virtual int                  get_window_width_pixels()          = 0;
    virtual int                  get_window_height_pixels()         = 0;
    virtual void                 uninitialize()                     = 0;
    virtual ~engine();
};

engine* create_engine();

void destroy_engine(engine* engine);
