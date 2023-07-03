#include "game_object.hxx"

game_object::game_object(int               health_,
                         float             local_pos_x_,
                         float             local_pos_y_,
                         float             global_pos_x_,
                         float             global_pos_y_,
                         float             size_,
                         game_object_state state_)
    : health(health_)
    , local_pos_x(local_pos_x_)
    , local_pos_y(local_pos_y_)
    , global_pos_x(global_pos_x_)
    , global_pos_y(global_pos_y_)
    , size(size_)
    , state(state_)
{
}

game_object::~game_object()
{
    for (auto sprite : sprites)
    {
        delete sprite.game_object_sprite;
        delete sprite.sprite_vertex_buffer;
    }
}