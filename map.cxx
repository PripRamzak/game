#include "map.hxx"
#include "enemy.hxx"

#include <algorithm>
#include <iostream>

#include <glm/matrix.hpp>

static texture* floor_      = nullptr;
static texture* wall        = nullptr;
static texture* wall_top    = nullptr;
static texture* wall_left   = nullptr;
static texture* wall_right  = nullptr;
static texture* wall_bottom = nullptr;

void map::initialize()
{
    floor_      = create_texture("./img/floor.png");
    wall        = create_texture("./img/wall.png");
    wall_top    = create_texture("./img/wall_top.png");
    wall_left   = create_texture("./img/wall_left.png");
    wall_right  = create_texture("./img/wall_right.png");
    wall_bottom = create_texture("./img/wall_bottom.png");
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
    }
    void create_tile_vertex_buffer(vertex_buffer* tile_vertex_buffer,
                                   map_tile       type) final
    {
        auto it = find_tile(type);

        if (it != tiles.end())
        {
            tile_vertex_buffer->buffer_data(it->vertices.data(),
                                            it->vertices.size());
        }
        else
            std::cout << "Such tile doesn't exists" << std::endl;
    }
    void draw_vertical_line(int      start_x,
                            int      start_y,
                            int      length,
                            map_tile type) final
    {
        auto it = find_tile(type);

        if (it != tiles.end())
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
    }
    void draw_horizontal_line(int      start_x,
                              int      start_y,
                              int      length,
                              map_tile type) final
    {
        auto it = find_tile(type);

        if (it != tiles.end())
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
    }
    void fill_rectangle(
        int start_x, int start_y, int width_, int height_, map_tile type) final
    {
        auto it = find_tile(type);

        if (it != tiles.end())
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
                }
        else
            std::cout << "Such tile doesn't exists" << std::endl;
    }
    bool check_hero_collision(hero* hero, map_tile type) final
    {
        auto it = find_tile(type);

        if (it != tiles.end())
        {
            const vertex_2d* sprite_vertices = hero->get_vertices();
            float            sprite_width    = hero->get_sprite()->get_width();
            float            sprite_height   = hero->get_sprite()->get_height();
            float            size            = hero->get_size();
            vertex_2d*       map_vertices    = it->vertices.data();
            int              num_vertices    = it->vertices.size();

            float delta_x = hero->get_delta_x();
            float delta_y = hero->get_delta_y();

            for (int i = 0; i < num_vertices / 4; i++, map_vertices += 4)
            {
                bool collision_x =
                    sprite_vertices[2].x + sprite_width / 2.f * (size - 1) >=
                        map_vertices->x - delta_x &&
                    (map_vertices + 2)->x - delta_x >=
                        sprite_vertices[0].x - sprite_width / 2.f * (size - 1);
                bool collision_y =
                    sprite_vertices[2].y + sprite_height / 2.f * (size - 1) >=
                        map_vertices->y - delta_y &&
                    (map_vertices + 2)->y - delta_y >=
                        sprite_vertices[0].y - sprite_height / 2.f * (size - 1);

                if (collision_x && collision_y)
                    return true;
            }
        }

        return false;
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
    map->fill_rectangle(4, 4, 10, 5, map_tile::floor);
    map->draw_horizontal_line(4, 3, 10, map_tile::wall);
    map->draw_horizontal_line(4, 2, 10, map_tile::wall_top);
    map->draw_horizontal_line(4, 9, 10, map_tile::wall);
    map->draw_horizontal_line(4, 9, 10, map_tile::wall_bottom);
    map->draw_vertical_line(3, 3, 6, map_tile::wall_left);
    map->draw_vertical_line(14, 3, 1, map_tile::wall_right);
    map->draw_vertical_line(14, 8, 1, map_tile::wall_right);

    // corridor from first room to second
    map->fill_rectangle(14, 5, 6, 3, map_tile::floor);
    map->draw_horizontal_line(14, 4, 6, map_tile::wall);
    map->draw_horizontal_line(14, 3, 6, map_tile::wall_top);
    map->draw_horizontal_line(14, 8, 6, map_tile::wall_bottom);

    // second room
    map->fill_rectangle(20, 3, 15, 9, map_tile::floor);
    map->draw_horizontal_line(20, 2, 15, map_tile::wall);
    map->draw_horizontal_line(20, 1, 15, map_tile::wall_top);
    map->draw_horizontal_line(20, 12, 15, map_tile::wall_bottom);
    map->draw_vertical_line(19, 2, 2, map_tile::wall_left);
    map->draw_vertical_line(19, 8, 4, map_tile::wall_left);
    map->draw_vertical_line(35, 2, 10, map_tile::wall_right);

    enemy* skeleton_1 = create_enemy(2,
                                     window_width / 2.f,
                                     window_height / 2.f,
                                     2100.f,
                                     300.f,
                                     1.75f,
                                     game_object_state::run);
    enemies.push_back(skeleton_1);
}

void game_logic_level_1(hero* hero, std::vector<enemy*>& enemies)
{
    if (hero->get_current_pos_x() > 1300.f && !enemies[0]->is_alive())
        enemies[0]->spawn();
}