#include "map.hxx"

#include <algorithm>
#include <iostream>

class game_map final : public map
{
    int width  = 0;
    int height = 0;
    // std::vector<texture*> tiles;
    int                  current_tile = 0;
    map_tile**           tiles_positions;
    int                  tile_width  = 0;
    int                  tile_height = 0;
    std::vector<tile>    tiles;
    std::vector<sprite*> tiles_sprite;
    std::vector<int>     tiles_sprite_indexes;

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

            for (int i = 0; i < height; i++)
                for (int j = 0; j < width; j++)
                    if (type == tiles_positions[i][j])
                    {
                        vertex_2d v1 = { static_cast<float>(j) * tile_width,
                                         static_cast<float>(i) * tile_height,
                                         0.f,
                                         1.f };
                        vertex_2d v2 = { static_cast<float>(j + 1) * tile_width,
                                         static_cast<float>(i) * tile_height,
                                         1.f,
                                         1.f };
                        vertex_2d v3 = { static_cast<float>(j + 1) * tile_width,
                                         static_cast<float>(i - 1) *
                                             tile_height,
                                         1.f,
                                         0.f };
                        vertex_2d v4 = { static_cast<float>(j) * tile_width,
                                         static_cast<float>(i - 1) *
                                             tile_height,
                                         0.f,
                                         0.f };

                        tile.vertices.push_back(v1);
                        tile.vertices.push_back(v2);
                        tile.vertices.push_back(v3);
                        tile.vertices.push_back(v4);
                    }

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
            tile_vertex_buffer->buffer_data(it->vertices.data(),
                                            it->vertices.size());
        else
            std::cout << "Such tile doesn't exists" << std::endl;
    }
    void fill_rectangle(
        int start_x, int start_y, int width_, int height_, map_tile type) final
    {
        for (int i = start_y; i < start_y + height_; i++)
            for (int j = start_x; j < start_x + width_; j++)
                tiles_positions[i][j] = type;
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