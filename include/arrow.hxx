#pragma once

#include "engine/include/sprite.hxx"
#include "engine/include/collision.hxx"

#include "include/character.hxx"

class arrow : public game_object
{
public:
    arrow(prip_engine::transform2d global_pos,
          int                      size,
          int                      direction,
          map*                     level_map);
    void                   update(character* hero);
    void                   draw() override;
    bool                   is_destroyed();
    prip_engine::sprite*   get_sprite();
    prip_engine::collider* get_hitbox();
    ~arrow();

private:
    prip_engine::sprite*   spr;
    prip_engine::collider* hitbox;
    bool                   destroyed = false;
};