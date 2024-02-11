#include "include/game_object.hxx"

#include <algorithm>

game_object::game_object(int               health,
                         float             speed,
                         float             global_pos_x,
                         float             global_pos_y,
                         float             size,
                         game_object_state state)
    : health(health)
    , speed(speed)
    , global_pos_x(global_pos_x)
    , global_pos_y(global_pos_y)
    , size(size)
    , state(state)
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
    {
        if (sprites[state]->get_sprite()->get_height() !=
            sprites[state_]->get_sprite()->get_height())
            global_pos_y += (sprites[state]->get_sprite()->get_height() -
                             sprites[state_]->get_sprite()->get_height()) *
                            size / 2;
        sprites[state_]->reset();
    }
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

animation* game_object::get_animation()
{
    return sprites[state];
}

game_object::~game_object()
{
    for (auto sprite : sprites)
        delete sprite.second;
}