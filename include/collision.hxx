#pragma once

#include "map.hxx"

namespace collision
{

class collider
{
public:
    collider();
    collider(transform2d offset,
             transform2d rect_size,
             color       color,
             float       scale,
             int         direction);
    void           change_pos(int direction);
    rectangle&     get_rectangle();
    vertex_buffer* get_vertex_buffer();
    index_buffer*  get_index_buffer();
    ~collider();

private:
    rectangle      rect;
    transform2d    offset;
    vertex_buffer* vb;
    index_buffer*  ib;
};

enum class direction
{
    up,
    down,
    left,
    right
};

bool map_with_game_object(map*             map,
                          transform2d&     game_object_pos,
                          const rectangle& collider,
                          direction        direction);
bool game_object_with_game_object(const transform2d& game_object1_pos,
                                  const rectangle&   game_object1_collider,
                                  const transform2d& game_object2_pos,
                                  const rectangle&   game_object2_collider);
} // namespace collision