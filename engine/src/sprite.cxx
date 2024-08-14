#include "include/sprite.hxx"
#include "include/camera.hxx"
#include "include/engine.hxx"

#include <glm/gtc/type_ptr.hpp>

namespace prip_engine
{
shader_program* sprite::shader = nullptr;

sprite::sprite(texture* texture, transform2d size)
    : tex(texture)
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

    vbo->buffer_data(vertices, static_cast<size_t>(4 * sizeof(vertex2d_uv)));
    vbo->set_attrib_pointer(0, 2, t_float, false, sizeof(vertex2d_uv), 0);
    vbo->set_attrib_pointer(
        1, 2, t_float, false, sizeof(vertex2d_uv), offsetof(vertex2d_uv, uv));

    ebo->add_indexes(primitives::triangle, 1);
}

sprite::sprite(texture* texture, transform2d size, transform2d origin)
    : tex(texture)
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

    vbo->buffer_data(vertices, static_cast<size_t>(4 * sizeof(vertex2d_uv)));
    vbo->set_attrib_pointer(0, 2, t_float, false, sizeof(vertex2d_uv), 0);
    vbo->set_attrib_pointer(
        1, 2, t_float, false, sizeof(vertex2d_uv), offsetof(vertex2d_uv, uv));

    ebo->add_indexes(primitives::triangle, 1);
}

void sprite::draw(float* m_model, bool camera_relative)
{
    shader->use();

    shader->set_uniform_matrix4fv("model", 1, false, m_model);

    if (camera_relative)
        shader->set_uniform_matrix4fv("view", 1, false, camera::get_view());
    else
    {
        glm::mat4 one{ 1.f };
        shader->set_uniform_matrix4fv("view", 1, false, glm::value_ptr(one));
    }
    
    shader->set_uniform_matrix4fv(
        "projection", 1, false, camera::get_projection());

    tex->bind();

    vao->bind();
    draw_elements(primitives::triangle, vao->get_index_buffer()->get_size());
}

transform2d sprite::get_size()
{
    return size;
}

texture* sprite::get_texture()
{
    return tex;
}

vertex_array* sprite::get_vertex_array()
{
    return vao;
}

sprite::~sprite()
{
    delete tex;
    delete vao;
}
} // namespace prip_engine