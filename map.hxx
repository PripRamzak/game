#pragma once

#include "enemy.hxx"
#include "geometry.hxx"
#include "index_buffer.hxx"
#include "texture.hxx"
#include "vertex_buffer.hxx"

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
    texture*               tile_texture;
    map_tile               type;
    std::vector<vertex_2d> vertices;
    vertex_buffer*         tile_vertex_buffer;
    index_buffer*          tile_index_buffer;
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
    virtual void           delete_tiles_horizontal(int      start_x,
                                                   int      start_y,
                                                   int      length,
                                                   map_tile type)         = 0;
    virtual void           delete_tiles_vertical(int      start_x,
                                                 int      start_y,
                                                 int      length,
                                                 map_tile type)           = 0;
    virtual texture*       get_tile(map_tile type)                        = 0;
    virtual vertex_2d*     get_vertices(map_tile type)                    = 0;
    virtual size_t         get_vertices_num(map_tile type)                = 0;
    virtual vertex_buffer* get_vertex_buffer(map_tile type)               = 0;
    virtual index_buffer*  get_index_buffer(map_tile type)                = 0;
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