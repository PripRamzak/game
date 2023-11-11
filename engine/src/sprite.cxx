#include "include/sprite.hxx"

sprite::sprite(texture* textures_, float width_, float height_)
    : textures(textures_)
    , width(width_)
    , height(height_)
{
    vb = create_vertex_buffer();

    vb->add_vertex({ -(width / 2), -(height / 2), 0.f, 1.f });
    vb->add_vertex({ width / 2, -(height / 2), 1.f, 1.f });
    vb->add_vertex({ width / 2, height / 2, 1.f, 0.f });
    vb->add_vertex({ -(width / 2), height / 2, 0.f, 0.f });
    vb->buffer_data();
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