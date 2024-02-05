#include "include/enemy.hxx"

#include <cmath>
#include <iostream>

using namespace std::chrono_literals;

static animation* skeleton_warrior_idle_anim   = nullptr;
static animation* skeleton_warrior_run_anim    = nullptr;
static animation* skeleton_warrior_attack_anim = nullptr;

static animation* skeleton_spearman_idle_anim   = nullptr;
static animation* skeleton_spearman_run_anim    = nullptr;
static animation* skeleton_spearman_attack_anim = nullptr;

static bool enemy_init = false;

enemy::enemy(int                       health,
             float                     speed,
             float                     global_pos_x,
             float                     global_pos_y,
             float                     size,
             game_object_state         state,
             enemy_type                type,
             float                     patrol_area,
             std::chrono::milliseconds patrol_time,
             std::chrono::milliseconds attack_delay)
    : game_object(health, speed, global_pos_x, global_pos_y, size, state)
    , patrol_direction(direction)
    , patrol_area(patrol_area)
    , patrol_area_dt(patrol_area / 2.f)
    , patrol_time(patrol_time)
    , patrol_time_dt(0ms)
    , attack_delay(attack_delay)
    , attack_delay_dt(0ms)
{
    if (type == enemy_type::warrior)
    {
        sprites.emplace(game_object_state::idle, skeleton_warrior_idle_anim);
        sprites.emplace(game_object_state::run, skeleton_warrior_run_anim);
        sprites.emplace(game_object_state::attack,
                        skeleton_warrior_attack_anim);
    }
    else
    {
        sprites.emplace(game_object_state::idle, skeleton_spearman_idle_anim);
        sprites.emplace(game_object_state::run, skeleton_spearman_run_anim);
        sprites.emplace(game_object_state::attack,
                        skeleton_spearman_attack_anim);
    }
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
            new sprite(skeleton_warrior_idle_sprite_sheet, 64.f, 64.f);
        sprite* skeleton_warrior_run_sprite =
            new sprite(skeleton_warrior_run_sprite_sheet, 74.f, 64.f);
        sprite* skeleton_warrior_attack_sprite =
            new sprite(skeleton_warrior_attack_sprite_sheet, 96.f, 64.f);

        skeleton_warrior_idle_anim =
            new animation(skeleton_warrior_idle_sprite, 7, 32.f, 150ms);
        skeleton_warrior_run_anim =
            new animation(skeleton_warrior_run_sprite, 8, 32.f, 150ms);
        skeleton_warrior_attack_anim =
            new animation(skeleton_warrior_attack_sprite, 4, 16.f, 150ms);

        texture* skeleton_spearman_idle_sprite_sheet =
            create_texture("img/skeleton_spearman_idle.png");
        texture* skeleton_spearman_run_sprite_sheet =
            create_texture("img/skeleton_spearman_run.png");
        texture* skeleton_spearman_attack_sprite_sheet =
            create_texture("img/skeleton_spearman_attack.png");

        sprite* skeleton_spearman_idle_sprite =
            new sprite(skeleton_spearman_idle_sprite_sheet, 64.f, 64.f);
        sprite* skeleton_spearman_run_sprite =
            new sprite(skeleton_spearman_run_sprite_sheet, 88.f, 64.f);
        sprite* skeleton_spearman_attack_sprite =
            new sprite(skeleton_spearman_attack_sprite_sheet, 128.f, 64.f);

        skeleton_spearman_idle_anim =
            new animation(skeleton_spearman_idle_sprite, 7, 32.f, 200ms);
        skeleton_spearman_run_anim =
            new animation(skeleton_spearman_run_sprite, 6, 20.f, 150ms);
        skeleton_spearman_attack_anim =
            new animation(skeleton_spearman_attack_sprite, 4, 0.f, 250ms);

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

void enemy::move(game_object* hero, std::chrono::milliseconds delta_time)
{
    if (collision::game_object_with_game_object(this, hero))
    {
        direction = global_pos_x < hero->get_global_pos_x() ? 0 : 1;
        attack(hero, delta_time);
        return;
    }
    else
    {
        direction       = patrol_direction;
        attack_delay_dt = 0ms;
    }

    if (std::abs(patrol_area_dt) >= patrol_area)
    {
        patrol_time_dt += delta_time;
        if (patrol_time_dt < patrol_time)
        {
            set_state(game_object_state::idle);
            return;
        }

        patrol_time_dt -= patrol_time;
        patrol_area_dt   = 0.f;
        patrol_direction = patrol_direction == 0 ? 1 : 0;
        direction        = patrol_direction;
    }
    else if (state != game_object_state::attack)
    {
        set_state(game_object_state::run);
        direction = patrol_direction;
    }

    if (state == game_object_state::run)
    {
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

    animation* attack_anim         = sprites[state];
    int        anim_current_number = attack_anim->get_current_frame_number();
    int        anim_quantity       = attack_anim->get_quantity();

    if (anim_current_number == anim_quantity - 1)
    {
        if (collision::game_object_with_game_object(this, hero))
            hero->hurt();
        set_state(game_object_state::idle);
    }
}

enemy::~enemy() = default;