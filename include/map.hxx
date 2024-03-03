#pragma once

#include "engine/include/geometry.hxx"
#include "engine/include/index_buffer.hxx"
#include "engine/include/texture.hxx"
#include "engine/include/vertex_buffer.hxx"

#include <unordered_map>

enum class map_tile_type
{
    brick_top,
    brick_bottom,
    brick_left,
    brick_right,
    brick_top_left,
    brick_top_right,
    brick_bottom_left,
    brick_bottom_right,
    plate_top_left,
    plate_top_right,
    plate_bottom_left,
    plate_bottom_right,
    coin
};

std::istream& operator>>(std::istream& is, map_tile_type& type);

class map
{
public:
    map(float tile_width_, float tile_height_, std::string file_path);
    void                                   draw(float* matrix);
    prip_engine::texture*                  get_tileset();
    std::vector<prip_engine::vertex2d_uv>& get_vertices(map_tile_type type);
    prip_engine::vertex_buffer* get_vertex_buffer(map_tile_type type);
    prip_engine::index_buffer*  get_index_buffer(map_tile_type type);
    prip_engine::transform2d    get_tile_min_uv(map_tile_type type);
    prip_engine::transform2d    get_tile_max_uv(map_tile_type type);
    ~map();

private:
    struct tile
    {
        prip_engine::transform2d              min_uv;
        prip_engine::transform2d              max_uv;
        std::vector<prip_engine::vertex2d_uv> vertices;
        prip_engine::vertex_buffer*           tile_vertex_buffer;
        prip_engine::index_buffer*            tile_index_buffer;

        tile();
        tile(prip_engine::transform2d min_uv, prip_engine::transform2d max_uv);
    };

    void draw_vertical_line(float         start_x,
                            float         start_y,
                            int           length,
                            map_tile_type type);
    void draw_horizontal_line(float         start_x,
                              float         start_y,
                              int           length,
                              map_tile_type type);
    void fill_rectangle(float         start_x,
                        float         start_y,
                        int           width,
                        int           height,
                        map_tile_type type);
    /*virtual void           delete_tiles_horizontal(int      start_x,
                                                   int      start_y,
                                                   int      length,
                                                   map_tile type)         = 0;
    virtual void           delete_tiles_vertical(int      start_x,
                                                 int      start_y,
                                                 int      length,
                                                 map_tile type)           = 0;*/

    prip_engine::texture*                    tileset = nullptr;
    std::unordered_map<map_tile_type, tile*> tiles;
    float                                    tile_width  = 0;
    float                                    tile_height = 0;
};