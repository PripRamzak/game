#include "engine/include/engine.hxx"

#include "include/hero.hxx"
#include "include/resources.hxx"

#include "glm/gtc/type_ptr.hpp"

using namespace std::chrono_literals;

hero::hero(prip_engine::transform2d global_pos, int direction, map* level_map)
    : character(global_pos,
                10.f,
                2.25f,
                direction,
                level_map,
                4,
                character_state::idle)
    , sound_attack(resources::warrior_attack_sound)
    , jump_height(300.f)
    , jump_force(12.f)
{
    prip_engine::animation* anim_warrior_idle =
        new prip_engine::animation(resources::warrior_idle, 6, 100ms);
    prip_engine::animation* anim_warrior_run =
        new prip_engine::animation(resources::warrior_run, 6, 100ms);
    prip_engine::animation* anim_warrior_attack =
        new prip_engine::animation(resources::warrior_attack, 4, 100ms);
    prip_engine::animation* anim_warrior_jump =
        new prip_engine::animation(resources::warrior_jump, 1, 250ms);
    prip_engine::animation* anim_warrior_fall =
        new prip_engine::animation(resources::warrior_fall, 1, 250ms);

    sprites.emplace(character_state::idle, anim_warrior_idle);
    sprites.emplace(character_state::move, anim_warrior_run);
    sprites.emplace(character_state::melee_attack, anim_warrior_attack);
    sprites.emplace(character_state::jump, anim_warrior_jump);
    sprites.emplace(character_state::fall, anim_warrior_fall);

    prip_engine::collider* idle_hitbox =
        new prip_engine::collider(this->global_pos,
                                  { -14.f, -20.f },
                                  { 34.f, 44.f },
                                  { prip_engine::e_color::GREEN, 0.6f },
                                  size,
                                  direction,
                                  true);
    prip_engine::collider* run_hitbox =
        new prip_engine::collider(this->global_pos,
                                  { -12.f, -20.f },
                                  { 32.f, 44.f },
                                  { prip_engine::e_color::GREEN, 0.6f },
                                  size,
                                  direction,
                                  true);
    prip_engine::collider* attack_hitbox =
        new prip_engine::collider(this->global_pos,
                                  { -15.f, -20.f },
                                  { 26.f, 44.f },
                                  { prip_engine::e_color::GREEN, 0.6f },
                                  size,
                                  direction,
                                  true);
    prip_engine::collider* jump_hitbox =
        new prip_engine::collider(this->global_pos,
                                  { -24.f, -24.f },
                                  { 48.f, 48.f },
                                  { prip_engine::e_color::GREEN, 0.6f },
                                  size,
                                  direction,
                                  true);
    prip_engine::collider* fall_hitbox =
        new prip_engine::collider(this->global_pos,
                                  { -24.f, -24.f },
                                  { 48.f, 48.f },
                                  { prip_engine::e_color::GREEN, 0.6f },
                                  size,
                                  direction,
                                  true);

    hitboxes.emplace(character_state::idle, idle_hitbox);
    hitboxes.emplace(character_state::move, run_hitbox);
    hitboxes.emplace(character_state::melee_attack, attack_hitbox);
    hitboxes.emplace(character_state::jump, jump_hitbox);
    hitboxes.emplace(character_state::fall, fall_hitbox);

    attack_collider =
        new prip_engine::collider{ global_pos,
                                   { 11.f, -10.f },
                                   { 32.f, 34.f },
                                   { prip_engine::e_color::ORANGE, 0.6f },
                                   size,
                                   direction,
                                   true };

    health_interface.push_back(
        new prip_engine::sprite{ resources::heart_full, { 48.f, 48.f } });
    health_interface.push_back(
        new prip_engine::sprite{ resources::heart_half, { 48.f, 48.f } });
    health_interface.push_back(
        new prip_engine::sprite{ resources::heart_empty, { 48.f, 48.f } });
}

void hero::update(std::chrono::milliseconds delta_time)
{
    if (prip_engine::is_key_down(prip_engine::key::attack) &&
        state != character_state::jump && state != character_state::fall)
        set_state(character_state::melee_attack);
    else
    {
        attacked = false;

        int dx = 0;
        int dy = 0;

        if (prip_engine::is_key_down(prip_engine::key::jump) &&
            state != character_state::jump && state != character_state::fall)
            set_state(character_state::jump);

        if (state == character_state::jump)
            if (jump())
                dy--;

        if (state == character_state::fall)
            dy++;

        if (prip_engine::is_key_down(prip_engine::key::left))
            dx--;
        if (prip_engine::is_key_down(prip_engine::key::right))
            dx++;

        if (dx == 0 && dy == 0)
            set_state(character_state::idle);
        else
            move(dx, dy);
    }

    if (state == character_state::melee_attack)
        attack();

    get_animation()->play(delta_time);
}

void hero::draw()
{
    character::draw();

    float pos_x = 34.f;

    for (int i = 0; i < health / 2; i++, pos_x += 53.f)
        draw_health(0, pos_x);

    if (health % 2 == 1)
    {
        draw_health(1, pos_x);
        pos_x += 53.f;
    }

    for (int i = 0; i < (max_health - health) / 2; i++, pos_x += 53.f)
        draw_health(2, pos_x);
}

bool hero::is_attacked()
{
    return attacked;
}

void hero::draw_health(int sprite_number, int pos_x)
{
    glm::mat4 model =
        glm::translate(glm::mat4{ 1 }, glm::vec3{ pos_x, 34.f, 0.f });
    health_interface[sprite_number]->draw(glm::value_ptr(model), false);
}

void hero::move(int dx, int dy)
{
    prip_engine::transform2d delta_pos(static_cast<float>(dx) * speed,
                                       static_cast<float>(dy) * speed);

    global_pos.x += delta_pos.x;
    global_pos.y += delta_pos.y;

    if (delta_pos.x > 0.f)
        change_direction(0);
    else if (delta_pos.x < 0.f)
        change_direction(1);

    resolve_map_collision(delta_pos);
}

bool hero::jump()
{
    set_state(character_state::jump);

    std::cout << jump_height_dt << std::endl;

    jump_height_dt += jump_force;

    if (jump_height_dt > jump_height)
    {
        set_state(character_state::fall);
        jump_height_dt = 0.f;
        return false;
    }

    return true;
}

void hero::attack()
{
    prip_engine::animation* attack_anim = sprites[state];
    int anim_current_number = attack_anim->get_current_frame_number();
    int anim_quantity       = attack_anim->get_frames_quantity();

    if (get_animation()->get_current_frame_number() == 2 &&
        !sound_attack->get_playing_status())
        sound_attack->play(prip_engine::audio_properties::once);
    if (!attacked && anim_current_number == anim_quantity - 1)
        attacked = true;

    if (attacked && anim_current_number == 0)
        attacked = false;
}

hero::~hero() = default;