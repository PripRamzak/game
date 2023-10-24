#pragma once

#include "geometry.hxx"
#include "texture.hxx"

class sprite
{
public:
    virtual void     next_sprite()                     = 0;
    virtual void     reset()                           = 0;
    virtual float    get_width()                       = 0;
    virtual float    get_height()                      = 0;
    virtual int      get_quantity()                    = 0;
    virtual int      get_current_number(int direction) = 0;
    virtual float    get_start_position()              = 0;
    virtual float    get_animation_time()              = 0;
    virtual texture* get_texture()                     = 0;
    virtual ~sprite();
};

sprite* create_sprite(texture* textures,
                      float    width,
                      float    height,
                      int      quantity,
                      float    start_position,
                      float    animation_time);