#pragma once

#include "engine/include/sprite.hxx"

#include "include/character.hxx"
#include "include/collision.hxx"

class arrow : public game_object
{
public:
    arrow(prip_engine::transform2d global_pos,
          int                      size,
          int                      direction,
          map*                     level_map);
    void                 update(character* hero);
    void                 draw(float* matrix) override;
    bool                 is_destroyed();
    prip_engine::sprite* get_sprite();
    collision::collider* get_hitbox();
    ~arrow();

private:
    prip_engine::sprite* spr;
    collision::collider* hitbox;
    bool                 destroyed = false;
};