#include "include/game_object.hxx"

#include <algorithm>

game_object::game_object(transform2d       global_pos,
                         int               health,
                         float             speed,
                         float             size,
                         game_object_state state)
    : global_pos(global_pos)
    , health(health)
    , speed(speed)
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
        sprites[state_]->reset();
    state = state_;
}

transform2d game_object::get_global_pos()
{
    return global_pos;
}

int game_object::get_health()
{
    return health;
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

collision::collider* game_object::get_collider()
{
    return hitboxes[state];
}

collision::collider* game_object::get_attack_collider()
{
    return attack_collider;
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

void game_object::change_direction(int new_dir)
{
    if (direction != new_dir)
    {
        direction = new_dir;
        for (auto& hitbox : hitboxes)
            hitbox.second->change_pos(direction);
        attack_collider->change_pos(direction);
    }
}