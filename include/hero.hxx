#pragma once

#include "engine/include/sound_buffer.hxx"

#include "character.hxx"
#include "map.hxx"

class hero : public character
{
public:
    hero(prip_engine::transform2d global_pos, int direction, map* level_map);
    void update(std::chrono::milliseconds) override;
    bool is_attacked();
    ~hero();

private:
    void move(float dx, float dy);
    void jump();
    void attack();

    prip_engine::sound_buffer* sound_attack   = nullptr;
    bool                       attacked       = false;
    float                      jump_force     = 0.f;
    float                      jump_height    = 0.f;
    float                      jump_height_dt = 0.f;
};