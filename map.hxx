#pragma once

#include "game_object.hxx"
#include "geometry.hxx"
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
    wall_bottom
};

struct tile
{
    texture*               tile_texture;
    std::vector<vertex_2d> vertices;
    map_tile               type;
};

class map
{
public:
    virtual void add_tile(texture* tile_texture, map_tile type) = 0;
    virtual void create_tile_vertex_buffer(vertex_buffer* tile_vertex_buffer,
                                           map_tile       type)       = 0;
    virtual void draw_vertical_line(int      start_x,
                                    int      start_y,
                                    int      length,
                                    map_tile type)              = 0;
    virtual void draw_horizontal_line(int      start_x,
                                      int      start_y,
                                      int      length,
                                      map_tile type)            = 0;
    virtual void fill_rectangle(
        int start_x, int start_y, int width_, int height_, map_tile type) = 0;
    virtual bool     check_collision(game_object* hero, map_tile type)    = 0;
    virtual texture* get_tile(map_tile type)                              = 0;
    ~map();
};

void generate_map(map* map);

map* create_map(float tile_width, float tile_height);