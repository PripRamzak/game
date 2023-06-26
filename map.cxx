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
    bool check_collision(game_object* hero,
                         map_tile     type,
                         float        window_width,
                         float        window_height) final
    {

        auto it =
            std::find_if(tiles.begin(),
                         tiles.end(),
                         [&](const tile tile) { return tile.type == type; });

        if (it != tiles.end())
        {
            const vertex_2d* sprite_vertices = hero->get_vertices();
            float            size            = hero->get_size();
            vertex_2d*       map_vertices    = it->vertices.data();
            int              num_vertices    = it->vertices.size();

            float delta_x = 0;
            float delta_y = 0;
            hero->get_delta_pos(delta_x, delta_y);

            vertex_2d vertex_delta = { delta_x + window_width / 2,
                                       delta_y + window_height / 2,
                                       0.f,
                                       0.f };

            for (int i = 0; i < num_vertices / 4; i++, map_vertices += 4)
            {
                bool collision_x = sprite_vertices[2].x * size >=
                                       map_vertices->x - vertex_delta.x &&
                                   (map_vertices + 2)->x - vertex_delta.x >=
                                       sprite_vertices[0].x * size;
                bool collision_y = sprite_vertices[2].y * size <=
                                       map_vertices->y - vertex_delta.y &&
                                   (map_vertices + 2)->y - vertex_delta.y <=
                                       sprite_vertices[0].y * size;

                if (collision_x && collision_y)
                    return true;
            }
        }

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
    // first room
    map->fill_rectangle(4, 4, 10, 5, map_tile::floor);
    map->draw_horizontal_line(4, 3, 10, map_tile::wall);
    map->draw_horizontal_line(4, 2, 10, map_tile::wall_top);
    map->draw_horizontal_line(4, 9, 10, map_tile::wall_bottom);
    map->draw_vertical_line(3, 3, 6, map_tile::wall_left);
    map->draw_vertical_line(14, 3, 1, map_tile::wall_right);
    map->draw_vertical_line(14, 8, 1, map_tile::wall_right);

    // corridor from fist room to second
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
}

map* create_map(float tile_width, float tile_height)
{
    return new game_map(tile_width, tile_height);
}