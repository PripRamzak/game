#pragma once

#include "engine/include/geometry.hxx"
#include "engine/include/index_buffer.hxx"
#include "engine/include/texture.hxx"
#include "engine/include/vertex_buffer.hxx"
#include "include/enemy.hxx"

#include <vector>

enum class map_tile
{
    floor,
    wall,
    wall_top,
    wall_left,
    wall_right,
    wall_bottom,
    wall_corner_top_left,
    wall_corner_top_right,
    wall_corner_bottom_left,
    wall_corner_bottom_right,
    coin
};

struct tile
{
    texture*       tile_texture;
    map_tile       type;
    vertex_buffer* tile_vertex_buffer;
    index_buffer*  tile_index_buffer;
};

class map
{
public:
    map(float tile_width_, float tile_height_);
    static void initialize();
    void        draw_vertical_line(int      start_x,
                                   int      start_y,
                                   int      length,
                                   map_tile type);
    void        draw_horizontal_line(int      start_x,
                                     int      start_y,
                                     int      length,
                                     map_tile type);
    void        fill_rectangle(
               int start_x, int start_y, int width_, int height_, map_tile type);
    /*virtual void           delete_tiles_horizontal(int      start_x,
                                                   int      start_y,
                                                   int      length,
                                                   map_tile type)         = 0;
    virtual void           delete_tiles_vertical(int      start_x,
                                                 int      start_y,
                                                 int      length,
                                                 map_tile type)           = 0;*/
    texture*       get_tile(map_tile type);
    vertex_buffer* get_vertex_buffer(map_tile type);
    index_buffer*  get_index_buffer(map_tile type);
    ~map();

private:
    void add_tile(texture* tile_texture, map_tile type);
    auto find_tile(map_tile type) -> std::vector<tile>::iterator;

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