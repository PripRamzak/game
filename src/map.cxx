#include "include/map.hxx"
#include "engine/include/memory_buf.hxx"

#include <algorithm>
#include <iostream>

#include <glm/matrix.hpp>

static texture* dungeon = nullptr;

std::istream& operator>>(std::istream& is, map_tile_type& type)
{
    int type_num;
    is >> type_num;
    type = static_cast<map_tile_type>(type_num);
    return is;
}

void map::initialize()
{
    dungeon = create_texture("img/dungeon.png");
}

map::map(float tile_width_, float tile_height_, std::string file_path)
    : tile_width(tile_width_)
    , tile_height(tile_height_)
{
    tileset = dungeon;

    tile brick_top          = { { 32.f / 240.f, 224.f / 288.f },
                                { 48.f / 240.f, 240.f / 288.f } };
    tile brick_bottom       = { { 32.f / 240.f, 256.f / 288.f },
                                { 48.f / 240.f, 272.f / 288.f } };
    tile brick_left         = { { 48.f / 240.f, 240.f / 288.f },
                                { 64.f / 240.f, 256.f / 288.f } };
    tile brick_right        = { { 16.f / 240.f, 240.f / 288.f },
                                { 32.f / 240.f, 256.f / 288.f } };
    tile brick_top_left     = { { 96.f / 240.f, 224.f / 288.f },
                                { 112.f / 240.f, 240.f / 288.f } };
    tile brick_top_right    = { { 80.f / 240.f, 224.f / 288.f },
                                { 96.f / 240.f, 240.f / 288.f } };
    tile brick_bottom_left  = { { 96.f / 240.f, 240.f / 288.f },
                                { 112.f / 240.f, 256.f / 288.f } };
    tile brick_bottom_right = { { 80.f / 240.f, 240.f / 288.f },
                                { 96.f / 240.f, 256.f / 288.f } };
    tile plate_top_left     = { { 48.f / 240.f, 160.f / 288.f },
                                { 64.f / 240.f, 172.f / 288.f } };
    tile plate_top_right    = { { 16.f / 240.f, 160.f / 288.f },
                                { 32.f / 240.f, 172.f / 288.f } };
    tile plate_bottom_left  = { { 48.f / 240.f, 192.f / 288.f },
                                { 64.f / 240.f, 208.f / 288.f } };
    tile plate_bottom_right = { { 16.f / 240.f, 192.f / 288.f },
                                { 32.f / 240.f, 208.f / 288.f } };

    tiles.emplace(map_tile_type::brick_top, brick_top);
    tiles.emplace(map_tile_type::brick_bottom, brick_bottom);
    tiles.emplace(map_tile_type::brick_left, brick_left);
    tiles.emplace(map_tile_type::brick_right, brick_right);
    tiles.emplace(map_tile_type::brick_top_left, brick_top_left);
    tiles.emplace(map_tile_type::brick_top_right, brick_top_right);
    tiles.emplace(map_tile_type::brick_bottom_left, brick_bottom_left);
    tiles.emplace(map_tile_type::brick_bottom_right, brick_bottom_right);
    tiles.emplace(map_tile_type::plate_top_left, plate_top_left);
    tiles.emplace(map_tile_type::plate_top_right, plate_top_right);
    tiles.emplace(map_tile_type::plate_bottom_left, plate_bottom_left);
    tiles.emplace(map_tile_type::plate_bottom_right, plate_bottom_right);

    memory_buf    buf_level = load_file(file_path);
    std::istream  data_level(&buf_level);
    int           method_to_draw_number;
    float         start_x, start_y;
    int           length;
    map_tile_type type;

    while (data_level >> method_to_draw_number)
    {
        data_level >> type >> start_x >> start_y >> length;
        switch (method_to_draw_number)
        {
            case 1:
                draw_vertical_line(start_x, start_y, length, type);
                break;
            case 2:
                draw_horizontal_line(start_x, start_y, length, type);
                break;
            case 3:
                int width;
                data_level >> width;
                fill_rectangle(start_x, start_y, width, length, type);
                break;
            default:
                break;
        }
    }

    for (auto& tile : tiles)
    {
        tile.second.tile_vertex_buffer->buffer_data(
            tile.second.vertices.data(), tile.second.vertices.size());
        tile.second.tile_index_buffer->add_indexes(
            primitives::triangle, tile.second.vertices.size() / 4);
    }
}

texture* map::get_tileset()
{
    return tileset;
}

std::vector<vertex2d_uv>& map::get_vertices(map_tile_type type)
{
    return tiles[type].vertices;
}

vertex_buffer* map::get_vertex_buffer(map_tile_type type)
{
    return tiles[type].tile_vertex_buffer;
}

index_buffer* map::get_index_buffer(map_tile_type type)
{
    return tiles[type].tile_index_buffer;
}

transform2d map::get_tile_min_uv(map_tile_type type)
{
    return tiles[type].min_uv;
}

transform2d map::get_tile_max_uv(map_tile_type type)
{
    return tiles[type].max_uv;
}

map::tile::tile()
{
    min_uv.x = 0.f;
    min_uv.y = 0.f;
    max_uv.x = 1.f;
    max_uv.y = 1.f;

    tile_vertex_buffer = create_vertex_buffer();
    tile_index_buffer  = create_index_buffer();
};

map::tile::tile(transform2d min_uv, transform2d max_uv)
    : min_uv(min_uv)
    , max_uv(max_uv)
{
    tile_vertex_buffer = create_vertex_buffer();
    tile_index_buffer  = create_index_buffer();
}

void map::draw_vertical_line(float         start_x,
                             float         start_y,
                             int           length,
                             map_tile_type type)
{
    tile& tile = tiles[type];
    tile.vertices.push_back(
        { start_x, start_y, 0.f, static_cast<float>(length) });
    tile.vertices.push_back(
        { start_x + tile_width, start_y, 1.f, static_cast<float>(length) });
    tile.vertices.push_back(
        { start_x + tile_width, start_y + length * tile_height, 1.f, 0.f });
    tile.vertices.push_back(
        { start_x, start_y + length * tile_width, 0.f, 0.f });
}

void map::draw_horizontal_line(float         start_x,
                               float         start_y,
                               int           length,
                               map_tile_type type)
{
    tile& tile = tiles[type];
    tile.vertices.push_back({ start_x, start_y, 0.f, 1.f });
    tile.vertices.push_back({ start_x + length * tile_width,
                              start_y,
                              static_cast<float>(length),
                              1.f });
    tile.vertices.push_back({ start_x + length * tile_width,
                              start_y + tile_height,
                              static_cast<float>(length),
                              0.f });
    tile.vertices.push_back({ start_x, start_y + tile_height, 0.f, 0.f });
}

void map::fill_rectangle(
    float start_x, float start_y, int width, int height, map_tile_type type)
{
    tile& tile = tiles[type];
    tile.vertices.push_back(
        { start_x, start_y, 0.f, static_cast<float>(height) });
    tile.vertices.push_back({ start_x + width * tile_width,
                              start_y,
                              static_cast<float>(width),
                              static_cast<float>(height) });
    tile.vertices.push_back({ start_x + width * tile_width,
                              start_y + height * tile_height,
                              static_cast<float>(width),
                              0.f });
    tile.vertices.push_back(
        { start_x, start_y + height * tile_height, 0.f, 0.f });
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
                [&](const vertex2d_uv vertex)
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
                [&](const vertex2d_uv vertex)
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

/*
void game_logic_level_1(map*                 map,
                        game_object*         hero,
                        std::vector<enemy*>& enemies)
{
    static bool second_room_cleaned = false;

    if (hero->get_global_pos_x() > 900.f && !enemies[0]->is_spawned())
    {
        enemies[0]->spawn();
        // map->draw_vertical_line(13, 4, 4, map_tile_type::brick_left);
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
        // map->draw_horizontal_line(18, 17, 4, map_tile_type::brick_top);
    }
    else if (!enemies[2]->is_alive() && enemies[2]->is_spawned() &&
             !enemies[3]->is_spawned())
        enemies[3]->spawn();
    else if (!enemies[3]->is_alive() && enemies[3]->is_spawned() &&
             !enemies[4]->is_spawned())
        enemies[4]->spawn();
}*/
