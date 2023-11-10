#pragma once

#include "engine/include/geometry.hxx"
#include "engine/include/sprite.hxx"
#include "engine/include/vertex_buffer.hxx"

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
};

class game_object
{
public:
    game_object(int               health_,
                float             speed_,
                float             global_pos_x,
                float             global_pos_y,
                float             size_,
                game_object_state state_);
    bool              is_alive();
    void              hurt();
    void              set_state(game_object_state state_);
    int               get_health();
    float             get_current_pos_x();
    float             get_current_pos_y();
    float             get_delta_x();
    float             get_delta_y();
    float             get_size();
    game_object_state get_state();
    int               get_direction();
    sprite*           get_sprite();
    virtual ~game_object();

protected:
    auto find_sprite(game_object_state state_)
        -> std::vector<hero_sprite_state>::iterator;
    void add_sprite(sprite* game_object_sprite_, game_object_state state_);

    std::vector<hero_sprite_state> sprites;
    int                            health       = 0;
    float                          speed        = 0.f;
    float                          global_pos_x = 0.f;
    float                          global_pos_y = 0.f;
    float                          delta_x      = 0.f;
    float                          delta_y      = 0.f;
    float                          size         = 0.f;
    int                            direction    = 0;
    game_object_state              state        = game_object_state::idle;
};