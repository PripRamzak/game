#include "include/map.hxx"
#include "include/enemy.hxx"

#include <algorithm>
#include <iostream>

#include <glm/matrix.hpp>

static texture* dungeon = nullptr;

void map::initialize()
{
    dungeon = create_texture("img/dungeon.png");
}

map::map(float tile_width_, float tile_height_)
    : tile_width(tile_width_)
    , tile_height(tile_height_)
{
    tileset = dungeon;

    tile wall = {
        32.f / 240.f, 48.0 / 240.0, 1.0 - 48.0 / 288.0, 1.0 - 32.0 / 288.0
    };
    tile brick_top = {
        32.f / 240.f, 48.0 / 240.0, 1.0 - 64.0 / 288.0, 1.0 - 48.0 / 288.0
    };
    tile brick_bottom = {
        32.f / 240.f, 48.0 / 240.0, 1.0 - 32.0 / 288.0, 1.0 - 16.0 / 288.0
    };
    tile brick_left = {
        48.f / 240.f, 64.0 / 240.0, 1.0 - 48.0 / 288.0, 1.0 - 32.0 / 288.0
    };
    tile brick_right = {
        16.f / 240.f, 32.0 / 240.0, 1.0 - 48.0 / 288.0, 1.0 - 32.0 / 288.0
    };

    tiles.emplace(map_tile_type::wall, wall);
    tiles.emplace(map_tile_type::brick_top, brick_top);
    tiles.emplace(map_tile_type::brick_bottom, brick_bottom);
    tiles.emplace(map_tile_type::brick_left, brick_left);
    tiles.emplace(map_tile_type::brick_right, brick_right);

    /*add_tile(
        wall_top_corner_left, map_tile_type::wall_corner_top_left, 0.f, 0.f);
    add_tile(
        wall_top_corner_right, map_tile_type::wall_corner_top_right, 0.f, 0.f);
    add_tile(wall_bottom_corner_left,
             map_tile_type::wall_corner_bottom_left,
             0.f,
             0.f);
    add_tile(wall_bottom_corner_right,
             map_tile_type::wall_corner_bottom_right,
             0.f,
             0.f);*/
}

void map::draw_vertical_line(int           start_x,
                             int           start_y,
                             int           length,
                             map_tile_type type)
{
    tile& tile = tiles[type];
    tile.tile_vertex_buffer->add_vertex(
        { static_cast<float>(start_x - 1) * tile_width,
          static_cast<float>(start_y - 1) * tile_height,
          0.f,
          static_cast<float>(length) });
    tile.tile_vertex_buffer->add_vertex(
        { static_cast<float>(start_x) * tile_width,
          static_cast<float>(start_y - 1) * tile_height,
          1.f,
          static_cast<float>(length) });
    tile.tile_vertex_buffer->add_vertex(
        { static_cast<float>(start_x) * tile_width,
          static_cast<float>(start_y + length - 1) * tile_height,
          1.f,
          0.f });
    tile.tile_vertex_buffer->add_vertex(
        { static_cast<float>(start_x - 1) * tile_width,
          static_cast<float>(start_y + length - 1) * tile_height,
          0.f,
          0.f });
    tile.tile_vertex_buffer->buffer_data();
    tile.tile_index_buffer->add_indexes(static_cast<size_t>(4));
}

void map::draw_horizontal_line(int           start_x,
                               int           start_y,
                               int           length,
                               map_tile_type type)
{
    tile& tile = tiles[type];
    tile.tile_vertex_buffer->add_vertex(
        { static_cast<float>(start_x - 1) * tile_width,
          static_cast<float>(start_y - 1) * tile_height,
          0.f,
          1.f });
    tile.tile_vertex_buffer->add_vertex(
        { static_cast<float>(start_x + length - 1) * tile_width,
          static_cast<float>(start_y - 1) * tile_height,
          static_cast<float>(length),
          1.f });
    tile.tile_vertex_buffer->add_vertex(
        { static_cast<float>(start_x + length - 1) * tile_width,
          static_cast<float>(start_y) * tile_height,
          static_cast<float>(length),
          0.f });
    tile.tile_vertex_buffer->add_vertex(
        { static_cast<float>(start_x - 1) * tile_width,
          static_cast<float>(start_y) * tile_height,
          0.f,
          0.f });

    tile.tile_vertex_buffer->buffer_data();
    tile.tile_index_buffer->add_indexes(static_cast<size_t>(4));
}

void map::fill_rectangle(
    int start_x, int start_y, int width_, int height_, map_tile_type type)
{
    tile& tile = tiles[type];
    tile.tile_vertex_buffer->add_vertex(
        { static_cast<float>(start_x - 1) * tile_width,
          static_cast<float>(start_y + height_ - 1) * tile_height,
          0.f,
          static_cast<float>(height_) });
    tile.tile_vertex_buffer->add_vertex(
        { static_cast<float>(start_x + width_ - 1) * tile_width,
          static_cast<float>(start_y + height_ - 1) * tile_height,
          static_cast<float>(width_),
          static_cast<float>(height_) });
    tile.tile_vertex_buffer->add_vertex(
        { static_cast<float>(start_x + width_ - 1) * tile_width,
          static_cast<float>(start_y - 1) * tile_height,
          static_cast<float>(width_),
          0.f });
    tile.tile_vertex_buffer->add_vertex(
        { static_cast<float>(start_x - 1) * tile_width,
          static_cast<float>(start_y - 1) * tile_height,
          0.f,
          0.f });

    tile.tile_vertex_buffer->buffer_data();
    tile.tile_index_buffer->add_indexes(static_cast<size_t>(4));
}

texture* map::get_tileset()
{
    return tileset;
}

vertex_buffer* map::get_vertex_buffer(map_tile_type type)
{
    return tiles[type].tile_vertex_buffer;
}

index_buffer* map::get_index_buffer(map_tile_type type)
{
    return tiles[type].tile_index_buffer;
}

float* map::get_tile_min_uv(map_tile_type type)
{
    return tiles[type].min_uv;
}

float* map::get_tile_max_uv(map_tile_type type)
{
    return tiles[type].max_uv;
}

map::tile::tile()
{
    min_uv[0] = 0.f;
    min_uv[1] = 0.f;
    max_uv[0] = 1.f;
    max_uv[1] = 1.f;

    tile_vertex_buffer = create_vertex_buffer();
    tile_index_buffer  = create_index_buffer();
};

map::tile::tile(float min_u, float max_u, float min_v, float max_v)
{
    min_uv[0] = min_u;
    min_uv[1] = min_v;
    max_uv[0] = max_u;
    max_uv[1] = max_v;

    tile_vertex_buffer = create_vertex_buffer();
    tile_index_buffer  = create_index_buffer();
}

/*void delete_tiles_horizontal(int      start_x,
                             int      start_y,
                             int      length,
                             map_tile type) final
{
    auto tile_it = find_tile(type);

    if (tile_it != tiles.end())
    {
        for (int i = start_x - 1; i < start_x + length - 1; i++)
        {
            auto vertex_it = std::find_if(
                tile_it->vertices.begin(),
                tile_it->vertices.end(),
                [&](const vertex_2d vertex)
                {
                    return vertex.x == static_cast<float>(i) * tile_width &&
                           vertex.y == (static_cast<float>(start_y) - 1.f) *
                                           tile_height &&
                           vertex.u == 0.f && vertex.v == 1.0;
                });

            if (vertex_it != tile_it->vertices.end())
                tile_it->vertices.erase(vertex_it, vertex_it + 4);
        }

        tile_it->tile_vertex_buffer->buffer_data(tile_it->vertices.data(),
                                                 tile_it->vertices.size());
        tile_it->tile_index_buffer->delete_indexes(
            static_cast<size_t>(length));
    }
}
void delete_tiles_vertical(int      start_x,
                           int      start_y,
                           int      length,
                           map_tile type) final
{
    auto tile_it = find_tile(type);

    if (tile_it != tiles.end())
    {
        for (int i = start_y - 1; i < start_y + length - 1; i++)
        {
            auto vertex_it = std::find_if(
                tile_it->vertices.begin(),
                tile_it->vertices.end(),
                [&](const vertex_2d vertex)
                {
                    return vertex.x == (static_cast<float>(start_x) - 1.f) *
                                           tile_width &&
                           vertex.y ==
                               static_cast<float>(i) * tile_height &&
                           vertex.u == 0.f && vertex.v == 1.0;
                });

            if (vertex_it != tile_it->vertices.end())
                tile_it->vertices.erase(vertex_it, vertex_it + 4);
        }

        tile_it->tile_vertex_buffer->buffer_data(tile_it->vertices.data(),
                                                 tile_it->vertices.size());
        tile_it->tile_index_buffer->delete_indexes(
            static_cast<size_t>(length));
    }
}*/

map::~map()
{
    delete tileset;
    for (auto& tile : tiles)
    {
        delete tile.second.tile_vertex_buffer;
        delete tile.second.tile_index_buffer;
    }
}

void generate_level_1(map*                 map,
                      std::vector<enemy*>& enemies,
                      float                window_width,
                      float                window_height)
{
    map->draw_horizontal_line(4, 9, 5, map_tile_type::brick_bottom);

    // first room
    map->fill_rectangle(4, 4, 6, 5, map_tile_type::wall);
    map->draw_horizontal_line(4, 2, 6, map_tile_type::brick_top);
    // map->draw_horizontal_line(3, 2, 1, map_tile::wall_corner_top_left);
    // map->draw_horizontal_line(10, 2, 1, map_tile::wall_corner_top_right);
    map->draw_horizontal_line(4, 9, 6, map_tile_type::brick_bottom);
    // map->draw_horizontal_line(3, 9, 1,
    // map_tile_type::wall_corner_bottom_left); map->draw_horizontal_line(10, 9,
    // 1, map_tile::wall_corner_bottom_right);
    map->draw_vertical_line(3, 3, 6, map_tile_type::brick_left);
    map->draw_vertical_line(10, 3, 1, map_tile_type::brick_right);
    map->draw_vertical_line(10, 8, 1, map_tile_type::brick_right);

    // hall from first room to second
    /*map->fill_rectangle(10, 5, 4, 3, map_tile::floor);
    map->draw_horizontal_line(10, 4, 4, map_tile::wall);
    map->draw_horizontal_line(10, 3, 4, map_tile::wall_top);
    map->draw_horizontal_line(10, 8, 4, map_tile::wall);
    map->draw_horizontal_line(10, 8, 4, map_tile::wall_bottom);

    // second room
    map->fill_rectangle(14, 3, 12, 9, map_tile::floor);
    map->draw_horizontal_line(14, 2, 12, map_tile::wall);
    map->draw_horizontal_line(14, 1, 12, map_tile::wall_top);
    map->draw_horizontal_line(13, 1, 1, map_tile::wall_corner_top_left);
    map->draw_horizontal_line(26, 1, 1, map_tile::wall_corner_top_right);
    map->draw_horizontal_line(14, 12, 4, map_tile::wall);
    map->draw_horizontal_line(22, 12, 4, map_tile::wall);
    map->draw_horizontal_line(14, 12, 12, map_tile::wall_bottom);
    map->draw_horizontal_line(13, 12, 1, map_tile::wall_corner_bottom_left);
    map->draw_horizontal_line(26, 12, 1, map_tile::wall_corner_bottom_right);
    map->draw_vertical_line(13, 2, 2, map_tile::wall_left);
    map->draw_vertical_line(13, 8, 4, map_tile::wall_left);
    map->draw_vertical_line(26, 2, 10, map_tile::wall_right);

    // hall from second room to third
    map->fill_rectangle(18, 12, 4, 7, map_tile::floor);
    map->draw_vertical_line(17, 12, 6, map_tile::wall_left);
    map->draw_vertical_line(22, 12, 6, map_tile::wall_right);

    // third room
    map->fill_rectangle(14, 19, 12, 8, map_tile::floor);
    map->draw_horizontal_line(14, 18, 4, map_tile::wall);
    map->draw_horizontal_line(22, 18, 4, map_tile::wall);
    map->draw_horizontal_line(14, 17, 4, map_tile::wall_top);
    map->draw_horizontal_line(22, 17, 4, map_tile::wall_top);
    map->draw_horizontal_line(13, 17, 1, map_tile::wall_corner_top_left);
    map->draw_horizontal_line(26, 17, 1, map_tile::wall_corner_top_right);
    map->draw_horizontal_line(14, 27, 12, map_tile::wall);
    map->draw_horizontal_line(14, 27, 12, map_tile::wall_bottom);
    map->draw_horizontal_line(13, 27, 1, map_tile::wall_corner_bottom_left);
    map->draw_horizontal_line(26, 27, 1, map_tile::wall_corner_bottom_right);
    map->draw_vertical_line(13, 18, 9, map_tile::wall_left);
    map->draw_vertical_line(26, 18, 9, map_tile::wall_right);

    enemy* skeleton_1 = new enemy(3,
                                  4.f,
                                  1500.f,
                                  250.f,
                                  1.75f,
                                  game_object_state::run,
                                  enemy_type::spearman);
    enemies.push_back(skeleton_1);
    enemy* skeleton_2 = new enemy(5,
                                  7.f,
                                  1000.f,
                                  650.f,
                                  1.75f,
                                  game_object_state::run,
                                  enemy_type::warrior);
    enemies.push_back(skeleton_2);
    enemy* skeleton_3 = new enemy(5,
                                  7.f,
                                  1500.f,
                                  1200.f,
                                  1.75f,
                                  game_object_state::run,
                                  enemy_type::warrior);
    enemies.push_back(skeleton_3);
    enemy* skeleton_4 = new enemy(5,
                                  7.f,
                                  1000.f,
                                  1400.f,
                                  1.75f,
                                  game_object_state::run,
                                  enemy_type::warrior);
    enemies.push_back(skeleton_4);
    enemy* skeleton_5 = new enemy(3,
                                  4.f,
                                  1200.f,
                                  1200.f,
                                  1.75f,
                                  game_object_state::run,
                                  enemy_type::spearman);
    enemies.push_back(skeleton_5);*/
}

void game_logic_level_1(map*                 map,
                        game_object*         hero,
                        std::vector<enemy*>& enemies)
{
    static bool second_room_cleaned = false;

    if (hero->get_global_pos_x() > 900.f && !enemies[0]->is_spawned())
    {
        enemies[0]->spawn();
        map->draw_vertical_line(13, 4, 4, map_tile_type::brick_left);
    }
    else if (!enemies[0]->is_alive() && enemies[0]->is_spawned() &&
             !enemies[1]->is_spawned())
        enemies[1]->spawn();
    else if (enemies[1]->is_spawned() && !enemies[1]->is_alive() &&
             !second_room_cleaned)
    {
        // map->delete_tiles_vertical(13, 4, 4, map_tile::wall_left);
        // map->delete_tiles_horizontal(18, 12, 4, map_tile::wall_bottom);
        second_room_cleaned = true;
    }
    else if (hero->get_global_pos_y() > 1300.f && !enemies[2]->is_spawned())
    {
        enemies[2]->spawn();
        map->draw_horizontal_line(18, 17, 4, map_tile_type::brick_top);
        map->draw_horizontal_line(18, 18, 4, map_tile_type::wall);
    }
    else if (!enemies[2]->is_alive() && enemies[2]->is_spawned() &&
             !enemies[3]->is_spawned())
        enemies[3]->spawn();
    else if (!enemies[3]->is_alive() && enemies[3]->is_spawned() &&
             !enemies[4]->is_spawned())
        enemies[4]->spawn();
}