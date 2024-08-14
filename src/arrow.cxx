#include "include/arrow.hxx"
#include "include/resources.hxx"

#include "glm/gtc/type_ptr.hpp"

arrow::arrow(prip_engine::transform2d global_pos,
             int                      size,
             int                      direction,
             map*                     level_map)
    : game_object(global_pos, 15.f, size, direction, level_map)
    , spr(resources::arrow)
{
    hitbox = new prip_engine::collider(this->global_pos,
                                       { 14.f, -8.f },
                                       { 10.f, 16.f },
                                       { prip_engine::e_color::RED, 0.6f },
                                       size,
                                       direction,
                                       true);
}

void arrow::update(character* hero)
{
    if (hitbox->detect(hero->get_hitbox()))
    {
        hero->hurt();
        destroyed = true;
        return;
    }

    const std::vector<prip_engine::collider*>& map_colliders =
        level_map->get_colliders();
    for (auto collider : map_colliders)
        if (hitbox->detect(collider))
        {
            destroyed = true;
            return;
        }

    global_pos.x += direction == 0 ? speed : -speed;
}

void arrow::draw()
{
    glm::mat4 model = glm::translate(
        glm::mat4{ 1 }, glm::vec3{ global_pos.x, global_pos.y, 0.f });
    if (direction == 1)
        model =
            glm::rotate(model, glm::radians(180.f), glm::vec3{ 0.f, 1.f, 0.f });
    model         = glm::scale(model, glm::vec3{ size, size, 1.f });
    spr->draw(glm::value_ptr(model), true);

    hitbox->draw();
}

bool arrow::is_destroyed()
{
    return destroyed;
}

prip_engine::sprite* arrow::get_sprite()
{
    return spr;
}

prip_engine::collider* arrow::get_hitbox()
{
    return hitbox;
}

arrow::~arrow()
{
    delete hitbox;
}