#pragma once

#include "geometry.hxx"
#include "texture.hxx"
#include "vertex_buffer.hxx"

class sprite
{
public:
    sprite(texture* textures_,
           float    width_,
           float    height_,
           int      quantity_,
           float    start_position_,
           float    animation_time_);
    void           next_sprite();
    void           reset();
    float          get_width();
    float          get_height();
    int            get_quantity();
    int            get_current_number(int direction);
    float          get_start_position();
    float          get_animation_time();
    texture*       get_texture();
    vertex_buffer* get_vertex_buffer();
    ~sprite();

private:
    texture*       textures       = nullptr;
    vertex_buffer* vb             = nullptr;
    float          width          = 0.f;
    float          height         = 0.f;
    int            quantity       = 0;
    int            current_number = 0;
    float          start_position = 0.f;
    float          animation_time = 0.f;
};
