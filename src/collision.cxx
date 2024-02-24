#include "include/collision.hxx"
#include "include/game_object.hxx"

#include <unordered_map>
#include <vector>

namespace collision
{
static std::vector<map_tile_type> collision_tiles_dir_up{
    map_tile_type::brick_top,
    map_tile_type::plate_top_left,
    map_tile_type::plate_top_right
};
static std::vector<map_tile_type> collision_tiles_dir_down{
    map_tile_type::brick_bottom,
    map_tile_type::plate_bottom_left,
    map_tile_type::plate_bottom_right
};
static std::vector<map_tile_type> collision_tiles_dir_left{
    map_tile_type::brick_left,
    map_tile_type::plate_top_left,
    map_tile_type::plate_bottom_left
};
static std::vector<map_tile_type> collision_tiles_dir_right{
    map_tile_type::brick_right,
    map_tile_type::plate_top_right,
    map_tile_type::plate_bottom_right
};

static std::unordered_map<direction, std::vector<map_tile_type>>
    collision_tiles_dirs{
        { direction::up, collision_tiles_dir_up },
        { direction::down, collision_tiles_dir_down },
        { direction::left, collision_tiles_dir_left },
        { direction::right, collision_tiles_dir_right },
    };

collider::collider()
{
    vb = create_vertex_buffer();
    ib = create_index_buffer();
}

collider::collider(transform2d offset,
                   transform2d rect_size,
                   color       color,
                   float       scale)
    : offset({ offset.x * scale, offset.y * scale })
    , rect({ offset.x * scale,
             offset.y * scale,
             rect_size.x * scale,
             rect_size.y * scale })
{
    vb = create_vertex_buffer();
    ib = create_index_buffer();

    vertex2d_color vertices[4];
    vertices[0] = { 0.f, 0.f, color };
    vertices[1] = { rect.size.x, 0.f, color };
    vertices[2] = { rect.size.x, rect.size.y, color };
    vertices[3] = { 0.f, rect.size.y, color };

    vb->buffer_data(vertices, static_cast<size_t>(4));
    ib->add_indexes(primitives::line, 1);
}

void collider::change_pos(int direction)
{
    rect.pos.x = direction == 0 ? offset.x : -1 * (rect.size.x + offset.x);
}

rectangle& collider::get_rectangle()
{
    return rect;
}

vertex_buffer* collider::get_vertex_buffer()
{
    return vb;
}

index_buffer* collider::get_index_buffer()
{
    return ib;
}

collider::~collider()
{
    delete vb;
    delete ib;
}

bool map_with_game_object(map*             map,
                          transform2d&     game_object_pos,
                          const rectangle& collider,
                          direction        direction)
{
    for (auto& type : collision_tiles_dirs[direction])
    {
        const vertex2d_uv* map_tile_vertices = map->get_vertices(type).data();
        const size_t map_tile_vertices_num   = map->get_vertices(type).size();

        for (size_t j = 0; j < map_tile_vertices_num / 4;
             j++, map_tile_vertices += 4)
        {
            bool collision_right =
                game_object_pos.x + collider.pos.x + collider.size.x >
                map_tile_vertices->pos.x;
            bool collision_left = (map_tile_vertices + 2)->pos.x >
                                  game_object_pos.x + collider.pos.x;
            bool collision_up = (map_tile_vertices + 2)->pos.y >
                                game_object_pos.y + collider.pos.y;
            bool collision_down;

            if (direction == direction::left || direction == direction::right)
                collision_down =
                    game_object_pos.y + collider.pos.y + collider.size.y >
                    map_tile_vertices->pos.y;
            else
                collision_down =
                    game_object_pos.y + collider.pos.y + collider.size.y >=
                    map_tile_vertices->pos.y;

            bool collision_x = collision_left && collision_right;
            bool collision_y = collision_up && collision_down;

            if (collision_x && collision_y)
            {
                std::cout << "collision" << std::endl;
                switch (direction)
                {
                    case direction::up:
                        std::cout << "top" << std::endl;
                        game_object_pos.y -= game_object_pos.y +
                                             collider.pos.y -
                                             (map_tile_vertices + 2)->pos.y;
                        break;
                    case direction::down:
                        std::cout << "bottom" << std::endl;
                        game_object_pos.y -= game_object_pos.y +
                                             collider.pos.y + collider.size.y -
                                             map_tile_vertices->pos.y;
                        break;
                    case direction::left:
                        std::cout << "left" << std::endl;
                        game_object_pos.x -= game_object_pos.x +
                                             collider.pos.x -
                                             (map_tile_vertices + 2)->pos.x;
                        break;
                    case direction::right:
                        std::cout << "right" << std::endl;
                        game_object_pos.x -= game_object_pos.x +
                                             collider.pos.x + collider.size.x -
                                             map_tile_vertices->pos.x;
                        break;
                }
                return true;
            }
        }
    }

    return false;
}

bool game_object_with_game_object(const transform2d& game_object1_pos,
                                  const rectangle&   game_object1_collider,
                                  const transform2d& game_object2_pos,
                                  const rectangle&   game_object2_collider)
{
    bool collision_x = game_object1_pos.x + game_object1_collider.pos.x +
                               game_object1_collider.size.x >=
                           game_object2_pos.x + game_object2_collider.pos.x &&
                       game_object2_pos.x + game_object2_collider.pos.x +
                               game_object2_collider.size.x >=
                           game_object1_pos.x + game_object1_collider.pos.x;
    bool collision_y = game_object1_pos.y + game_object1_collider.pos.y +
                               game_object1_collider.size.y >=
                           game_object2_pos.y + game_object2_collider.pos.y &&
                       game_object2_pos.y + game_object2_collider.pos.y +
                               game_object2_collider.size.y >=
                           game_object1_pos.y + game_object1_collider.pos.y;
    bool collision = collision_x && collision_y;

    if (collision)
        std::cout << "yes" << std::endl;
    else
        std::cout << "no" << std::endl;

    return collision;
}

} // namespace collision