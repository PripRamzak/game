#pragma once

#include "character.hxx"
#include "map.hxx"

class hero : public character
{
public:
    hero(prip_engine::transform2d global_pos,
         float                    speed,
         float                    size,
         int                      direction,
         map*                     level_map,
         int                      health,
         float                    jump_force,
         float                    jump_height);
    void update(std::chrono::milliseconds, character* enemy) override;
    ~hero();

private:
    void move(float dx, float dy);
    void jump();
    void attack(character* enemy);

    bool  attacked       = false;
    float jump_force     = 0.f;
    float jump_height    = 0.f;
    float jump_height_dt = 0.f;
};