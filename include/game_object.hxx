#pragma once

#include "collision.hxx"
#include "engine/include/animation.hxx"
#include "engine/include/geometry.hxx"
#include "engine/include/vertex_buffer.hxx"

#include <unordered_map>

enum class game_object_state
{
    idle,
    run,
    attack,
    jump,
    fall,
    run_n_attack
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
    float             get_global_pos_x();
    float             get_global_pos_y();
    float             get_size();
    game_object_state get_state();
    int               get_direction();
    animation*        get_animation();
    virtual ~game_object();

    friend bool collision::map_with_game_object(map*         map,
                                                game_object* game_object,
                                                collision::direction direction);
    friend bool collision::game_object_with_game_object(
        game_object* game_object_1, game_object* game_object_2);

protected:
    std::unordered_map<game_object_state, animation*> sprites;
    int                                               health       = 0;
    float                                             speed        = 0.f;
    float                                             global_pos_x = 0.f;
    float                                             global_pos_y = 0.f;
    float                                             size         = 0.f;
    int                                               direction    = 0;
    game_object_state state = game_object_state::idle;
};