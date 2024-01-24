#include "include/game_object.hxx"

#include <algorithm>
#include <iostream>

game_object::game_object(int               health_,
                         float             speed_,
                         float             global_pos_x_,
                         float             global_pos_y_,
                         float             size_,
                         game_object_state state_)
    : health(health_)
    , speed(speed_)
    , global_pos_x(global_pos_x_)
    , global_pos_y(global_pos_y_)
    , size(size_)
    , state(state_)
{
}

bool game_object::is_alive()
{
    return health > 0 ? true : false;
}

void game_object::hurt()
{
    health--;
}

void game_object::set_state(game_object_state state_)
{
    if (state != state_)
        sprites[state_]->reset();
    state = state_;
}

int game_object::get_health()
{
    return health;
}

float game_object::get_global_pos_x()
{
    return global_pos_x;
}

float game_object::get_global_pos_y()
{
    return global_pos_y;
}

float game_object::get_size()
{
    return size;
}

game_object_state game_object::get_state()
{
    return state;
}

int game_object::get_direction()
{
    return direction;
}

animation* game_object::get_animated_sprite()
{
    return sprites[state];
}

game_object::~game_object()
{
    for (auto sprite : sprites)
        delete sprite.second;
}