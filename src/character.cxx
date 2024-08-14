#include "engine/include/engine.hxx"

#include "include/character.hxx"

#include "glm/gtc/type_ptr.hpp"

character::character(prip_engine::transform2d global_pos,
                     float                    speed,
                     float                    size,
                     int                      direction,
                     map*                     level_map,
                     int                      health,
                     character_state          state)
    : game_object(global_pos, speed, size, direction, level_map)
    , max_health(health)
    , health(health)
    , state(state)
{
}

void character::draw()
{
    glm::mat4 model = glm::translate(
        glm::mat4{ 1 }, glm::vec3{ global_pos.x, global_pos.y, 0.f });
    if (direction == 1)
        model =
            glm::rotate(model, glm::radians(180.f), glm::vec3{ 0.f, 1.f, 0.f });
    model = glm::scale(model, glm::vec3{ size, size, 1.f });
    get_animation()->draw(glm::value_ptr(model));

    if (state != character_state::dead)
    {
        get_hitbox()->draw();
        if (state == character_state::melee_attack)
            attack_collider->draw();
    }
}

void character::hurt()
{
    health--;
}

bool character::is_alive()
{
    if (health > 0)
        return true;
    else
        set_state(character_state::dead);
    return false;
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

prip_engine::collider* character::get_hitbox()
{
    return hitboxes[state];
}

prip_engine::collider* character::get_attack_collider()
{
    return attack_collider;
}

prip_engine::animation* character::get_animation()
{
    return sprites[state];
}

character::~character()
{
    for (auto& hitbox : hitboxes)
        delete hitbox.second;
}

void character::resolve_map_collision(prip_engine::transform2d& delta_pos)
{
    prip_engine::collider*                     current_hitbox = get_hitbox();
    const std::vector<prip_engine::collider*>& map_colliders =
        level_map->get_colliders();
    character_state new_state = character_state::move;

    prip_engine::collision_direction dir =
        prip_engine::collision_direction::null;
    if (delta_pos.x < 0.f)
        dir = prip_engine::collision_direction::left;
    else if (delta_pos.x > 0.f)
        dir = prip_engine::collision_direction::right;
    else
        new_state = character_state::idle;

    if (dir != prip_engine::collision_direction::null)
        for (auto map_collider : map_colliders)
        {
            prip_engine::collision_resolution resol =
                current_hitbox->resolve(map_collider, dir);
            if (resol.first)
            {
                global_pos.x -= std::fabs(resol.second) < std::fabs(delta_pos.x)
                                    ? resol.second
                                    : delta_pos.x;
                new_state = character_state::idle;
                break;
            }
        }

    dir = prip_engine::collision_direction::down;
    if (delta_pos.y < 0.f)
    {
        dir       = prip_engine::collision_direction::up;
        new_state = character_state::jump;
    }

    bool collision_y = false;
    for (auto map_collider : map_colliders)
    {
        prip_engine::collision_resolution resol =
            current_hitbox->resolve(map_collider, dir);
        if (resol.first)
        {
            global_pos.y -= resol.second;
            if (new_state == character_state::jump)
                new_state = character_state::fall;
            collision_y = true;
            break;
        }
    }

    if (!collision_y && new_state != character_state::jump)
        new_state = character_state::fall;

    set_state(new_state);
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