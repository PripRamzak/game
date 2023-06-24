#pragma once

#include "geometry.hxx"
#include "sprite.hxx"
#include "texture.hxx"
#include "vertex_buffer.hxx"

#include <vector>

enum class map_tile
{
    null_tile,
    floor
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
    virtual void fill_rectangle(int      start_x,
                                int      start_y,
                                int      width_,
                                int      height_,
                                float    window_width,
                                float    window_height,
                                map_tile type)                  = 0;
    virtual texture* get_tile(map_tile type)                    = 0;
    ~map();
};

map* create_map(int width, int height, int tile_width, int tile_height);