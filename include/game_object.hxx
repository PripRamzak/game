#pragma once

#include "collision.hxx"
#include "engine/include/animation.hxx"
#include "engine/include/geometry.hxx"
#include "engine/include/vertex_buffer.hxx"

#include <unordered_map>

enum class game_object_state
{
    idle,
    move,
    attack,
    jump,
    fall,
    run_n_attack
};

class game_object
{
public:
    game_object(transform2d       global_pos,
                int               health,
                float             speed,
                float             size,
                game_object_state state);
    bool                 is_alive();
    void                 hurt();
    void                 set_state(game_object_state state_);
    transform2d          get_global_pos();
    int                  get_health();
    float                get_size();
    game_object_state    get_state();
    int                  get_direction();
    collision::collider* get_collider();
    collision::collider* get_attack_collider();
    animation*           get_animation();
    virtual ~game_object();

protected:
    void change_direction(int new_dir);

    std::unordered_map<game_object_state, animation*>           sprites;
    std::unordered_map<game_object_state, collision::collider*> hitboxes;
    collision::collider*                                        attack_collider;
    transform2d                                                 global_pos;
    int                                                         health    = 0;
    float                                                       speed     = 0.f;
    float                                                       size      = 0.f;
    int                                                         direction = 0;
    game_object_state state = game_object_state::idle;
};