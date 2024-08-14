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

    sprite(texture* texture, transform2d size);
    sprite(texture* texture, transform2d size, transform2d origin);
    void          draw(float* m_model, bool camera_relative);
    transform2d   get_size();
    texture*      get_texture();
    vertex_array* get_vertex_array();
    ~sprite();

private:
    static shader_program* shader;

    transform2d   size;
    transform2d   scale;
    texture*      tex = nullptr;
    vertex_array* vao = nullptr;
};
} // namespace prip_engine