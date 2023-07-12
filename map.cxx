#include "map.hxx"
#include "enemy.hxx"

#include <algorithm>
#include <iostream>

#include <glm/matrix.hpp>

static texture* floor_                   = nullptr;
static texture* wall                     = nullptr;
static texture* wall_top                 = nullptr;
static texture* wall_left                = nullptr;
static texture* wall_right               = nullptr;
static texture* wall_bottom              = nullptr;
static texture* wall_top_corner_left     = nullptr;
static texture* wall_top_corner_right    = nullptr;
static texture* wall_bottom_corner_left  = nullptr;
static texture* wall_bottom_corner_right = nullptr;

void map::initialize()
{
    floor_                  = create_texture("img/floor.png");
    wall                    = create_texture("img/wall.png");
    wall_top                = create_texture("img/wall_top.png");
    wall_left               = create_texture("img/wall_left.png");
    wall_right              = create_texture("img/wall_right.png");
    wall_bottom             = create_texture("img/wall_bottom.png");
    wall_top_corner_left    = create_texture("img/wall_corner_top_left.png");
    wall_top_corner_right   = create_texture("img/wall_corner_top_right.png");
    wall_bottom_corner_left = create_texture("img/wall_corner_bottom_left.png");
    wall_bottom_corner_right =
        create_texture("img/wall_corner_bottom_right.png");
}

class game_map final : public map
{
    float             tile_width  = 0;
    float             tile_height = 0;
    std::vector<tile> tiles;

    auto find_tile(map_tile type)
    {
        return std::find_if(tiles.begin(),
                            tiles.end(),
                            [&](const tile tile) { return tile.type == type; });
    }
    void add_tile(texture* tile_texture, map_tile type) final
    {
        auto it = find_tile(type);

        if (it != tiles.end())
        {
            std::cout << "This tile already exists" << std::endl;
            return;
        }
        else
        {
            tile tile;
            tile.type         = type;
            tile.tile_texture = tile_texture;
            tiles.push_back(tile);
            tiles[tiles.size() - 1].tile_vertex_buffer = create_vertex_buffer();
            tiles[tiles.size() - 1].tile_index_buffer  = create_index_buffer();
        }
    }

public:
    game_map(float tile_width_, float tile_height_)
        : tile_width(tile_width_)
        , tile_height(tile_height_)
    {
        add_tile(floor_, map_tile::floor);
        add_tile(wall, map_tile::wall);
        add_tile(wall_top, map_tile::wall_top);
        add_tile(wall_left, map_tile::wall_left);
        add_tile(wall_right, map_tile::wall_right);
        add_tile(wall_bottom, map_tile::wall_bottom);
        add_tile(wall_top_corner_left, map_tile::wall_corner_top_left);
        add_tile(wall_top_corner_right, map_tile::wall_corner_top_right);
        add_tile(wall_bottom_corner_left, map_tile::wall_corner_bottom_left);
        add_tile(wall_bottom_corner_right, map_tile::wall_corner_bottom_right);
    }
    void draw_vertical_line(int      start_x,
                            int      start_y,
                            int      length,
                            map_tile type) final
    {
        auto it = find_tile(type);

        if (it != tiles.end())
        {
            for (int i = start_y - 1; i < start_y + length - 1; i++)
            {
                vertex_2d v1 = { (static_cast<float>(start_x) - 1.f) *
                                     tile_width,
                                 static_cast<float>(i) * tile_height,
                                 0.f,
                                 1.f };
                vertex_2d v2 = { static_cast<float>(start_x) * tile_width,
                                 static_cast<float>(i) * tile_height,
                                 1.f,
                                 1.f };
                vertex_2d v3 = { static_cast<float>(start_x) * tile_width,
                                 (static_cast<float>(i) + 1.f) * tile_height,
                                 1.f,
                                 0.f };
                vertex_2d v4 = { (static_cast<float>(start_x) - 1.f) *
                                     tile_width,
                                 (static_cast<float>(i) + 1.f) * tile_height,
                                 0.f,
                                 0.f };

                it->vertices.push_back(v1);
                it->vertices.push_back(v2);
                it->vertices.push_back(v3);
                it->vertices.push_back(v4);
            }
            it->tile_vertex_buffer->buffer_data(it->vertices.data(),
                                                it->vertices.size());
            it->tile_index_buffer->add_indexes(static_cast<size_t>(length * 4));
        }
    }
    void draw_horizontal_line(int      start_x,
                              int      start_y,
                              int      length,
                              map_tile type) final
    {
        auto it = find_tile(type);

        if (it != tiles.end())
        {
            for (int i = start_x - 1; i < start_x + length - 1; i++)
            {
                vertex_2d v1 = { static_cast<float>(i) * tile_width,
                                 (static_cast<float>(start_y) - 1.f) *
                                     tile_height,
                                 0.f,
                                 1.f };
                vertex_2d v2 = { (static_cast<float>(i) + 1.f) * tile_width,
                                 (static_cast<float>(start_y) - 1.f) *
                                     tile_height,
                                 1.f,
                                 1.f };
                vertex_2d v3 = { (static_cast<float>(i) + 1.f) * tile_width,
                                 static_cast<float>(start_y) * tile_height,
                                 1.f,
                                 0.f };
                vertex_2d v4 = { static_cast<float>(i) * tile_width,
                                 static_cast<float>(start_y) * tile_height,
                                 0.f,
                                 0.f };

                it->vertices.push_back(v1);
                it->vertices.push_back(v2);
                it->vertices.push_back(v3);
                it->vertices.push_back(v4);
            }
            it->tile_vertex_buffer->buffer_data(it->vertices.data(),
                                                it->vertices.size());
            it->tile_index_buffer->add_indexes(static_cast<size_t>(length * 4));
        }
    }
    void fill_rectangle(
        int start_x, int start_y, int width_, int height_, map_tile type) final
    {
        auto it = find_tile(type);

        if (it != tiles.end())
        {
            for (int i = start_y - 1; i < start_y + height_ - 1; i++)
                for (int j = start_x - 1; j < start_x + width_ - 1; j++)
                {
                    vertex_2d v1 = { static_cast<float>(j) * tile_width,
                                     static_cast<float>(i) * tile_height,
                                     0.f,
                                     1.f };
                    vertex_2d v2 = { (static_cast<float>(j) + 1.f) * tile_width,
                                     static_cast<float>(i) * tile_height,
                                     1.f,
                                     1.f };
                    vertex_2d v3 = { (static_cast<float>(j) + 1.f) * tile_width,
                                     (static_cast<float>(i) + 1.f) *
                                         tile_height,
                                     1.f,
                                     0.f };
                    vertex_2d v4 = { static_cast<float>(j) * tile_width,
                                     (static_cast<float>(i) + 1.f) *
                                         tile_height,
                                     0.f,
                                     0.f };

                    it->vertices.push_back(v1);
                    it->vertices.push_back(v2);
                    it->vertices.push_back(v3);
                    it->vertices.push_back(v4);

                    it->tile_vertex_buffer->buffer_data(it->vertices.data(),
                                                        it->vertices.size());
                }
            it->tile_vertex_buffer->buffer_data(it->vertices.data(),
                                                it->vertices.size());
            it->tile_index_buffer->add_indexes(
                static_cast<size_t>(width_ * height_ * 4));
        }
        else
            std::cout << "Such tile doesn't exists" << std::endl;
    }
    void delete_tiles_horizontal(int      start_x,
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
    }
    texture* get_tile(map_tile type) final
    {
        auto it = find_tile(type);

        if (it != tiles.end())
            return it->tile_texture;
        else
            std::cout << "Such tile doesn't exists" << std::endl;

        return nullptr;
    }
    vertex_2d* get_vertices(map_tile type) final
    {
        auto it = find_tile(type);

        if (it != tiles.end())
            return it->vertices.data();
        else
            std::cout << "Such tile doesn't exists" << std::endl;

        return nullptr;
    }
    size_t get_vertices_num(map_tile type) final
    {
        auto it = find_tile(type);

        if (it != tiles.end())
            return it->vertices.size();
        else
            std::cout << "Such tile doesn't exists" << std::endl;

        return 0;
    }
    vertex_buffer* get_vertex_buffer(map_tile type) final
    {
        auto it = find_tile(type);

        if (it != tiles.end())
            return it->tile_vertex_buffer;
        else
            std::cout << "Such tile doesn't exists" << std::endl;

        return nullptr;
    }
    index_buffer* get_index_buffer(map_tile type) final
    {
        auto it = find_tile(type);

        if (it != tiles.end())
            return it->tile_index_buffer;
        else
            std::cout << "Such tile doesn't exists" << std::endl;

        return nullptr;
    }
};

map::~map() = default;

map* create_map(float tile_width, float tile_height)
{
    return new game_map(tile_width, tile_height);
}

void generate_level_1(map*                 map,
                      std::vector<enemy*>& enemies,
                      float                window_width,
                      float                window_height)
{
    // first room
    map->fill_rectangle(4, 4, 6, 5, map_tile::floor);
    map->draw_horizontal_line(4, 3, 6, map_tile::wall);
    map->draw_horizontal_line(4, 2, 6, map_tile::wall_top);
    map->draw_horizontal_line(3, 2, 1, map_tile::wall_corner_top_left);
    map->draw_horizontal_line(10, 2, 1, map_tile::wall_corner_top_right);
    map->draw_horizontal_line(4, 9, 6, map_tile::wall);
    map->draw_horizontal_line(4, 9, 6, map_tile::wall_bottom);
    map->draw_horizontal_line(3, 9, 1, map_tile::wall_corner_bottom_left);
    map->draw_horizontal_line(10, 9, 1, map_tile::wall_corner_bottom_right);
    map->draw_vertical_line(3, 3, 6, map_tile::wall_left);
    map->draw_vertical_line(10, 3, 1, map_tile::wall_right);
    map->draw_vertical_line(10, 8, 1, map_tile::wall_right);

    // corridor from first room to second
    map->fill_rectangle(10, 5, 4, 3, map_tile::floor);
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

    enemy* skeleton_1 = create_enemy(5,
                                     5.f,
                                     window_width / 2.f,
                                     window_height / 2.f,
                                     1500.f,
                                     250.f,
                                     1.75f,
                                     game_object_state::run,
                                     enemy_type::spearman);
    enemies.push_back(skeleton_1);
    enemy* skeleton_2 = create_enemy(5,
                                     5.f,
                                     window_width / 2.f,
                                     window_height / 2.f,
                                     1000.f,
                                     650.f,
                                     1.75f,
                                     game_object_state::run,
                                     enemy_type::warrior);
    enemies.push_back(skeleton_2);
}

void game_logic_level_1(map*                 map,
                        game_object*         hero,
                        std::vector<enemy*>& enemies)
{
    static bool second_room_cleaned = false;

    if (hero->get_current_pos_x() > 900.f && !enemies[0]->is_spawned())
    {
        enemies[0]->spawn();
        map->draw_vertical_line(13, 4, 4, map_tile::wall_left);
    }
    else if (!enemies[0]->is_alive() && enemies[0]->is_spawned() &&
             !enemies[1]->is_spawned())
        enemies[1]->spawn();
    else if (enemies[1]->is_spawned() && !enemies[1]->is_alive() &&
             !second_room_cleaned)
    {
        map->delete_tiles_vertical(13, 4, 4, map_tile::wall_left);
        map->delete_tiles_horizontal(18, 12, 4, map_tile::wall_bottom);
        second_room_cleaned = true;
    }
}