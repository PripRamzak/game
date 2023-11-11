#pragma once

#include "geometry.hxx"
#include "texture.hxx"
#include "vertex_buffer.hxx"

class sprite
{
public:
    sprite(texture* textures_, float width_, float height_);
    float          get_width();
    float          get_height();
    texture*       get_texture();
    vertex_buffer* get_vertex_buffer();
    ~sprite();

private:
    texture*       textures = nullptr;
    vertex_buffer* vb       = nullptr;
    float          width    = 0.f;
    float          height   = 0.f;
};
