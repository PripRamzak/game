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
    virtual void add_tile(texture* tile_texture, map_tile type) = 0;

public:
    static void  initialize();
    virtual void draw_vertical_line(int      start_x,
                                    int      start_y,
                                    int      length,
                                    map_tile type)   = 0;
    virtual void draw_horizontal_line(int      start_x,
                                      int      start_y,
                                      int      length,
                                      map_tile type) = 0;
    virtual void fill_rectangle(
        int start_x, int start_y, int width_, int height_, map_tile type) = 0;
    /*virtual void           delete_tiles_horizontal(int      start_x,
                                                   int      start_y,
                                                   int      length,
                                                   map_tile type)         = 0;
    virtual void           delete_tiles_vertical(int      start_x,
                                                 int      start_y,
                                                 int      length,
                                                 map_tile type)           = 0;*/
    virtual texture*       get_tile(map_tile type)          = 0;
    virtual vertex_buffer* get_vertex_buffer(map_tile type) = 0;
    virtual index_buffer*  get_index_buffer(map_tile type)  = 0;
    virtual ~map();
};

map* create_map(float tile_width, float tile_height);

void generate_level_1(map*                 map,
                      std::vector<enemy*>& enemies,
                      float                window_width,
                      float                window_height);
void game_logic_level_1(map*                 map,
                        game_object*         hero,
                        std::vector<enemy*>& enemies);