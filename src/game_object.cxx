#include "include/game_object.hxx"

game_object::game_object(transform2d global_pos,
                         float       speed,
                         float       size,
                         int         direction,
                         map*        level_map)
    : global_pos(global_pos)
    , speed(speed)
    , size(size)
    , direction(direction)
    , level_map(level_map)
{
}

transform2d game_object::get_global_pos()
{
    return global_pos;
}

float game_object::get_size()
{
    return size;
}

int game_object::get_direction()
{
    return direction;
}

game_object::~game_object() {}