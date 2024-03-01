#include "include/character.hxx"

character::character(transform2d     global_pos,
                     float           speed,
                     float           size,
                     int             direction,
                     map*            level_map,
                     int             health,
                     character_state state)
    : game_object(global_pos, speed, size, direction, level_map)
    , health(health)
    , state(state)
{
}

bool character::is_alive()
{
    if (health > 0)
        return true;
    else
        set_state(character_state::dead);
    return false;
}

void character::hurt()
{
    health--;
}

void character::set_state(character_state state_)
{
    if (state != state_)
        sprites[state_]->reset();
    state = state_;
}

int character::get_health()
{
    return health;
}

character_state character::get_state()
{
    return state;
}

collision::collider* character::get_collider()
{
    return hitboxes[state];
}

collision::collider* character::get_attack_collider()
{
    return attack_collider;
}

animation* character::get_animation()
{
    return sprites[state];
}

character::~character()
{
    for (auto& hitbox : hitboxes)
        delete hitbox.second;
}

void character::change_direction(int new_dir)
{
    if (direction != new_dir)
    {
        direction = new_dir;
        for (auto& hitbox : hitboxes)
            hitbox.second->change_pos(direction);
        attack_collider->change_pos(direction);
    }
}