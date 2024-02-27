#include "include/enemy.hxx"
#include "include/resources.hxx"

#include <cmath>

using namespace std::chrono_literals;

enemy::enemy(transform2d               global_pos,
             int                       health,
             float                     speed,
             float                     size,
             game_object_state         state,
             std::chrono::milliseconds attack_delay)
    : game_object(global_pos, health, speed, size, state)
    , attack_delay(attack_delay)
    , attack_delay_dt(0ms)
{
}

void enemy::spawn()
{
    spawned = true;
}

bool enemy::is_spawned()
{
    return spawned;
}

collision::collider* enemy::get_attack_trigger()
{
    return attack_trigger;
}

void enemy::move()
{
    set_state(game_object_state::move);
    if (direction == 0)
        global_pos.x += speed;
    else
        global_pos.x -= speed;
}

void enemy::attack(game_object* hero, std::chrono::milliseconds delta_time)
{
    if (state != game_object_state::attack)
    {
        set_state(game_object_state::idle);
        attack_delay_dt += delta_time;
        if (attack_delay_dt < attack_delay)
            return;
        attack_delay_dt -= attack_delay;
    }

    set_state(game_object_state::attack);

    animation* attack_anim         = get_animation();
    int        anim_current_number = attack_anim->get_current_frame_number();
    int        anim_quantity       = attack_anim->get_frames_quantity();

    if (anim_current_number == anim_quantity - 1)
    {
        if (collision::game_object_with_game_object(
                global_pos,
                attack_collider->get_rectangle(),
                hero->get_global_pos(),
                hero->get_collider()->get_rectangle()))
            hero->hurt();
        set_state(game_object_state::idle);
    }
}

enemy::~enemy() = default;

skeleton_warrior::skeleton_warrior(transform2d               global_pos,
                                   int                       health,
                                   float                     speed,
                                   float                     size,
                                   std::chrono::milliseconds attack_delay)
    : enemy(global_pos,
            health,
            speed,
            size,
            game_object_state::idle,
            attack_delay)
{
    animation* anim_skeleton_warrior_idle =
        new animation(resources::skeleton_warrior_idle, 7, 125ms);
    animation* anim_skeleton_warrior_run =
        new animation(resources::skeleton_warrior_run, 8, 125ms);
    animation* anim_skeleton_warrior_attack =
        new animation(resources::skeleton_warrior_attack, 4, 125ms);
    animation* anim_skeleton_warrior_dead =
        new animation(resources::skeleton_warrior_dead, 4, 125ms);

    sprites.emplace(game_object_state::idle, anim_skeleton_warrior_idle);
    sprites.emplace(game_object_state::move, anim_skeleton_warrior_run);
    sprites.emplace(game_object_state::attack, anim_skeleton_warrior_attack);
    sprites.emplace(game_object_state::dead, anim_skeleton_warrior_dead);

    collision::collider* idle_hitbox = new collision::collider(
        { -15.f, -26.f }, { 34.f, 58.f }, { e_color::GREEN, 0.6f }, size);
    collision::collider* run_hitbox = new collision::collider(
        { -19.f, -26.f }, { 34.f, 58.f }, { e_color::GREEN, 0.6f }, size);
    collision::collider* attack_hitbox = new collision::collider(
        { -23.f, -24.f }, { 40.f, 62.f }, { e_color::GREEN, 0.6f }, size);

    hitboxes.emplace(game_object_state::idle, idle_hitbox);
    hitboxes.emplace(game_object_state::move, run_hitbox);
    hitboxes.emplace(game_object_state::attack, attack_hitbox);

    attack_collider = new collision::collider{
        { 17.f, -24.f }, { 40.f, 62.f }, { e_color::ORANGE, 0.6f }, size
    };
    attack_trigger = new collision::collider{
        { -22.f, -50.f }, { 70.f, 100.f }, { e_color::YELLOW, 0.6f }, size
    };
    agro_trigger = new collision::collider{
        { -300.f, -50.f }, { 600.f, 100.f }, { e_color::RED, 0.6f }, 1.f
    };
}

skeleton_spearman::skeleton_spearman(transform2d               global_pos,
                                     int                       health,
                                     float                     speed,
                                     float                     size,
                                     std::chrono::milliseconds attack_delay,
                                     float                     patrol_area,
                                     std::chrono::milliseconds patrol_time)
    : enemy(global_pos,
            health,
            speed,
            size,
            game_object_state::move,
            attack_delay)
    , patrol_area(patrol_area)
    , patrol_area_dt(patrol_area / 2.f)
    , patrol_time(patrol_time)
    , patrol_time_dt(0ms)
{
    animation* anim_skeleton_spearman_idle =
        new animation(resources::skeleton_spearman_idle, 7, 125ms);
    animation* anim_skeleton_spearman_walk =
        new animation(resources::skeleton_spearman_walk, 7, 125ms);
    animation* anim_skeleton_spearman_attack =
        new animation(resources::skeleton_spearman_attack, 4, 125ms);
    animation* anim_skeleton_spearman_dead =
        new animation(resources::skeleton_spearman_dead, 5, 125ms);

    sprites.emplace(game_object_state::idle, anim_skeleton_spearman_idle);
    sprites.emplace(game_object_state::move, anim_skeleton_spearman_walk);
    sprites.emplace(game_object_state::attack, anim_skeleton_spearman_attack);
    sprites.emplace(game_object_state::dead, anim_skeleton_spearman_dead);

    collision::collider* idle_hitbox = new collision::collider(
        { -14.f, -23.f }, { 28.f, 64.f }, { e_color::GREEN, 0.6f }, size);
    collision::collider* walk_hitbox = new collision::collider(
        { -16.f, -17.f }, { 24.f, 64.f }, { e_color::GREEN, 0.6f }, size);
    collision::collider* attack_hitbox = new collision::collider(
        { -32.f, -26.f }, { 32.f, 54.f }, { e_color::GREEN, 0.6f }, size);

    hitboxes.emplace(game_object_state::idle, idle_hitbox);
    hitboxes.emplace(game_object_state::move, walk_hitbox);
    hitboxes.emplace(game_object_state::attack, attack_hitbox);

    attack_collider = new collision::collider{
        { 0.f, -14.f }, { 52.f, 20.f }, { e_color::ORANGE, 0.6f }, size
    };
    attack_trigger = new collision::collider{
        { -35.f, -50.f }, { 70.f, 100.f }, { e_color::YELLOW, 0.6f }, size
    };
}

void skeleton_warrior::update(game_object*              hero,
                              std::chrono::milliseconds delta_time)
{
    if (is_alive())
    {
        if (agro)
        {
            change_direction(global_pos.x < hero->get_global_pos().x ? 0 : 1);
            if (collision::game_object_with_game_object(
                    global_pos,
                    attack_trigger->get_rectangle(),
                    hero->get_global_pos(),
                    hero->get_collider()->get_rectangle()) ||
                state == game_object_state::attack)
                attack(hero, delta_time);
            else if (state != game_object_state::attack)
            {
                move();
                attack_delay_dt = 0ms;
            }
        }
        else if (collision::game_object_with_game_object(
                     global_pos,
                     agro_trigger->get_rectangle(),
                     hero->get_global_pos(),
                     hero->get_collider()->get_rectangle()))
            agro = true;
    }
    else if (get_animation()->get_current_frame_number() ==
             get_animation()->get_frames_quantity() - 1)
        return;

    get_animation()->play(delta_time);
}

void skeleton_spearman::update(game_object*              hero,
                               std::chrono::milliseconds delta_time)
{
    if (is_alive())
    {
        if (collision::game_object_with_game_object(
                global_pos,
                attack_trigger->get_rectangle(),
                hero->get_global_pos(),
                hero->get_collider()->get_rectangle()) ||
            state == game_object_state::attack)
        {
            change_direction(global_pos.x < hero->get_global_pos().x ? 0 : 1);
            attack(hero, delta_time);
        }
        else
        {
            change_direction(patrol_direction);
            attack_delay_dt = 0ms;

            if (std::abs(patrol_area_dt) >= patrol_area)
            {
                patrol_time_dt += delta_time;
                if (patrol_time_dt < patrol_time)
                    set_state(game_object_state::idle);
                else
                {
                    patrol_time_dt -= patrol_time;
                    patrol_area_dt   = 0.f;
                    patrol_direction = patrol_direction == 0 ? 1 : 0;
                }
            }
            else
                move();
        }
    }
    else if (get_animation()->get_current_frame_number() ==
             get_animation()->get_frames_quantity() - 1)
        return;

    get_animation()->play(delta_time);
}

void skeleton_spearman::move()
{
    set_state(game_object_state::move);
    if (direction == 0)
    {
        global_pos.x += speed;
        patrol_area_dt += speed;
    }
    else
    {
        global_pos.x -= speed;
        patrol_area_dt -= speed;
    }
}