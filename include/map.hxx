#pragma once

#include "engine/include/geometry.hxx"
#include "engine/include/index_buffer.hxx"
#include "engine/include/texture.hxx"
#include "engine/include/vertex_buffer.hxx"
#include "include/enemy.hxx"

#include <vector>

enum class map_tile_type
{
    wall,
    brick_top,
    brick_bottom,
    brick_left,
    brick_right,
    wall_corner_top_left,
    wall_corner_top_right,
    wall_corner_bottom_left,
    wall_corner_bottom_right,
    coin
};

class map
{
public:
    map(float tile_width_, float tile_height_);
    static void initialize();
    void        draw_vertical_line(int           start_x,
                                   int           start_y,
                                   int           length,
                                   map_tile_type type);
    void        draw_horizontal_line(int           start_x,
                                     int           start_y,
                                     int           length,
                                     map_tile_type type);
    void        fill_rectangle(
               int start_x, int start_y, int width_, int height_, map_tile_type type);
    /*virtual void           delete_tiles_horizontal(int      start_x,
                                                   int      start_y,
                                                   int      length,
                                                   map_tile type)         = 0;
    virtual void           delete_tiles_vertical(int      start_x,
                                                 int      start_y,
                                                 int      length,
                                                 map_tile type)           = 0;*/
    texture*       get_tileset();
    vertex_buffer* get_vertex_buffer(map_tile_type type);
    index_buffer*  get_index_buffer(map_tile_type type);
    float*         get_tile_min_uv(map_tile_type type);
    float*         get_tile_max_uv(map_tile_type type);
    ~map();

private:
    struct tile
    {
        map_tile_type  type;
        vertex_buffer* tile_vertex_buffer;
        index_buffer*  tile_index_buffer;
        float          min_uv[2];
        float          max_uv[2];

        tile(map_tile_type type,
             float         min_u,
             float         max_u,
             float         min_v,
             float         max_v);
    };

    void add_tile(
        map_tile_type type, float min_u, float max_u, float min_v, float max_v);
    auto find_tile(map_tile_type type) -> std::vector<tile>::iterator;

    texture*          tileset = nullptr;
    std::vector<tile> tiles;
    float             tile_width  = 0;
    float             tile_height = 0;
};

void generate_level_1(map*                 map,
                      std::vector<enemy*>& enemies,
                      float                window_width,
                      float                window_height);
void game_logic_level_1(map*                 map,
                        game_object*         hero,
                        std::vector<enemy*>& enemies);