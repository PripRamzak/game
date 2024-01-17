#pragma once

#include "game_object.hxx"
#include "map.hxx"

class hero : public game_object
{
public:
    hero(int               health,
         float             speed,
         float             global_pos_x,
         float             global_pos_y,
         float             size,
         game_object_state state,
         float             jump_height_);
    static void initialize();
    void        move(float dx, float dy, map* map);
    void        jump();
    void        attack(game_object* enemy, bool skeleton_collision);
    bool        check_collision_map(map* map, map_tile_type type);
    bool        check_collision_map_x_axis(map* map, map_tile_type type);
    bool        check_collision_map_y_axis(map* map, map_tile_type type);
    ~hero();

private:
    bool  attacked       = false;
    float jump_height    = 0.f;
    float jump_height_dt = 0.f;
};