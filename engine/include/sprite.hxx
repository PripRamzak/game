#pragma once

#include "engine.hxx"
#include "geometry.hxx"
#include "shader_program.hxx"
#include "texture.hxx"
#include "vertex_array.hxx"

namespace prip_engine
{
class sprite
{
public:
    friend bool init();
    friend void destroy();

    sprite(texture*    texture,
           transform2d size,
           transform2d origin = {0.f, 0.f},
           transform2d min_uv = {0.f, 0.f},
           transform2d max_uv = {1.f, 1.f});
    void          draw(transform2d pos,
                       transform2d scale,
                       float       rotation_angle,
                       bool        camera_relative);
    transform2d   get_size();
    transform2d   get_origin();
    texture*      get_texture();
    vertex_array* get_vertex_array();
    ~sprite();

private:
    static shader_program* shader;

    transform2d   size;
    transform2d   origin;
    texture*      tex = nullptr;
    vertex_array* vao = nullptr;
};
} // namespace prip_engine