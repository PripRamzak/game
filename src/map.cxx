#include "engine/include/engine.hxx"
#include "engine/include/memory_buf.hxx"

#include "include/camera.hxx"
#include "include/map.hxx"
#include "include/resources.hxx"

#include <algorithm>
#include <iostream>

#include "glm/gtc/type_ptr.hpp"

std::istream& operator>>(std::istream& is, map_tile_type& type)
{
    int type_num;
    is >> type_num;
    type = static_cast<map_tile_type>(type_num);
    return is;
}

map::map(float tile_width_, float tile_height_, std::string file_path)
    : tileset(resources::dungeon)
    , tile_width(tile_width_)
    , tile_height(tile_height_)
{
    tile* brick_top          = new tile{ { 32.f / 240.f, 224.f / 288.f },
                                         { 48.f / 240.f, 240.f / 288.f } };
    tile* brick_bottom       = new tile{ { 32.f / 240.f, 256.f / 288.f },
                                         { 48.f / 240.f, 272.f / 288.f } };
    tile* brick_left         = new tile{ { 48.f / 240.f, 240.f / 288.f },
                                         { 64.f / 240.f, 256.f / 288.f } };
    tile* brick_right        = new tile{ { 16.f / 240.f, 240.f / 288.f },
                                         { 32.f / 240.f, 256.f / 288.f } };
    tile* brick_top_left     = new tile{ { 96.f / 240.f, 224.f / 288.f },
                                         { 112.f / 240.f, 240.f / 288.f } };
    tile* brick_top_right    = new tile{ { 80.f / 240.f, 224.f / 288.f },
                                         { 96.f / 240.f, 240.f / 288.f } };
    tile* brick_bottom_left  = new tile{ { 96.f / 240.f, 240.f / 288.f },
                                         { 112.f / 240.f, 256.f / 288.f } };
    tile* brick_bottom_right = new tile{ { 80.f / 240.f, 240.f / 288.f },
                                         { 96.f / 240.f, 256.f / 288.f } };
    tile* plate_top_left     = new tile{ { 48.f / 240.f, 160.f / 288.f },
                                         { 64.f / 240.f, 172.f / 288.f } };
    tile* plate_top_right    = new tile{ { 16.f / 240.f, 160.f / 288.f },
                                         { 32.f / 240.f, 172.f / 288.f } };
    tile* plate_bottom_left  = new tile{ { 48.f / 240.f, 192.f / 288.f },
                                         { 64.f / 240.f, 208.f / 288.f } };
    tile* plate_bottom_right = new tile{ { 16.f / 240.f, 192.f / 288.f },
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

    prip_engine::memory_buf buf_level = prip_engine::load_file(file_path);
    std::istream            data_level(&buf_level);
    int                     method_to_draw_number;
    float                   start_x, start_y;
    int                     length;
    map_tile_type           type;

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
        tile.second->vao->bind();

        prip_engine::vertex_buffer* vbo = tile.second->vao->get_vertex_buffer();

        vbo->buffer_data(tile.second->vertices.data(),
                         tile.second->vertices.size());
        vbo->set_attrib_pointer(0, 2, sizeof(prip_engine::vertex2d_uv), 0);
        vbo->set_attrib_pointer(1,
                                2,
                                sizeof(prip_engine::vertex2d_uv),
                                offsetof(prip_engine::vertex2d_uv, uv));

        tile.second->vao->get_index_buffer()->add_indexes(
            prip_engine::primitives::triangle,
            tile.second->vertices.size() / 4);
    }
}

void map::draw()
{
    glm::mat4 projection      = glm::make_mat4x4(camera::get_projection());
    glm::mat4 view            = glm::make_mat4x4(camera::get_view());
    glm::mat4 projection_view = projection * view;

    for (auto& tile : tiles)
        prip_engine::render(tileset,
                            tile.second->vao,
                            tile.second->min_uv,
                            tile.second->max_uv,
                            &projection_view[0][0]);
}

prip_engine::texture* map::get_tileset()
{
    return tileset;
}

std::vector<prip_engine::vertex2d_uv>& map::get_vertices(map_tile_type type)
{
    return tiles[type]->vertices;
}

prip_engine::transform2d map::get_tile_min_uv(map_tile_type type)
{
    return tiles[type]->min_uv;
}

prip_engine::transform2d map::get_tile_max_uv(map_tile_type type)
{
    return tiles[type]->max_uv;
}

map::tile::tile()
{
    min_uv.x = 0.f;
    min_uv.y = 0.f;
    max_uv.x = 1.f;
    max_uv.y = 1.f;

    vao = prip_engine::create_vertex_array(prip_engine::create_vertex_buffer(),
                                           prip_engine::create_index_buffer());
};

map::tile::tile(prip_engine::transform2d min_uv,
                prip_engine::transform2d max_uv)
    : min_uv(min_uv)
    , max_uv(max_uv)
{
    vao = prip_engine::create_vertex_array(prip_engine::create_vertex_buffer(),
                                           prip_engine::create_index_buffer());
}

void map::draw_vertical_line(float         start_x,
                             float         start_y,
                             int           length,
                             map_tile_type type)
{
    tile* tile = tiles[type];
    tile->vertices.push_back(
        { start_x, start_y, 0.f, static_cast<float>(length) });
    tile->vertices.push_back(
        { start_x + tile_width, start_y, 1.f, static_cast<float>(length) });
    tile->vertices.push_back(
        { start_x + tile_width, start_y + length * tile_height, 1.f, 0.f });
    tile->vertices.push_back(
        { start_x, start_y + length * tile_width, 0.f, 0.f });
}

void map::draw_horizontal_line(float         start_x,
                               float         start_y,
                               int           length,
                               map_tile_type type)
{
    tile* tile = tiles[type];
    tile->vertices.push_back({ start_x, start_y, 0.f, 1.f });
    tile->vertices.push_back({ start_x + length * tile_width,
                               start_y,
                               static_cast<float>(length),
                               1.f });
    tile->vertices.push_back({ start_x + length * tile_width,
                               start_y + tile_height,
                               static_cast<float>(length),
                               0.f });
    tile->vertices.push_back({ start_x, start_y + tile_height, 0.f, 0.f });
}

void map::fill_rectangle(
    float start_x, float start_y, int width, int height, map_tile_type type)
{
    tile* tile = tiles[type];
    tile->vertices.push_back(
        { start_x, start_y, 0.f, static_cast<float>(height) });
    tile->vertices.push_back({ start_x + width * tile_width,
                               start_y,
                               static_cast<float>(width),
                               static_cast<float>(height) });
    tile->vertices.push_back({ start_x + width * tile_width,
                               start_y + height * tile_height,
                               static_cast<float>(width),
                               0.f });
    tile->vertices.push_back(
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
        delete tile.second->vao;
    }
}