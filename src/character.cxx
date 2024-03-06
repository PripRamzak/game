#include "engine/include/engine.hxx"

#include "include/camera.hxx"
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
    glm::mat4 projection = glm::make_mat4x4(camera::get_projection());
    glm::mat4 view       = glm::make_mat4x4(camera::get_view());
    glm::mat4 model      = glm::translate(
        glm::mat4{ 1 }, glm::vec3{ global_pos.x, global_pos.y, 0.f });
    if (direction == 1)
        model =
            glm::rotate(model, glm::radians(180.f), glm::vec3{ 0.f, 1.f, 0.f });
    model         = glm::scale(model, glm::vec3{ size, size, 1.f });
    glm::mat4 mvp = projection * view * model;
    prip_engine::render(get_animation(), &mvp[0][0]);

    if (state != character_state::dead)
    {
        get_hitbox()->draw(global_pos);
        if (state == character_state::melee_attack)
            attack_collider->draw(global_pos);
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

collision::collider* character::get_hitbox()
{
    return hitboxes[state];
}

collision::collider* character::get_attack_collider()
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