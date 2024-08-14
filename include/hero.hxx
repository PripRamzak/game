#pragma once

#include "engine/include/sound_buffer.hxx"

#include "character.hxx"
#include "map.hxx"

class hero : public character
{
public:
    hero(prip_engine::transform2d global_pos, int direction, map* level_map);
    void update(std::chrono::milliseconds) override;
    void draw() override;
    bool is_attacked();
    ~hero();

private:
    void move(int dx, int dy);
    bool jump();
    void attack();
    void draw_health(int sprite_number, int pos_x);

    prip_engine::sound_buffer*        sound_attack = nullptr;
    std::vector<prip_engine::sprite*> health_interface;
    bool                              attacked       = false;
    float                             jump_force     = 0.f;
    float                             jump_height    = 0.f;
    float                             jump_height_dt = 0.f;
};