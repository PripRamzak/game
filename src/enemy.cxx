#include "include/enemy.hxx"

#include <cmath>
#include <iostream>

using namespace std::chrono_literals;

static animation* skeleton_warrior_idle_anim   = nullptr;
static animation* skeleton_warrior_run_anim    = nullptr;
static animation* skeleton_warrior_attack_anim = nullptr;

static animation* skeleton_spearman_idle_anim   = nullptr;
static animation* skeleton_spearman_walk_anim   = nullptr;
static animation* skeleton_spearman_attack_anim = nullptr;

static bool enemy_init = false;

enemy::enemy(int                       health,
             float                     speed,
             float                     global_pos_x,
             float                     global_pos_y,
             float                     size,
             game_object_state         state,
             std::chrono::milliseconds attack_delay)
    : game_object(health, speed, global_pos_x, global_pos_y, size, state)
    , attack_delay(attack_delay)
    , attack_delay_dt(0ms)
{
}

void enemy::initialize()
{
    using namespace std::chrono_literals;

    if (!enemy_init)
    {
        texture* skeleton_warrior_idle_sprite_sheet =
            create_texture("img/skeleton_warrior_idle.png");
        texture* skeleton_warrior_run_sprite_sheet =
            create_texture("img/skeleton_warrior_run.png");
        texture* skeleton_warrior_attack_sprite_sheet =
            create_texture("img/skeleton_warrior_attack.png");

        sprite* skeleton_warrior_idle_sprite =
            new sprite(skeleton_warrior_idle_sprite_sheet, 58.f, 64.f);
        sprite* skeleton_warrior_run_sprite =
            new sprite(skeleton_warrior_run_sprite_sheet, 70.f, 64.f);
        sprite* skeleton_warrior_attack_sprite =
            new sprite(skeleton_warrior_attack_sprite_sheet, 128.f, 76.f);

        skeleton_warrior_idle_anim =
            new animation(skeleton_warrior_idle_sprite, 7, 125ms);
        skeleton_warrior_run_anim =
            new animation(skeleton_warrior_run_sprite, 8, 125ms);
        skeleton_warrior_attack_anim =
            new animation(skeleton_warrior_attack_sprite, 4, 125ms);

        texture* skeleton_spearman_idle_sprite_sheet =
            create_texture("img/skeleton_spearman_idle.png");
        texture* skeleton_spearman_walk_sprite_sheet =
            create_texture("img/skeleton_spearman_walk.png");
        texture* skeleton_spearman_attack_sprite_sheet =
            create_texture("img/skeleton_spearman_attack.png");

        sprite* skeleton_spearman_idle_sprite =
            new sprite(skeleton_spearman_idle_sprite_sheet, 36.f, 82.f);
        sprite* skeleton_spearman_walk_sprite =
            new sprite(skeleton_spearman_walk_sprite_sheet, 64.f, 94.f);
        sprite* skeleton_spearman_attack_sprite =
            new sprite(skeleton_spearman_attack_sprite_sheet, 100.f, 56.f);

        skeleton_spearman_idle_anim =
            new animation(skeleton_spearman_idle_sprite, 7, 125ms);
        skeleton_spearman_walk_anim =
            new animation(skeleton_spearman_walk_sprite, 7, 125ms);
        skeleton_spearman_attack_anim =
            new animation(skeleton_spearman_attack_sprite, 4, 125ms);

        enemy_init = true;
    }
    else
        std::cout << "Enemy already init!" << std::endl;
}

void enemy::spawn()
{
    spawned = true;
}

bool enemy::is_spawned()
{
    return spawned;
}

void enemy::move()
{
    set_state(game_object_state::run);
    if (direction == 0)
        global_pos_x += speed;
    else
        global_pos_x -= speed;
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
        if (collision::game_object_with_game_object(this, hero))
            hero->hurt();
        set_state(game_object_state::idle);
    }
}

enemy::~enemy() = default;

skeleton_warrior::skeleton_warrior(int                       health,
                                   float                     speed,
                                   float                     global_pos_x,
                                   float                     global_pos_y,
                                   float                     size,
                                   game_object_state         state,
                                   std::chrono::milliseconds attack_delay,
                                   float                     agro_area)
    : enemy(
          health, speed, global_pos_x, global_pos_y, size, state, attack_delay)
    , agro_area(agro_area)
{
    sprites.emplace(game_object_state::idle, skeleton_warrior_idle_anim);
    sprites.emplace(game_object_state::run, skeleton_warrior_run_anim);
    sprites.emplace(game_object_state::attack, skeleton_warrior_attack_anim);
}

skeleton_spearman::skeleton_spearman(int                       health,
                                     float                     speed,
                                     float                     global_pos_x,
                                     float                     global_pos_y,
                                     float                     size,
                                     game_object_state         state,
                                     std::chrono::milliseconds attack_delay,
                                     float                     patrol_area,
                                     std::chrono::milliseconds patrol_time)
    : enemy(
          health, speed, global_pos_x, global_pos_y, size, state, attack_delay)
    , patrol_area(patrol_area)
    , patrol_area_dt(patrol_area / 2.f)
    , patrol_time(patrol_time)
    , patrol_time_dt(0ms)
{
    sprites.emplace(game_object_state::idle, skeleton_spearman_idle_anim);
    sprites.emplace(game_object_state::walk, skeleton_spearman_walk_anim);
    sprites.emplace(game_object_state::attack, skeleton_spearman_attack_anim);
}

void skeleton_warrior::update(game_object*              hero,
                              std::chrono::milliseconds delta_time)
{
    if (agro)
    {
        direction = global_pos_x < hero->get_global_pos_x() ? 0 : 1;
        if (collision::game_object_with_game_object(this, hero) ||
            state == game_object_state::attack)
            attack(hero, delta_time);
        else if (state != game_object_state::attack)
        {
            move();
            attack_delay_dt = 0ms;
        }
    }
    else if (global_pos_x - agro_area / 2.f <= hero->get_global_pos_x() &&
             global_pos_x + agro_area / 2.f >= hero->get_global_pos_x())
        agro = true;

    get_animation()->play(delta_time);
}

void skeleton_spearman::update(game_object*              hero,
                               std::chrono::milliseconds delta_time)
{
    std::cout << global_pos_x << std::endl;
    if (collision::game_object_with_game_object(this, hero) ||
        state == game_object_state::attack)
    {
        direction = global_pos_x < hero->get_global_pos_x() ? 0 : 1;
        attack(hero, delta_time);
    }
    else
    {
        direction       = patrol_direction;
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
                direction        = patrol_direction;
            }
        }
        else
            move();
    }

    get_animation()->play(delta_time);
}

void skeleton_spearman::move()
{
    set_state(game_object_state::walk);
    if (direction == 0)
    {
        global_pos_x += speed;
        patrol_area_dt += speed;
    }
    else
    {
        global_pos_x -= speed;
        patrol_area_dt -= speed;
    }
}