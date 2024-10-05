#include "include/sprite.hxx"
#include "include/camera.hxx"

#include <glm/gtc/type_ptr.hpp>

namespace prip_engine
{
shader_program* sprite::shader = nullptr;

sprite::sprite(texture*    texture,
               transform2d size,
               transform2d origin,
               transform2d min_uv,
               transform2d max_uv)
    : tex(texture)
    , size(size)
    , origin(origin)
{
    vertex2d_uv vertices[4];
    vertices[0] = { -(size.x / 2), -(size.y / 2), min_uv.x, max_uv.y };
    vertices[1] = { size.x / 2, -(size.y / 2), max_uv.x, max_uv.y };
    vertices[2] = { size.x / 2, size.y / 2, max_uv.x, min_uv.y };
    vertices[3] = { -(size.x / 2), size.y / 2, min_uv.x, min_uv.y };

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

void sprite::draw(transform2d pos,
                  transform2d scale,
                  float       rotation_angle,
                  bool        camera_relative)
{
    shader->use();

    glm::mat4 model =
        glm::translate(glm::mat4{ 1.f }, glm::vec3{ pos.x, pos.y, 0.f });
    model = glm::rotate(
        model, glm::radians(rotation_angle), glm::vec3(0.f, 1.f, 0.f));
    model = glm::translate(model, glm::vec3{ origin.x, origin.y, 0.f });
    model = glm::scale(model, glm::vec3(scale.x, scale.y, 0.f));

    shader->set_uniform_matrix4fv("model", 1, false, glm::value_ptr(model));

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

transform2d sprite::get_origin()
{
    return origin;
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