#pragma once

#include "engine/include/sprite.hxx"

#include "include/character.hxx"
#include "include/collision.hxx"

class arrow : public game_object
{
public:
    arrow(transform2d global_pos, int size, int direction, map* level_map);
    void                 update(character* hero);
    bool                 is_destroyed();
    sprite*              get_sprite();
    collision::collider* get_hitbox();
    ~arrow();

private:
    sprite*              spr;
    collision::collider* hitbox;
    bool                 destroyed = false;
};