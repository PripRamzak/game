#include "include/game_object.hxx"

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

game_object::~game_object()
{
    for (auto sprite : sprites)
        delete sprite.game_object_sprite;
}