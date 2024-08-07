#pragma once

#include "geometry.hxx"
#include "texture.hxx"
#include "vertex_array.hxx"

namespace prip_engine
{
class sprite
{
public:
    sprite(texture* textures, transform2d size);
    sprite(texture* textures, transform2d size, transform2d origin);
    transform2d    get_size();
    texture*       get_texture();
    vertex_array* get_vertex_array();
    ~sprite();

private:
    transform2d    size;
    transform2d    scale;
    texture*       textures = nullptr;
    vertex_array*  vao      = nullptr;
};
} // namespace prip_engine