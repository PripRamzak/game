#include "include/enemy.hxx"
#include "include/resources.hxx"

#include <cmath>

using namespace std::chrono_literals;

static float enemy_size = 1.8f;

enemy::enemy(prip_engine::transform2d  global_pos,
             float                     speed,
             float                     size,
             int                       direction,
             map*                      level_map,
             int                       health,
             character_state           state,
             hero*                     target,
             std::chrono::milliseconds attack_delay)
    : character(global_pos, speed, size, direction, level_map, health, state)
    , target(target)
    , invulnerability_cooldown(200ms)
    , invulnerability_cooldown_dt(0ms)
    , attack_delay(attack_delay)
    , attack_delay_dt(0ms)
{
}

void enemy::draw()
{
    character::draw();

    if (state != character_state::dead)
        attack_trigger->draw();
}

void enemy::move()
{
    set_state(character_state::move);
    if (direction == 0)
        global_pos.x += speed;
    else
        global_pos.x -= speed;
}

enemy::~enemy()
{
    delete attack_trigger;
}

void enemy::attack(std::chrono::milliseconds delta_time)
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
        if (attack_collider->detect(target->get_hitbox()))
            target->hurt();
        set_state(character_state::idle);
    }
}

void enemy::is_damaged(std::chrono::milliseconds delta_time)
{
    if (target->is_attacked() && !damaged)
        if (get_hitbox()->detect(target->get_attack_collider()))
        {
            hurt();
            damaged = true;
        }

    if (damaged)
    {
        invulnerability_cooldown_dt += delta_time;
        if (invulnerability_cooldown_dt >= invulnerability_cooldown)
        {
            invulnerability_cooldown_dt -= invulnerability_cooldown;
            damaged = false;
        }
    }
}

skeleton_warrior::skeleton_warrior(prip_engine::transform2d global_pos,
                                   int                      direction,
                                   map*                     level_map,
                                   hero*                    target)
    : enemy(global_pos,
            8.f,
            enemy_size,
            direction,
            level_map,
            6,
            character_state::idle,
            target,
            1000ms)
{
    prip_engine::animation* anim_skeleton_warrior_idle =
        new prip_engine::animation(resources::skeleton_warrior_idle, 7, 100ms);
    prip_engine::animation* anim_skeleton_warrior_run =
        new prip_engine::animation(resources::skeleton_warrior_run, 8, 100ms);
    prip_engine::animation* anim_skeleton_warrior_attack =
        new prip_engine::animation(
            resources::skeleton_warrior_attack, 4, 100ms);
    prip_engine::animation* anim_skeleton_warrior_dead =
        new prip_engine::animation(resources::skeleton_warrior_dead, 4, 100ms);

    sprites.emplace(character_state::idle, anim_skeleton_warrior_idle);
    sprites.emplace(character_state::move, anim_skeleton_warrior_run);
    sprites.emplace(character_state::melee_attack,
                    anim_skeleton_warrior_attack);
    sprites.emplace(character_state::dead, anim_skeleton_warrior_dead);

    prip_engine::collider* idle_hitbox =
        new prip_engine::collider(this->global_pos,
                                  { -15.f, -26.f },
                                  { 34.f, 58.f },
                                  { prip_engine::e_color::GREEN, 0.6f },
                                  size,
                                  direction,
                                  true);
    prip_engine::collider* run_hitbox =
        new prip_engine::collider(this->global_pos,
                                  { -19.f, -26.f },
                                  { 34.f, 58.f },
                                  { prip_engine::e_color::GREEN, 0.6f },
                                  size,
                                  direction,
                                  true);
    prip_engine::collider* attack_hitbox =
        new prip_engine::collider(this->global_pos,
                                  { -23.f, -30.f },
                                  { 40.f, 62.f },
                                  { prip_engine::e_color::GREEN, 0.6f },
                                  size,
                                  direction,
                                  true);

    hitboxes.emplace(character_state::idle, idle_hitbox);
    hitboxes.emplace(character_state::move, run_hitbox);
    hitboxes.emplace(character_state::melee_attack, attack_hitbox);

    attack_collider =
        new prip_engine::collider{ this->global_pos,
                                   { 17.f, -30.f },
                                   { 40.f, 62.f },
                                   { prip_engine::e_color::ORANGE, 0.6f },
                                   size,
                                   direction,
                                   true };
    attack_trigger =
        new prip_engine::collider{ this->global_pos,
                                   { -22.f, -50.f },
                                   { 70.f, 100.f },
                                   { prip_engine::e_color::YELLOW, 0.6f },
                                   size,
                                   direction,
                                   true };
    agro_trigger =
        new prip_engine::collider{ this->global_pos,
                                   { -300.f, -50.f },
                                   { 600.f, 100.f },
                                   { prip_engine::e_color::RED, 0.6f },
                                   1.f,
                                   direction,
                                   true };
}

void skeleton_warrior::update(std::chrono::milliseconds delta_time)
{
    if (is_alive())
    {
        is_damaged(delta_time);

        if (agro)
        {
            change_direction(global_pos.x < target->get_global_pos().x ? 0 : 1);
            attack_trigger->change_pos(direction);

            if (attack_trigger->detect(target->get_hitbox()) ||
                state == character_state::melee_attack)
                attack(delta_time);
            else if (state != character_state::melee_attack)
            {
                move();
                attack_delay_dt = 0ms;
            }
        }
        else if (agro_trigger->detect(target->get_hitbox()))
            agro = true;
    }
    else if (get_animation()->get_current_frame_number() ==
             get_animation()->get_frames_quantity() - 1)
        return;

    get_animation()->play(delta_time);
}

void skeleton_warrior::draw()
{
    enemy::draw();

    if (state != character_state::dead)
        agro_trigger->draw();
}

skeleton_warrior::~skeleton_warrior()
{
    delete agro_trigger;
}

skeleton_spearman::skeleton_spearman(prip_engine::transform2d global_pos,
                                     int                      direction,
                                     map*                     level_map,
                                     hero*                    target)
    : enemy(global_pos,
            3.f,
            enemy_size,
            direction,
            level_map,
            4,
            character_state::move,
            target,
            1000ms)
    , patrol_area(400.f)
    , patrol_area_dt(400.f / 2.f)
    , patrol_time(2000ms)
    , patrol_time_dt(0ms)
{
    prip_engine::animation* anim_skeleton_spearman_idle =
        new prip_engine::animation(resources::skeleton_spearman_idle, 7, 100ms);
    prip_engine::animation* anim_skeleton_spearman_walk =
        new prip_engine::animation(resources::skeleton_spearman_walk, 7, 100ms);
    prip_engine::animation* anim_skeleton_spearman_attack =
        new prip_engine::animation(
            resources::skeleton_spearman_attack, 4, 100ms);
    prip_engine::animation* anim_skeleton_spearman_dead =
        new prip_engine::animation(resources::skeleton_spearman_dead, 5, 100ms);

    sprites.emplace(character_state::idle, anim_skeleton_spearman_idle);
    sprites.emplace(character_state::move, anim_skeleton_spearman_walk);
    sprites.emplace(character_state::melee_attack,
                    anim_skeleton_spearman_attack);
    sprites.emplace(character_state::dead, anim_skeleton_spearman_dead);

    prip_engine::collider* idle_hitbox =
        new prip_engine::collider(this->global_pos,
                                  { -14.f, -16.f },
                                  { 28.f, 64.f },
                                  { prip_engine::e_color::GREEN, 0.6f },
                                  size,
                                  direction,
                                  true);
    prip_engine::collider* walk_hitbox =
        new prip_engine::collider(this->global_pos,
                                  { -16.f, -17.f },
                                  { 24.f, 64.f },
                                  { prip_engine::e_color::GREEN, 0.6f },
                                  size,
                                  direction,
                                  true);
    prip_engine::collider* attack_hitbox =
        new prip_engine::collider(this->global_pos,
                                  { -32.f, -8.f },
                                  { 32.f, 54.f },
                                  { prip_engine::e_color::GREEN, 0.6f },
                                  size,
                                  direction,
                                  true);

    hitboxes.emplace(character_state::idle, idle_hitbox);
    hitboxes.emplace(character_state::move, walk_hitbox);
    hitboxes.emplace(character_state::melee_attack, attack_hitbox);

    attack_collider =
        new prip_engine::collider{ this->global_pos,
                                   { 0.f, 5.f },
                                   { 52.f, 20.f },
                                   { prip_engine::e_color::ORANGE, 0.6f },
                                   size,
                                   direction,
                                   true };
    attack_trigger =
        new prip_engine::collider{ this->global_pos,
                                   { -35.f, -50.f },
                                   { 70.f, 100.f },
                                   { prip_engine::e_color::YELLOW, 0.6f },
                                   size,
                                   direction,
                                   true };
}

void skeleton_spearman::update(std::chrono::milliseconds delta_time)
{
    if (is_alive())
    {
        is_damaged(delta_time);

        if (attack_trigger->detect(target->get_hitbox()) ||
            state == character_state::melee_attack)
        {
            change_direction(global_pos.x < target->get_global_pos().x ? 0 : 1);
            attack_trigger->change_pos(direction);
            attack(delta_time);
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

skeleton_spearman::~skeleton_spearman() = default;

void skeleton_spearman::move()
{
    enemy::move();
    if (direction == 0)
        patrol_area_dt += speed;
    else
        patrol_area_dt -= speed;
}

skeleton_archer::skeleton_archer(prip_engine::transform2d global_pos,
                                 int                      direction,
                                 map*                     level_map,
                                 hero*                    target)
    : enemy(global_pos,
            12.f,
            enemy_size,
            direction,
            level_map,
            3,
            character_state::idle,
            target,
            1000ms)
    , shot_cooldown(1200ms)
    , shot_cooldown_dt(0ms)
{
    prip_engine::animation* anim_skeleton_archer_idle =
        new prip_engine::animation(resources::skeleton_archer_idle, 7, 100ms);
    prip_engine::animation* anim_skeleton_archer_walk =
        new prip_engine::animation(resources::skeleton_archer_walk, 8, 100ms);
    prip_engine::animation* anim_skeleton_archer_attack =
        new prip_engine::animation(resources::skeleton_archer_attack, 4, 100ms);
    prip_engine::animation* anim_skeleton_archer_shot =
        new prip_engine::animation(resources::skeleton_archer_shot, 15, 60ms);
    prip_engine::animation* anim_skeleton_archer_dead =
        new prip_engine::animation(resources::skeleton_archer_dead, 5, 100ms);

    sprites.emplace(character_state::idle, anim_skeleton_archer_idle);
    sprites.emplace(character_state::move, anim_skeleton_archer_walk);
    sprites.emplace(character_state::melee_attack, anim_skeleton_archer_attack);
    sprites.emplace(character_state::range_attack, anim_skeleton_archer_shot);
    sprites.emplace(character_state::dead, anim_skeleton_archer_dead);

    prip_engine::collider* idle_hitbox =
        new prip_engine::collider(this->global_pos,
                                  { -18.f, -32.f },
                                  { 28.f, 64.f },
                                  { prip_engine::e_color::GREEN, 0.6f },
                                  size,
                                  direction,
                                  true);
    prip_engine::collider* walk_hitbox =
        new prip_engine::collider(this->global_pos,
                                  { -12.f, -32.f },
                                  { 24.f, 64.f },
                                  { prip_engine::e_color::GREEN, 0.6f },
                                  size,
                                  direction,
                                  true);
    prip_engine::collider* attack_hitbox =
        new prip_engine::collider(this->global_pos,
                                  { -22.f, -33.f },
                                  { 30.f, 66.f },
                                  { prip_engine::e_color::GREEN, 0.6f },
                                  size,
                                  direction,
                                  true);
    prip_engine::collider* shot_hitbox =
        new prip_engine::collider(this->global_pos,
                                  { -18.f, -34.f },
                                  { 28.f, 66.f },
                                  { prip_engine::e_color::GREEN, 0.6f },
                                  size,
                                  direction,
                                  true);

    hitboxes.emplace(character_state::idle, idle_hitbox);
    hitboxes.emplace(character_state::move, walk_hitbox);
    hitboxes.emplace(character_state::melee_attack, attack_hitbox);
    hitboxes.emplace(character_state::range_attack, shot_hitbox);

    attack_collider =
        new prip_engine::collider(this->global_pos,
                                  { 10.f, -5.f },
                                  { 24.f, 24.f },
                                  { prip_engine::e_color::ORANGE, 0.6f },
                                  size,
                                  direction,
                                  true);
    attack_trigger =
        new prip_engine::collider(this->global_pos,
                                  { -20.f, -33.f },
                                  { 40.f, 66.f },
                                  { prip_engine::e_color::YELLOW, 0.6f },
                                  size,
                                  direction,
                                  true);
    shot_trigger =
        new prip_engine::collider(this->global_pos,
                                  { -500.f, -33.f },
                                  { 1000.f, 66.f },
                                  { prip_engine::e_color::RED, 0.6f },
                                  1.f,
                                  direction,
                                  true);
}

void skeleton_archer::update(std::chrono::milliseconds delta_time)
{
    if (is_alive())
    {
        is_damaged(delta_time);

        if (attack_trigger->detect(target->get_hitbox()) ||
            state == character_state::melee_attack)
        {
            change_direction(global_pos.x < target->get_global_pos().x ? 0 : 1);
            attack(delta_time);
        }
        else if (shot_trigger->detect(target->get_hitbox()) ||
                 state == character_state::range_attack)
        {
            change_direction(global_pos.x < target->get_global_pos().x ? 0 : 1);
            shoot(target, delta_time);
        }
    }
    else if (get_animation()->get_current_frame_number() ==
             get_animation()->get_frames_quantity() - 1)
        return;

    for (auto it = arrows.begin(); it != arrows.end();)
    {
        (*it)->update(target);
        if ((*it)->is_destroyed())
            it = arrows.erase(it);
        else
            ++it;
    }

    get_animation()->play(delta_time);
}

void skeleton_archer::draw()
{
    for (auto it = arrows.begin(); it != arrows.end(); ++it)
        (*it)->draw();

    enemy::draw();

    if (state != character_state::dead)
        shot_trigger->draw();
}

skeleton_archer::~skeleton_archer()
{
    delete shot_trigger;
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