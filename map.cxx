#include "map.hxx"

#include <algorithm>
#include <iostream>

#include <glm/matrix.hpp>

class game_map final : public map
{
    float             tile_width  = 0;
    float             tile_height = 0;
    std::vector<tile> tiles;

public:
    game_map(float tile_width_, float tile_height_)
        : tile_width(tile_width_)
        , tile_height(tile_height_)
    {
    }
    void add_tile(texture* tile_texture, map_tile type) final
    {
        auto it =
            std::find_if(tiles.begin(),
                         tiles.end(),
                         [&](const tile tile) { return tile.type == type; });

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
    void create_tile_vertex_buffer(vertex_buffer* tile_vertex_buffer,
                                   map_tile       type) final
    {
        auto it =
            std::find_if(tiles.begin(),
                         tiles.end(),
                         [&](const tile tile) { return tile.type == type; });

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
        auto it =
            std::find_if(tiles.begin(),
                         tiles.end(),
                         [&](const tile tile) { return tile.type == type; });

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
        auto it =
            std::find_if(tiles.begin(),
                         tiles.end(),
                         [&](const tile tile) { return tile.type == type; });

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
        auto it =
            std::find_if(tiles.begin(),
                         tiles.end(),
                         [&](const tile tile) { return tile.type == type; });

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
    bool check_collision(sprite* sprite, map_tile type) final
    {
        /*vertex_2d* sprite_vertices = sprite->get_vertices();

        auto it =
            std::find_if(tiles.begin(),
                         tiles.end(),
                         [&](const tile tile) { return tile.type == type; });

        if (it != tiles.end())
        {
            vertex_2d* map_vertices = it->vertices.data();
            int        num_vertices = it->vertices.size();

            float delta_x = 0;
            float delta_y = 0;
            sprite->get_delta(delta_x, delta_y);*/

        // Yes it's wrong collision detection, but it's used for my
        // sprite_sheet hero with transparent pixels
        /*if (type == map_tile::wall_left)
            for (int i = 0; i < num_vertices / 4; i++, map_vertices += 4)
            {
                bool collision_x =
                    sprite_vertices[2].x >= map_vertices->x - delta_x &&
                    (map_vertices + 3)->x - delta_x >= sprite_vertices[0].x;
                bool collision_y =
                    sprite_vertices[2].y <= map_vertices->y - delta_y &&
                    (map_vertices + 3)->y - delta_y <= sprite_vertices[0].y;

                if (collision_x && collision_y)
                    return true;
            }
        else if (type == map_tile::wall_right)
            for (int i = 0; i < num_vertices / 4; i++, map_vertices += 4)
            {
                bool collision_x =
                    sprite_vertices[2].x >=
                        (map_vertices + 1)->x - delta_x &&
                    (map_vertices + 2)->x - delta_x >= sprite_vertices[0].x;
                bool collision_y =
                    sprite_vertices[2].y <=
                        (map_vertices + 1)->y - delta_y &&
                    (map_vertices + 2)->y - delta_y <= sprite_vertices[0].y;

                if (collision_x && collision_y)
                    return true;
            }
        else*/
        /*for (int i = 0; i < num_vertices / 4; i++, map_vertices += 4)
        {
            bool collision_x =
                sprite_vertices[2].x >= map_vertices->x - delta_x &&
                (map_vertices + 2)->x - delta_x >= sprite_vertices[0].x;
            bool collision_y =
                sprite_vertices[2].y <= map_vertices->y - delta_y &&
                (map_vertices + 2)->y - delta_y <= sprite_vertices[0].y;

            if (collision_x && collision_y)
                return true;
        }
    }*/

        return false;
    }
    texture* get_tile(map_tile type) final
    {
        auto it =
            std::find_if(tiles.begin(),
                         tiles.end(),
                         [&](const tile tile) { return tile.type == type; });

        if (it != tiles.end())
            return it->tile_texture;
        else
            std::cout << "Such tile doesn't exists" << std::endl;

        return nullptr;
    }
};

map::~map() = default;

void generate_map(map* map)
{
    map->fill_rectangle(4, 4, 10, 5, map_tile::floor);
    map->draw_horizontal_line(4, 3, 10, map_tile::wall);
    map->draw_horizontal_line(4, 2, 10, map_tile::wall_top);
    map->draw_horizontal_line(4, 9, 10, map_tile::wall_bottom);
    map->draw_vertical_line(3, 3, 6, map_tile::wall_left);
    map->draw_vertical_line(14, 3, 6, map_tile::wall_right);
}

map* create_map(float tile_width, float tile_height)
{
    return new game_map(tile_width, tile_height);
}