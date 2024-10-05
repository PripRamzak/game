#include "include/collision.hxx"
#include "include/camera.hxx"
#include "include/engine.hxx"

#include <glm/gtc/type_ptr.hpp>

#include <vector>

namespace prip_engine
{
shader_program* collider::shader = nullptr;

collider::collider()
{
    vao = create_vertex_array(create_vertex_buffer(), create_index_buffer());
}

collider::collider(transform2d& pos,
                   transform2d  offset,
                   transform2d  rect_size,
                   color        color,
                   float        scale,
                   int          direction,
                   bool         dynamic)
    : pos(dynamic ? &pos : new transform2d(pos))
    , offset({ offset.x * scale, offset.y * scale })
    , rect({ direction == 0 ? offset.x * scale
                            : -1 * (rect_size.x + offset.x) * scale,
             offset.y * scale,
             rect_size.x * scale,
             rect_size.y * scale })
{
    vertex_buffer* vbo = create_vertex_buffer();
    index_buffer*  ebo = create_index_buffer();
    vao                = create_vertex_array(vbo, ebo);

    vertex2d_color vertices[4];
    vertices[0] = { 0.f, 0.f, color };
    vertices[1] = { rect.size.x, 0.f, color };
    vertices[2] = { rect.size.x, rect.size.y, color };
    vertices[3] = { 0.f, rect.size.y, color };

    vao->bind();

    vbo->buffer_data(vertices, static_cast<size_t>(4 * sizeof(vertex2d_color)));
    vbo->set_attrib_pointer(0, 2, t_float, false, sizeof(vertex2d_color), 0);
    vbo->set_attrib_pointer(1,
                            4,
                            t_float,
                            false,
                            sizeof(vertex2d_color),
                            offsetof(vertex2d_color, col));

    ebo->add_indexes(primitives::line, 1);
}

collider::~collider()
{
    delete vao;
}

bool collider::detect(collider* other)
{
    bool collision_x = pos->x + rect.pos.x + rect.size.x >=
                           other->pos->x + other->rect.pos.x &&
                       other->pos->x + other->rect.pos.x + other->rect.size.x >=
                           pos->x + rect.pos.x;
    bool collision_y =
        pos->y + rect.pos.y + rect.size.y >= other->pos->y + other->rect.pos.y &&
        other->pos->y + other->rect.pos.y + other->rect.size.y >=
            pos->y + rect.pos.y;
    bool collision = collision_x && collision_y;

    if (collision)
        std::cout << "yes" << std::endl;
    else
        std::cout << "no" << std::endl;

    return collision;
}

collision_resolution collider::resolve(collider* other, collision_direction dir)
{
    float this_left_edge  = pos->x + rect.pos.x;
    float this_right_edge = pos->x + rect.pos.x + rect.size.x;
    float this_up_edge    = pos->y + rect.pos.y;
    float this_down_edge  = pos->y + rect.pos.y + rect.size.y;

    float other_left_edge = other->pos->x + other->rect.pos.x;
    float other_right_edge =
        other->pos->x + other->rect.pos.x + other->rect.size.x;
    float other_up_edge = other->pos->y + other->rect.pos.y;
    float other_down_edge =
        other->pos->y + other->rect.pos.y + other->rect.size.y;

    bool collision_x =
        this_right_edge > other_left_edge && other_right_edge > this_left_edge;

    bool collision_y;
    if (dir == collision_direction::left || dir == collision_direction::right)
        collision_y =
            this_down_edge > other_up_edge && other_down_edge >= this_up_edge;
    else
        collision_y =
            this_down_edge >= other_up_edge && other_down_edge >= this_up_edge;

    bool collision = collision_x && collision_y;

    float diff = 0.f;
    if (collision)
        switch (dir)
        {
            case collision_direction::left:
                diff = this_left_edge - other_right_edge;
                break;
            case collision_direction::right:
                diff = this_right_edge - other_left_edge;
                break;
            case collision_direction::up:
                diff = this_up_edge - other_down_edge;
                break;
            case collision_direction::down:
                diff = this_down_edge - other_up_edge;
                break;
            default:
                break;
        }

    return { collision, diff };
}

void collider::draw()
{
    shader->use();

    glm::mat4 model = glm::translate(
        glm::mat4{ 1 },
        glm::vec3{ pos->x + rect.pos.x, pos->y + rect.pos.y, 0.f });

    shader->set_uniform_matrix4fv("model", 1, false, glm::value_ptr(model));
    shader->set_uniform_matrix4fv("view", 1, false, camera::get_view());
    shader->set_uniform_matrix4fv(
        "projection", 1, false, camera::get_projection());

    vao->bind();
    draw_elements(primitives::line, vao->get_index_buffer()->get_size());
}

void collider::change_pos(int direction)
{
    rect.pos.x = direction == 0 ? offset.x : -1 * (rect.size.x + offset.x);
}

const rectangle& collider::get_rectangle() const
{
    return rect;
}

const vertex_array* collider::get_vertex_array() const
{
    return vao;
}

trigger::trigger(transform2d& pos,
                 transform2d  offset,
                 transform2d  rect_size,
                 color        color,
                 float        scale,
                 int          direction,
                 bool         dynamic,
                 type         type)
    : col(new collider(
          pos, offset, rect_size, color, scale, direction, dynamic))
    , t(type)
{
}

trigger::~trigger()
{
    delete col;
}

bool trigger::detect(collider* other)
{
    return col->detect(other);
}

trigger::type trigger::get_type()
{
    return t;
}

} // namespace prip_engine