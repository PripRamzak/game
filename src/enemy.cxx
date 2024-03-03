#include "include/enemy.hxx"
#include "include/resources.hxx"

#include <cmath>

using namespace std::chrono_literals;

enemy::enemy(prip_engine::transform2d  global_pos,
             float                     speed,
             float                     size,
             int                       direction,
             map*                      level_map,
             int                       health,
             character_state           state,
             std::chrono::milliseconds attack_delay)
    : character(global_pos, speed, size, direction, level_map, health, state)
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
    set_state(character_state::move);
    if (direction == 0)
        global_pos.x += speed;
    else
        global_pos.x -= speed;
}

void enemy::attack(character* hero, std::chrono::milliseconds delta_time)
{
    if (state != character_state::melee_attack)
    {
        set_state(character_state::idle);
        attack_delay_dt += delta_time;
        if (attack_delay_dt < attack_delay)
            return;
        attack_delay_dt -= attack_delay;
    }

    set_state(character_state::melee_attack);

    prip_engine::animation* attack_anim = get_animation();
    int anim_current_number = attack_anim->get_current_frame_number();
    int anim_quantity       = attack_anim->get_frames_quantity();

    if (anim_current_number == anim_quantity - 1)
    {
        if (collision::game_object_with_game_object(
                global_pos,
                attack_collider->get_rectangle(),
                hero->get_global_pos(),
                hero->get_collider()->get_rectangle()))
            hero->hurt();
        set_state(character_state::idle);
    }
}

enemy::~enemy() = default;

skeleton_warrior::skeleton_warrior(prip_engine::transform2d  global_pos,
                                   float                     speed,
                                   float                     size,
                                   int                       direction,
                                   map*                      level_map,
                                   int                       health,
                                   std::chrono::milliseconds attack_delay)
    : enemy(global_pos,
            speed,
            size,
            direction,
            level_map,
            health,
            character_state::idle,
            attack_delay)
{
    prip_engine::animation* anim_skeleton_warrior_idle =
        new prip_engine::animation(resources::skeleton_warrior_idle, 7, 125ms);
    prip_engine::animation* anim_skeleton_warrior_run =
        new prip_engine::animation(resources::skeleton_warrior_run, 8, 125ms);
    prip_engine::animation* anim_skeleton_warrior_attack =
        new prip_engine::animation(
            resources::skeleton_warrior_attack, 4, 125ms);
    prip_engine::animation* anim_skeleton_warrior_dead =
        new prip_engine::animation(resources::skeleton_warrior_dead, 4, 125ms);

    sprites.emplace(character_state::idle, anim_skeleton_warrior_idle);
    sprites.emplace(character_state::move, anim_skeleton_warrior_run);
    sprites.emplace(character_state::melee_attack,
                    anim_skeleton_warrior_attack);
    sprites.emplace(character_state::dead, anim_skeleton_warrior_dead);

    collision::collider* idle_hitbox =
        new collision::collider({ -15.f, -26.f },
                                { 34.f, 58.f },
                                { prip_engine::e_color::GREEN, 0.6f },
                                size,
                                direction);
    collision::collider* run_hitbox =
        new collision::collider({ -19.f, -26.f },
                                { 34.f, 58.f },
                                { prip_engine::e_color::GREEN, 0.6f },
                                size,
                                direction);
    collision::collider* attack_hitbox =
        new collision::collider({ -23.f, -30.f },
                                { 40.f, 62.f },
                                { prip_engine::e_color::GREEN, 0.6f },
                                size,
                                direction);

    hitboxes.emplace(character_state::idle, idle_hitbox);
    hitboxes.emplace(character_state::move, run_hitbox);
    hitboxes.emplace(character_state::melee_attack, attack_hitbox);

    attack_collider =
        new collision::collider{ { 17.f, -30.f },
                                 { 40.f, 62.f },
                                 { prip_engine::e_color::ORANGE, 0.6f },
                                 size,
                                 direction };
    attack_trigger =
        new collision::collider{ { -22.f, -50.f },
                                 { 70.f, 100.f },
                                 { prip_engine::e_color::YELLOW, 0.6f },
                                 size,
                                 direction };
    agro_trigger = new collision::collider{ { -300.f, -50.f },
                                            { 600.f, 100.f },
                                            { prip_engine::e_color::RED, 0.6f },
                                            1.f,
                                            direction };
}

skeleton_spearman::skeleton_spearman(prip_engine::transform2d  global_pos,
                                     float                     speed,
                                     float                     size,
                                     int                       direction,
                                     map*                      level_map,
                                     int                       health,
                                     std::chrono::milliseconds attack_delay,
                                     float                     patrol_area,
                                     std::chrono::milliseconds patrol_time)
    : enemy(global_pos,
            speed,
            size,
            direction,
            level_map,
            health,
            character_state::move,
            attack_delay)
    , patrol_area(patrol_area)
    , patrol_area_dt(patrol_area / 2.f)
    , patrol_time(patrol_time)
    , patrol_time_dt(0ms)
{
    prip_engine::animation* anim_skeleton_spearman_idle =
        new prip_engine::animation(resources::skeleton_spearman_idle, 7, 125ms);
    prip_engine::animation* anim_skeleton_spearman_walk =
        new prip_engine::animation(resources::skeleton_spearman_walk, 7, 125ms);
    prip_engine::animation* anim_skeleton_spearman_attack =
        new prip_engine::animation(
            resources::skeleton_spearman_attack, 4, 125ms);
    prip_engine::animation* anim_skeleton_spearman_dead =
        new prip_engine::animation(resources::skeleton_spearman_dead, 5, 125ms);

    sprites.emplace(character_state::idle, anim_skeleton_spearman_idle);
    sprites.emplace(character_state::move, anim_skeleton_spearman_walk);
    sprites.emplace(character_state::melee_attack,
                    anim_skeleton_spearman_attack);
    sprites.emplace(character_state::dead, anim_skeleton_spearman_dead);

    collision::collider* idle_hitbox =
        new collision::collider({ -14.f, -16.f },
                                { 28.f, 64.f },
                                { prip_engine::e_color::GREEN, 0.6f },
                                size,
                                direction);
    collision::collider* walk_hitbox =
        new collision::collider({ -16.f, -17.f },
                                { 24.f, 64.f },
                                { prip_engine::e_color::GREEN, 0.6f },
                                size,
                                direction);
    collision::collider* attack_hitbox =
        new collision::collider({ -32.f, -8.f },
                                { 32.f, 54.f },
                                { prip_engine::e_color::GREEN, 0.6f },
                                size,
                                direction);

    hitboxes.emplace(character_state::idle, idle_hitbox);
    hitboxes.emplace(character_state::move, walk_hitbox);
    hitboxes.emplace(character_state::melee_attack, attack_hitbox);

    attack_collider =
        new collision::collider{ { 0.f, 5.f },
                                 { 52.f, 20.f },
                                 { prip_engine::e_color::ORANGE, 0.6f },
                                 size,
                                 direction };
    attack_trigger =
        new collision::collider{ { -35.f, -50.f },
                                 { 70.f, 100.f },
                                 { prip_engine::e_color::YELLOW, 0.6f },
                                 size,
                                 direction };
}

skeleton_archer::skeleton_archer(prip_engine::transform2d  global_pos,
                                 float                     speed,
                                 float                     size,
                                 int                       direction,
                                 map*                      level_map,
                                 int                       health,
                                 std::chrono::milliseconds attack_delay)
    : enemy(global_pos,
            speed,
            size,
            direction,
            level_map,
            health,
            character_state::range_attack,
            attack_delay)
    , shot_cooldown(1500ms)
    , shot_cooldown_dt(0ms)
{
    prip_engine::animation* anim_skeleton_archer_idle =
        new prip_engine::animation(resources::skeleton_archer_idle, 7, 125ms);
    prip_engine::animation* anim_skeleton_archer_walk =
        new prip_engine::animation(resources::skeleton_archer_walk, 8, 125ms);
    prip_engine::animation* anim_skeleton_archer_attack =
        new prip_engine::animation(resources::skeleton_archer_attack, 4, 125ms);
    prip_engine::animation* anim_skeleton_archer_shot =
        new prip_engine::animation(resources::skeleton_archer_shot, 15, 60ms);
    prip_engine::animation* anim_skeleton_archer_dead =
        new prip_engine::animation(resources::skeleton_archer_dead, 5, 125ms);

    sprites.emplace(character_state::idle, anim_skeleton_archer_idle);
    sprites.emplace(character_state::move, anim_skeleton_archer_walk);
    sprites.emplace(character_state::melee_attack, anim_skeleton_archer_attack);
    sprites.emplace(character_state::range_attack, anim_skeleton_archer_shot);
    sprites.emplace(character_state::dead, anim_skeleton_archer_dead);

    collision::collider* idle_hitbox =
        new collision::collider({ -18.f, -32.f },
                                { 28.f, 64.f },
                                { prip_engine::e_color::GREEN, 0.6f },
                                size,
                                direction);
    collision::collider* walk_hitbox =
        new collision::collider({ -12.f, -32.f },
                                { 24.f, 64.f },
                                { prip_engine::e_color::GREEN, 0.6f },
                                size,
                                direction);
    collision::collider* attack_hitbox =
        new collision::collider({ -22.f, -33.f },
                                { 30.f, 66.f },
                                { prip_engine::e_color::GREEN, 0.6f },
                                size,
                                direction);
    collision::collider* shot_hitbox =
        new collision::collider({ -18.f, -34.f },
                                { 28.f, 66.f },
                                { prip_engine::e_color::GREEN, 0.6f },
                                size,
                                direction);

    hitboxes.emplace(character_state::idle, idle_hitbox);
    hitboxes.emplace(character_state::move, walk_hitbox);
    hitboxes.emplace(character_state::melee_attack, attack_hitbox);
    hitboxes.emplace(character_state::range_attack, shot_hitbox);

    attack_collider =
        new collision::collider({ 10.f, -5.f },
                                { 24.f, 24.f },
                                { prip_engine::e_color::ORANGE, 0.6f },
                                size,
                                direction);
    attack_trigger =
        new collision::collider({ -20.f, -33.f },
                                { 40.f, 66.f },
                                { prip_engine::e_color::YELLOW, 0.6f },
                                size,
                                direction);
    shot_trigger = new collision::collider({ -500.f, -33.f },
                                           { 1000.f, 66.f },
                                           { prip_engine::e_color::RED, 0.6f },
                                           1.f,
                                           direction);
}

void skeleton_warrior::update(character*                hero,
                              std::chrono::milliseconds delta_time)
{
    if (is_alive())
    {
        if (agro)
        {
            change_direction(global_pos.x < hero->get_global_pos().x ? 0 : 1);
            attack_trigger->change_pos(direction);

            if (collision::game_object_with_game_object(
                    global_pos,
                    attack_trigger->get_rectangle(),
                    hero->get_global_pos(),
                    hero->get_collider()->get_rectangle()) ||
                state == character_state::melee_attack)
                attack(hero, delta_time);
            else if (state != character_state::melee_attack)
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

void skeleton_spearman::update(character*                hero,
                               std::chrono::milliseconds delta_time)
{
    if (is_alive())
    {
        if (collision::game_object_with_game_object(
                global_pos,
                attack_trigger->get_rectangle(),
                hero->get_global_pos(),
                hero->get_collider()->get_rectangle()) ||
            state == character_state::melee_attack)
        {
            change_direction(global_pos.x < hero->get_global_pos().x ? 0 : 1);
            attack_trigger->change_pos(direction);
            attack(hero, delta_time);
        }
        else
        {
            change_direction(patrol_direction);
            attack_trigger->change_pos(direction);
            attack_delay_dt = 0ms;

            if (std::abs(patrol_area_dt) >= patrol_area)
            {
                patrol_time_dt += delta_time;
                if (patrol_time_dt < patrol_time)
                    set_state(character_state::idle);
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
    set_state(character_state::move);
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

void skeleton_archer::update(character*                hero,
                             std::chrono::milliseconds delta_time)
{
    if (is_alive())
    {
        if (collision::game_object_with_game_object(
                global_pos,
                attack_trigger->get_rectangle(),
                hero->get_global_pos(),
                hero->get_collider()->get_rectangle()) ||
            state == character_state::melee_attack)
        {
            change_direction(global_pos.x < hero->get_global_pos().x ? 0 : 1);
            attack(hero, delta_time);
        }
        else if (collision::game_object_with_game_object(
                     global_pos,
                     shot_trigger->get_rectangle(),
                     hero->get_global_pos(),
                     hero->get_collider()->get_rectangle()) ||
                 state == character_state::range_attack)
        {
            change_direction(global_pos.x < hero->get_global_pos().x ? 0 : 1);
            shoot(hero, delta_time);
        }
    }
    else if (get_animation()->get_current_frame_number() ==
             get_animation()->get_frames_quantity() - 1)
        return;

    for (auto it = arrows.begin(); it != arrows.end();)
    {
        (*it)->update(hero);
        if ((*it)->is_destroyed())
            it = arrows.erase(it);
        else
            ++it;
    }

    get_animation()->play(delta_time);
}

void skeleton_archer::shoot(game_object*              hero,
                            std::chrono::milliseconds delta_time)
{
    if (state != character_state::range_attack)
    {
        set_state(character_state::idle);
        shot_cooldown_dt += delta_time;
        if (shot_cooldown_dt < shot_cooldown)
            return;
        shot_cooldown_dt -= shot_cooldown;
    }

    set_state(character_state::range_attack);

    prip_engine::animation* shot_anim = get_animation();
    int anim_current_number           = shot_anim->get_current_frame_number();
    int anim_quantity                 = shot_anim->get_frames_quantity();

    if (anim_current_number == anim_quantity - 3)
    {
        if (direction == 0)
            arrows.emplace_back(
                new arrow({ global_pos.x + 10.f, global_pos.y - 28.f },
                          size,
                          direction,
                          level_map));
        else
            arrows.emplace_back(
                new arrow({ global_pos.x - 10.f, global_pos.y - 28.f },
                          size,
                          direction,
                          level_map));
        set_state(character_state::idle);
    }
}

std::vector<std::unique_ptr<arrow>>& skeleton_archer::get_arrows()
{
    return arrows;
}