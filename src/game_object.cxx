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

auto game_object::find_sprite(game_object_state state_)
    -> std::vector<anim_sprite_state>::iterator
{
    auto it = std::find_if(sprites.begin(),
                           sprites.end(),
                           [&](const anim_sprite_state sprite)
                           { return sprite.state == state_; });

    return it;
}

void game_object::add_sprite(animation*        game_object_anim_sprite_,
                             game_object_state state_)
{
    auto it = find_sprite(state_);

    if (it != sprites.end())
    {
        std::cout << "This tile already exists" << std::endl;
        return;
    }
    else
    {
        anim_sprite_state sprite_;
        sprite_.game_object_anim_sprite = game_object_anim_sprite_;
        sprite_.state                   = state_;

        sprites.push_back(sprite_);
    }
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
        const auto it = find_sprite(state);
        if (it != sprites.end())
            it->game_object_anim_sprite->reset();
        else
        {
            std::cout << "Such sprite doesn't exists" << std::endl;
            return;
        }
    }
    state = state_;
}

int game_object::get_health()
{
    return health;
}

float game_object::get_current_pos_x()
{
    return global_pos_x + delta_x;
}

float game_object::get_current_pos_y()
{
    return global_pos_y + delta_y;
}

float game_object::get_delta_x()
{
    return delta_x;
}

float game_object::get_delta_y()
{
    return delta_y;
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
    auto it = find_sprite(state);

    if (it != sprites.end())
        return it->game_object_anim_sprite;
    else
    {
        std::cout << "Such sprite doesn't exists" << std::endl;
        return nullptr;
    }
}

game_object::~game_object()
{
    for (auto sprite : sprites)
        delete sprite.game_object_anim_sprite;
}