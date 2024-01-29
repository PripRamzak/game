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

bool map_with_game_object(map*         map,
                          game_object* game_object,
                          direction    direction)
{
    const vertex_2d* game_object_vertices =
        game_object->get_animation()->get_sprite()->get_vertex_buffer()->data();

    for (auto type : collision_tiles_dirs[direction])
    {
        const vertex_2d* map_tile_vertices =
            map->get_vertex_buffer(type)->data();
        const size_t map_tile_vertices_num =
            map->get_vertex_buffer(type)->size();

        for (size_t j = 0; j < map_tile_vertices_num / 4;
             j++, map_tile_vertices += 4)
        {
            bool collision_right =
                game_object->global_pos_x +
                    (game_object_vertices + 2)->x * game_object->size >
                map_tile_vertices->x;
            bool collision_left =
                (map_tile_vertices + 2)->x >
                game_object->global_pos_x +
                    game_object_vertices->x * game_object->size;
            bool collision_up = (map_tile_vertices + 2)->y >
                                game_object->global_pos_y +
                                    game_object_vertices->y * game_object->size;
            bool collision_down;

            if (direction == direction::left || direction == direction::right)
                collision_down =
                    game_object->global_pos_y +
                        (game_object_vertices + 2)->y * game_object->size >
                    map_tile_vertices->y;
            else
                collision_down =
                    game_object->global_pos_y +
                        (game_object_vertices + 2)->y * game_object->size >=
                    map_tile_vertices->y;

            bool collision_x = collision_left && collision_right;
            bool collision_y = collision_up && collision_down;

            if (collision_x && collision_y)
            {
                std::cout << "collision" << std::endl;
                switch (direction)
                {
                    case direction::up:
                        std::cout << "top" << std::endl;
                        game_object->global_pos_y -=
                            game_object->global_pos_y +
                            game_object_vertices->y * game_object->size -
                            (map_tile_vertices + 2)->y;
                        break;
                    case direction::down:
                        std::cout << "bottom" << std::endl;
                        game_object->global_pos_y -=
                            game_object->global_pos_y +
                            (game_object_vertices + 2)->y * game_object->size -
                            map_tile_vertices->y;
                        break;
                    case direction::left:
                        std::cout << "left" << std::endl;
                        game_object->global_pos_x -=
                            game_object->global_pos_x +
                            game_object_vertices->x * game_object->size -
                            (map_tile_vertices + 2)->x;
                        break;
                    case direction::right:
                        std::cout << "right" << std::endl;
                        game_object->global_pos_x -=
                            game_object->global_pos_x +
                            (game_object_vertices + 2)->x * game_object->size -
                            map_tile_vertices->x;
                        break;
                }
                return true;
            }
        }
    }

    return false;
}
} // namespace collision