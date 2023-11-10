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
         game_object_state state);
    static void initialize();
    void        move(int dx, int dy, map* map, bool* skeleton_collision);
    void        attack(game_object* enemy, bool skeleton_collision);
    bool        check_collision_map(map* map);
    ~hero();

private:
    bool attacked = false;
};