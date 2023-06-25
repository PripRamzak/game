#pragma once

#include "geometry.hxx"
#include "texture.hxx"

class sprite
{
public:
    virtual void       add_texture(texture* texture)             = 0;
    virtual void       set_current_texture(int current_texture_) = 0;
    virtual void       next_sprite()                             = 0;
    virtual void       move(float x, float y)                    = 0;
    virtual float      get_current_pos_x()                       = 0;
    virtual float      get_current_pos_y()                       = 0;
    virtual vertex_2d* get_vertices()                            = 0;
    virtual texture*   get_sprite()                              = 0;
    ~sprite();
};

sprite* create_sprite(float pos_x,
                      float pos_y,
                      float width,
                      float height,
                      int   window_width,
                      int   window_height);