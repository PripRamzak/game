#pragma once

#include "map.hxx"

namespace collision
{
class collider
{
public:
    collider();
    collider(prip_engine::transform2d offset,
             prip_engine::transform2d rect_size,
             prip_engine::color       color,
             float                    scale,
             int                      direction);
    void                    draw(const prip_engine::transform2d& pos);
    void                    change_pos(int direction);
    prip_engine::rectangle& get_rectangle();
    prip_engine::vertex_array* get_vertex_array();
    ~collider();

private:
    prip_engine::rectangle     rect;
    prip_engine::transform2d   offset;
    prip_engine::vertex_array* vao;
};

enum class direction
{
    up,
    down,
    left,
    right
};

bool map_with_game_object(map*                          map,
                          prip_engine::transform2d&     game_object_pos,
                          const prip_engine::rectangle& collider,
                          direction                     direction);
bool game_object_with_game_object(
    const prip_engine::transform2d& game_object1_pos,
    const prip_engine::rectangle&   game_object1_collider,
    const prip_engine::transform2d& game_object2_pos,
    const prip_engine::rectangle&   game_object2_collider);
} // namespace collision