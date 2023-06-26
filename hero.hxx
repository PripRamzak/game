#pragma once

#include "geometry.hxx"
#include "sprite.hxx"
#include "vertex_buffer.hxx"

#include <vector>

enum class hero_state
{
    idle,
    run,
    attack,
    run_n_attack
};

struct hero_sprite_state
{
    sprite*        hero_sprite = nullptr;
    hero_state     state;
    vertex_2d      vertices[4];
    vertex_buffer* sprite_vertex_buffer = nullptr;
};

class hero
{
public:
    virtual void       add_sprite(sprite* hero_sprite_, hero_state state) = 0;
    virtual void       move(float delta_x, float delta_y)                 = 0;
    virtual void       set_state(hero_state state_)                       = 0;
    virtual void       set_direction(int direction_)                      = 0;
    virtual float      get_current_pos_x()                                = 0;
    virtual float      get_current_pos_y()                                = 0;
    virtual void       get_delta_pos(float& x, float& y)                  = 0;
    virtual float      get_size()                                         = 0;
    virtual hero_state get_state()                                        = 0;
    virtual int        get_direction()                                    = 0;
    virtual sprite*    get_sprite()                                       = 0;
    virtual vertex_2d* get_vertices()                                     = 0;
    virtual vertex_buffer* get_vertex_buffer()                            = 0;
    virtual ~hero();
};

hero* create_hero(float pos_x, float pos_y, float size, hero_state state);