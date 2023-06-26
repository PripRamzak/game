#include "game_object.hxx"

game_object::game_object(int               pos_x,
                         int               pos_y,
                         float             size_,
                         game_object_state state_)
    : position_x(pos_x)
    , position_y(pos_y)
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