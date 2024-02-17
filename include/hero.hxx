#pragma once

#include "game_object.hxx"
#include "map.hxx"

class hero : public game_object
{
public:
    hero(transform2d global_pos,
         int         health,
         float       speed,
         float       size,
         float       jump_force,
         float       jump_height);
    static void initialize();
    void        move(float dx, float dy, map* map);
    void        jump();
    void        attack(game_object* enemy);
    ~hero();

private:
    bool  attacked       = false;
    float jump_force     = 0.f;
    float jump_height    = 0.f;
    float jump_height_dt = 0.f;
};