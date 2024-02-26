#include "include/sprite.hxx"

sprite::sprite(texture* textures, transform2d size)
    : textures(textures)
    , size(size)
{
    vertex2d_uv vertices[4];
    vertices[0] = { -(size.x / 2), -(size.y / 2), 0.f, 1.f };
    vertices[1] = { size.x / 2, -(size.y / 2), 1.f, 1.f };
    vertices[2] = { size.x / 2, size.y / 2, 1.f, 0.f };
    vertices[3] = { -(size.x / 2), size.y / 2, 0.f, 0.f };

    vb = create_vertex_buffer();
    vb->buffer_data(vertices, static_cast<size_t>(4));
}

sprite::sprite(texture* textures, transform2d size, transform2d origin)
    : textures(textures)
    , size(size)
{
    vertex2d_uv vertices[4];
    vertices[0] = { -origin.x, -origin.y, 0.f, 1.f };
    vertices[1] = { size.x - origin.x, -origin.y, 1.f, 1.f };
    vertices[2] = { size.x - origin.x, size.y - origin.y, 1.f, 0.f };
    vertices[3] = { -origin.x, size.y - origin.y, 0.f, 0.f };

    vb = create_vertex_buffer();
    vb->buffer_data(vertices, static_cast<size_t>(4));
}

transform2d sprite::get_size()
{
    return size;
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