#pragma once

#include "engine/include/animation.hxx"

#include "collision.hxx"
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
    character(transform2d     global_pos,
              float           speed,
              float           size,
              int             direction,
              map*            level_map,
              int             health,
              character_state state);
    bool                 is_alive();
    void                 hurt();
    void                 set_state(character_state state_);
    int                  get_health();
    character_state      get_state();
    collision::collider* get_collider();
    collision::collider* get_attack_collider();
    animation*           get_animation();
    virtual ~character();

protected:
    void change_direction(int new_dir);

    std::unordered_map<character_state, animation*>           sprites;
    std::unordered_map<character_state, collision::collider*> hitboxes;
    collision::collider*                                      attack_collider;
    int                                                       health = 0;
    character_state                                           state;
};