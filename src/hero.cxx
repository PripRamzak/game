#include "engine/include/engine.hxx"

#include "include/hero.hxx"
#include "include/resources.hxx"

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

    collision::collider* idle_hitbox =
        new collision::collider({ -14.f, -20.f },
                                { 34.f, 44.f },
                                { prip_engine::e_color::GREEN, 0.6f },
                                size,
                                direction);
    collision::collider* run_hitbox =
        new collision::collider({ -12.f, -20.f },
                                { 32.f, 44.f },
                                { prip_engine::e_color::GREEN, 0.6f },
                                size,
                                direction);
    collision::collider* attack_hitbox =
        new collision::collider({ -15.f, -20.f },
                                { 26.f, 44.f },
                                { prip_engine::e_color::GREEN, 0.6f },
                                size,
                                direction);
    collision::collider* jump_hitbox =
        new collision::collider({ -24.f, -24.f },
                                { 48.f, 48.f },
                                { prip_engine::e_color::GREEN, 0.6f },
                                size,
                                direction);
    collision::collider* fall_hitbox =
        new collision::collider({ -24.f, -24.f },
                                { 48.f, 48.f },
                                { prip_engine::e_color::GREEN, 0.6f },
                                size,
                                direction);

    hitboxes.emplace(character_state::idle, idle_hitbox);
    hitboxes.emplace(character_state::move, run_hitbox);
    hitboxes.emplace(character_state::melee_attack, attack_hitbox);
    hitboxes.emplace(character_state::jump, jump_hitbox);
    hitboxes.emplace(character_state::fall, fall_hitbox);

    attack_collider =
        new collision::collider{ { 11.f, -10.f },
                                 { 32.f, 34.f },
                                 { prip_engine::e_color::ORANGE, 0.6f },
                                 size,
                                 direction };
}

void hero::update(std::chrono::milliseconds delta_time)
{
    if (prip_engine::is_key_down(prip_engine::key::attack) &&
        state != character_state::jump && state != character_state::fall)
        set_state(character_state::melee_attack);
    else
    {
        float dx = 0.f;
        float dy = 0.f;

        if (prip_engine::is_key_down(prip_engine::key::jump) &&
            state != character_state::jump && state != character_state::fall)
            set_state(character_state::jump);

        if (state == character_state::jump)
        {
            dy--;
            jump();
        }
        else if (state == character_state::fall)
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

bool hero::is_attacked()
{
    return attacked;
}

void hero::move(float dx, float dy)
{
    float delta_x = dx * speed;
    float delta_y = dy * speed;

    global_pos.x += delta_x;

    if (delta_x < 0.f)
    {
        change_direction(1);
        if (collision::map_with_game_object(level_map,
                                            global_pos,
                                            hitboxes[state]->get_rectangle(),
                                            collision::direction::left))
        {
            if (state != character_state::jump &&
                state != character_state::fall)
            {
                set_state(character_state::idle);
                return;
            }
        }
    }
    else if (delta_x > 0.f)
    {
        change_direction(0);
        if (collision::map_with_game_object(level_map,
                                            global_pos,
                                            hitboxes[state]->get_rectangle(),
                                            collision::direction::right))
        {
            if (state != character_state::jump &&
                state != character_state::fall)
            {
                set_state(character_state::idle);
                return;
            }
        }
    }

    if (state != character_state::jump && state != character_state::fall &&
        !collision::map_with_game_object(level_map,
                                         global_pos,
                                         hitboxes[state]->get_rectangle(),
                                         collision::direction::down))
    {
        set_state(character_state::fall);
        delta_y += speed;
    }

    global_pos.y += delta_y;

    if (delta_y > 0.f)
    {
        if (collision::map_with_game_object(level_map,
                                            global_pos,
                                            hitboxes[state]->get_rectangle(),
                                            collision::direction::down))
        {
            set_state(character_state::idle);
            return;
        }
        else
            set_state(character_state::fall);
    }
    else if (delta_y < 0.f)
        if (collision::map_with_game_object(level_map,
                                            global_pos,
                                            hitboxes[state]->get_rectangle(),
                                            collision::direction::up))
        {
            set_state(character_state::fall);
            jump_height_dt = 0.f;
        }

    if (state != character_state::jump && state != character_state::fall)
        set_state(character_state::move);
}

void hero::jump()
{
    set_state(character_state::jump);

    std::cout << jump_height_dt << std::endl;

    jump_height_dt += jump_force;

    if (jump_height_dt >= jump_height)
    {
        set_state(character_state::fall);
        jump_height_dt = 0.f;
    }
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