#pragma once

#include "game_object.hxx"

enum class enemy_type
{
    warrior,
    spearman
};

class enemy : public game_object
{
    bool attacked = false;
    bool spawned  = false;

public:
    enemy(int               health,
          float             speed,
          float             global_pos_x,
          float             global_pos_y,
          float             size,
          game_object_state state,
          enemy_type        type);
    static void initialize();
    void        spawn();
    bool        is_spawned();
    void        move(game_object* hero);
    void        attack(game_object* hero);
    bool        check_hero_collision_x(game_object* hero);
    bool        check_hero_collision_y(game_object* hero);
    ~enemy();
};