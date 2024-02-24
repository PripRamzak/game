#include "include/sprite.hxx"

sprite::sprite(texture* textures_, float width_, float height_)
    : textures(textures_)
    , width(width_)
    , height(height_)
{
    vertex2d_uv vertices[4];
    vertices[0] = { -(width / 2), -(height / 2), 0.f, 1.f };
    vertices[1] = { width / 2, -(height / 2), 1.f, 1.f };
    vertices[2] = { width / 2, height / 2, 1.f, 0.f };
    vertices[3] = { -(width / 2), height / 2, 0.f, 0.f };

    vb = create_vertex_buffer();
    vb->buffer_data(vertices, static_cast<size_t>(4));
}

float sprite::get_width()
{
    return width;
}

float sprite::get_height()
{
    return height;
}

texture* sprite::get_texture()
{
    return textures;
}

vertex_buffer* sprite::get_vertex_buffer()
{
    return vb;
}

sprite::~sprite()
{
    delete textures;
    delete vb;
}