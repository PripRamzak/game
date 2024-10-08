#pragma once

#include "engine.hxx"
#include "geometry.hxx"
#include "shader_program.hxx"
#include "vertex_array.hxx"

namespace prip_engine
{
using collision_resolution = std::pair<bool, float>;

enum class collision_direction
{
    null,
    left,
    right,
    up,
    down
};

class collider
{
public:
    friend bool init();
    friend void destroy();

    collider();
    collider(transform2d& pos,
             transform2d  offset,
             transform2d  rect_size,
             color        color,
             float        scale,
             int          direction,
             bool         dynamic);
    ~collider();
    bool                 detect(collider* other);
    collision_resolution resolve(collider* other, collision_direction dir);
    void                 draw();
    void                 change_pos(int direction);
    const rectangle&     get_rectangle() const;
    const vertex_array*  get_vertex_array() const;

private:
    static shader_program* shader;

    transform2d*  pos = nullptr;
    rectangle     rect;
    transform2d   offset;
    vertex_array* vao;
};

class trigger
{
public:
    enum class type
    {
        damage
    };

    trigger(transform2d& pos,
            transform2d  offset,
            transform2d  rect_size,
            color        color,
            float        scale,
            int          direction,
            bool         dynamic,
            type         type);
    ~trigger();
    bool detect(collider* other);
    type get_type();

private:
    collider* col;
    type      t;
};
} // namespace prip_engine