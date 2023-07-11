#pragma once

#include "geometry.hxx"
#include "sprite.hxx"
#include "vertex_buffer.hxx"

#include <vector>

enum class game_object_state
{
    idle,
    run,
    attack,
    run_n_attack
};

struct hero_sprite_state
{
    sprite*           game_object_sprite = nullptr;
    game_object_state state;
    vertex_2d         vertices[4];
    vertex_buffer*    sprite_vertex_buffer = nullptr;
};

class game_object
{
protected:
    std::vector<hero_sprite_state> sprites;
    bool                           alive        = false;
    int                            health       = 0;
    float                          speed        = 0.f;
    float                          local_pos_x  = 0.f;
    float                          local_pos_y  = 0.f;
    float                          global_pos_x = 0.f;
    float                          global_pos_y = 0.f;
    float                          delta_x      = 0.f;
    float                          delta_y      = 0.f;
    float                          size         = 0.f;
    int                            direction    = 0;
    game_object_state              state        = game_object_state::idle;

public:
    game_object(int               health_,
                float             speed_,
                float             local_pos_x_,
                float             local_pos_y_,
                float             global_pos_x,
                float             global_pos_y,
                float             size_,
                game_object_state state_);
    virtual void              add_sprite(sprite*           game_object_sprite_,
                                         game_object_state state) = 0;
    virtual bool              is_alive()                          = 0;
    virtual void              hurt()                              = 0;
    virtual void              set_state(game_object_state state_) = 0;
    virtual void              set_direction(int direction_)       = 0;
    virtual int               get_health()                        = 0;
    virtual float             get_current_pos_x()                 = 0;
    virtual float             get_current_pos_y()                 = 0;
    virtual float             get_delta_x()                       = 0;
    virtual float             get_delta_y()                       = 0;
    virtual float             get_size()                          = 0;
    virtual game_object_state get_state()                         = 0;
    virtual int               get_direction()                     = 0;
    virtual sprite*           get_sprite()                        = 0;
    virtual vertex_2d*        get_vertices()                      = 0;
    virtual vertex_buffer*    get_vertex_buffer()                 = 0;
    virtual ~game_object();
};