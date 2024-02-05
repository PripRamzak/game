#pragma once

#include "game_object.hxx"

enum class enemy_type
{
    warrior,
    spearman
};

class enemy : public game_object
{
public:
    enemy(int                       health,
          float                     speed,
          float                     global_pos_x,
          float                     global_pos_y,
          float                     size,
          game_object_state         state,
          enemy_type                type,
          float                     patrol_area,
          std::chrono::milliseconds patrol_time,
          std::chrono::milliseconds attack_delay);
    static void initialize();
    void        spawn();
    bool        is_spawned();
    void        move(game_object* hero, std::chrono::milliseconds delta_time);
    void        attack(game_object* hero, std::chrono::milliseconds);
    ~enemy();

private:
    int                       patrol_direction = 0;
    float                     patrol_area      = 0.f;
    float                     patrol_area_dt   = 0.f;
    bool                      attacked         = false;
    std::chrono::milliseconds patrol_time;
    std::chrono::milliseconds patrol_time_dt;
    std::chrono::milliseconds attack_delay;
    std::chrono::milliseconds attack_delay_dt;
    bool                      spawned = false;
};