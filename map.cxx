#include "map.hxx"

#include <algorithm>
#include <iostream>

#include <glm/matrix.hpp>

class game_map final : public map
{
    int               width  = 0;
    int               height = 0;
    map_tile**        tiles_positions;
    int               tile_width  = 0;
    int               tile_height = 0;
    std::vector<tile> tiles;

public:
    game_map(int width_, int height_, int tile_width_, int tile_height_)
        : width(width_)
        , height(height_)
        , tile_width(tile_width_)
        , tile_height(tile_height_)
    {
        tiles_positions = new map_tile*[height];
        for (int i = 0; i < height; i++)
            tiles_positions[i] = new map_tile[width];

        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++)
                tiles_positions[i][j] = map_tile::null_tile;
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
    void fill_rectangle(int      start_x,
                        int      start_y,
                        int      width_,
                        int      height_,
                        float    window_width,
                        float    window_height,
                        map_tile type) final
    {
        auto it =
            std::find_if(tiles.begin(),
                         tiles.end(),
                         [&](const tile tile) { return tile.type == type; });

        if (it != tiles.end())
        {
            for (int i = start_y; i < start_y + height_; i++)
                for (int j = start_x; j < start_x + width_; j++)
                {
                    tiles_positions[i][j] = type;

                    glm::vec3 position = { static_cast<float>(j) * tile_width *
                                               2.f / window_width,
                                           static_cast<float>(i) * tile_height *
                                               2.f / window_height,
                                           1.f };

                    glm::vec3 size = { tile_width * 2.f / window_width,
                                       tile_height * 2.f / window_height,
                                       1.f };

                    vertex_2d v1 = { position.x, position.y, 0.f, 1.f };
                    vertex_2d v2 = {
                        position.x + size.x, position.y, 1.f, 1.f
                    };
                    vertex_2d v3 = {
                        position.x + size.x, position.y - size.y, 1.f, 0.f
                    };
                    vertex_2d v4 = {
                        position.x, position.y - size.y, 0.f, 0.f
                    };

                    it->vertices.push_back(v1);
                    it->vertices.push_back(v2);
                    it->vertices.push_back(v3);
                    it->vertices.push_back(v4);
                }
        }
        else
            std::cout << "Such tile doesn't exists" << std::endl;
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

map* create_map(int width, int height, int tile_width, int tile_height)
{
    return new game_map(width, height, tile_width, tile_height);
}