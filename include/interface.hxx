#pragma once

#include "engine/include/texture.hxx"
#include "engine/include/vertex_buffer.hxx"

class interface
{
public:
    virtual void add_texture(prip_engine::texture* texture_) = 0;
    virtual void set_texture(int number)                     = 0;
    virtual prip_engine::texture*       get_texture()        = 0;
    virtual prip_engine::vertex_buffer* get_vertex_buffer()  = 0;
    virtual ~interface();
};

interface* create_interface(float pos_x, float pos_y, int width, int height);