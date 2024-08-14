#pragma once

#include "engine/include/animation.hxx"
#include "engine/include/collision.hxx"

#include "game_object.hxx"

#include <unordered_map>

enum class character_state
{
    idle,
    move,
    melee_attack,
    range_attack,
    jump,
    fall,
    dead
};

class character : public game_object
{
public:
    character(prip_engine::transform2d global_pos,
              float                    speed,
              float                    size,
              int                      direction,
              map*                     level_map,
              int                      health,
              character_state          state);
    virtual void            update(std::chrono::milliseconds delta_time) = 0;
    void                    draw() override;
    void                    hurt();
    bool                    is_alive();
    void                    set_state(character_state state_);
    int                     get_health();
    character_state         get_state();
    prip_engine::collider*  get_hitbox();
    prip_engine::collider*  get_attack_collider();
    prip_engine::animation* get_animation();
    virtual ~character();

protected:
    void resolve_map_collision(prip_engine::transform2d& delta_xy);
    void change_direction(int new_dir);

    std::unordered_map<character_state, prip_engine::animation*> sprites;
    std::unordered_map<character_state, prip_engine::collider*>  hitboxes;
    prip_engine::collider* attack_collider;
    int                    max_health = 0;
    int                    health     = 0;
    character_state        state;
};