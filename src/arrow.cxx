#include "include/arrow.hxx"
#include "include/resources.hxx"

arrow::arrow(transform2d global_pos, int size, int direction, map* level_map)
    : game_object(global_pos, 15.f, size, direction, level_map)
    , spr(resources::arrow)
{
    hitbox = new collision::collider({ 14.f, -8.f },
                                     { 10.f, 16.f },
                                     { e_color::RED, 0.6f },
                                     size,
                                     direction);
}

void arrow::update(character* hero)
{
    if (collision::game_object_with_game_object(
            global_pos,
            hitbox->get_rectangle(),
            hero->get_global_pos(),
            hero->get_collider()->get_rectangle()))
    {
        hero->hurt();
        destroyed = true;
    }
    else if (collision::map_with_game_object(level_map,
                                             global_pos,
                                             hitbox->get_rectangle(),
                                             direction == 0
                                                 ? collision::direction::right
                                                 : collision::direction::left))
        destroyed = true;
    else
        global_pos.x += direction == 0 ? speed : -speed;
}

bool arrow::is_destroyed()
{
    return destroyed;
}

sprite* arrow::get_sprite()
{
    return spr;
}

collision::collider* arrow::get_hitbox()
{
    return hitbox;
}

arrow::~arrow()
{
    delete hitbox;
}