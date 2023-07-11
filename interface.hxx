#pragma once

#include "texture.hxx"
#include "vertex_buffer.hxx"

class interface
{
public:
    virtual void           add_texture(texture* texture_) = 0;
    virtual void           set_texture(int number)        = 0;
    virtual texture*       get_texture()                  = 0;
    virtual vertex_buffer* get_vertex_buffer()            = 0;
    virtual ~interface();
};

interface* create_interface(float pos_x, float pos_y, int width, int height);