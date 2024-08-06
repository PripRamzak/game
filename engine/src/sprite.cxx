#include "include/sprite.hxx"

namespace prip_engine
{

sprite::sprite(texture* textures, transform2d size)
    : textures(textures)
    , size(size)
{
    vertex2d_uv vertices[4];
    vertices[0] = { -(size.x / 2), -(size.y / 2), 0.f, 1.f };
    vertices[1] = { size.x / 2, -(size.y / 2), 1.f, 1.f };
    vertices[2] = { size.x / 2, size.y / 2, 1.f, 0.f };
    vertices[3] = { -(size.x / 2), size.y / 2, 0.f, 0.f };

    vertex_buffer* vbo = create_vertex_buffer();
    index_buffer*  ebo = create_index_buffer();
    vao                = create_vertex_array(vbo, ebo);

    vao->bind();

    vbo->buffer_data(vertices, static_cast<size_t>(4));
    vbo->set_attrib_pointer(0, 2, sizeof(vertex2d_uv), 0);
    vbo->set_attrib_pointer(
        1, 2, sizeof(vertex2d_uv), offsetof(vertex2d_uv, uv));

    ebo->add_indexes(primitives::triangle, 1);
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

    vertex_buffer* vbo = create_vertex_buffer();
    index_buffer*  ebo = create_index_buffer();
    vao                = create_vertex_array(vbo, ebo);

    vao->bind();

    vbo->buffer_data(vertices, static_cast<size_t>(4));
    vbo->set_attrib_pointer(0, 2, sizeof(vertex2d_uv), 0);
    vbo->set_attrib_pointer(
        1, 2, sizeof(vertex2d_uv), offsetof(vertex2d_uv, uv));

    ebo->add_indexes(primitives::triangle, 1);
}

transform2d sprite::get_size()
{
    return size;
}

texture* sprite::get_texture()
{
    return textures;
}

vertex_array* sprite::get_vertex_array()
{
    return vao;
}

sprite::~sprite()
{
    delete textures;
    delete vao;
}
} // namespace prip_engine