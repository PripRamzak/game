#include "engine/include/engine.hxx"

#include "include/arrow.hxx"
#include "include/camera.hxx"
#include "include/resources.hxx"

#include "glm/gtc/type_ptr.hpp"

arrow::arrow(prip_engine::transform2d global_pos,
             int                      size,
             int                      direction,
             map*                     level_map)
    : game_object(global_pos, 15.f, size, direction, level_map)
    , spr(resources::arrow)
{
    hitbox = new collision::collider({ 14.f, -8.f },
                                     { 10.f, 16.f },
                                     { prip_engine::e_color::RED, 0.6f },
                                     size,
                                     direction);
}

void arrow::update(character* hero)
{
    if (collision::game_object_with_game_object(
            global_pos,
            hitbox->get_rectangle(),
            hero->get_global_pos(),
            hero->get_hitbox()->get_rectangle()))
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

void arrow::draw()
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
    prip_engine::render(spr, &mvp[0][0]);

    hitbox->draw(global_pos);
}

bool arrow::is_destroyed()
{
    return destroyed;
}

prip_engine::sprite* arrow::get_sprite()
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