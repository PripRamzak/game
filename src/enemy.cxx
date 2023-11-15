#include "include/enemy.hxx"

#include <iostream>

static animation* skeleton_warrior_idle_anim   = nullptr;
static animation* skeleton_warrior_run_anim    = nullptr;
static animation* skeleton_warrior_attack_anim = nullptr;

static animation* skeleton_spearman_idle_anim   = nullptr;
static animation* skeleton_spearman_run_anim    = nullptr;
static animation* skeleton_spearman_attack_anim = nullptr;

static bool enemy_init = false;

enemy::enemy(int               health,
             float             speed,
             float             global_pos_x,
             float             global_pos_y,
             float             size,
             game_object_state state,
             enemy_type        type)
    : game_object(health, speed, global_pos_x, global_pos_y, size, state)
{
    if (type == enemy_type::warrior)
    {
        add_sprite(skeleton_warrior_idle_anim, game_object_state::idle);
        add_sprite(skeleton_warrior_run_anim, game_object_state::run);
        add_sprite(skeleton_warrior_attack_anim, game_object_state::attack);
    }
    else
    {
        add_sprite(skeleton_spearman_idle_anim, game_object_state::idle);
        add_sprite(skeleton_spearman_run_anim, game_object_state::run);
        add_sprite(skeleton_spearman_attack_anim, game_object_state::attack);
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
            new animation(skeleton_warrior_attack_sprite, 4, 16.f, 200ms);

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

void enemy::move(game_object* hero)
{
    bool hero_collision_x = check_hero_collision_x(hero);

    if (!hero_collision_x)
    {
        if (hero->get_global_pos_x() < get_global_pos_x())
        {
            global_pos_x -= speed;
            direction = 1;
        }
        else
        {
            global_pos_x += speed;
            direction = 0;
        }

        set_state(game_object_state::run);
    }
    else if (!check_hero_collision_y(hero))
    {
        if (hero->get_global_pos_y() > get_global_pos_y())
            global_pos_y += speed;
        else
            global_pos_y -= speed;

        set_state(game_object_state::run);
    }
    else if (hero->get_global_pos_y() - 3.5f > get_global_pos_y() ||
             hero->get_global_pos_y() + 3.5f < get_global_pos_y())
    {
        if (hero->get_global_pos_x() <= get_global_pos_x())
        {
            global_pos_x += speed;
            direction = 0;
        }
        else
        {
            global_pos_x -= speed;
            direction = 1;
        }

        if (!check_hero_collision_x(hero))
        {
            if (hero->get_global_pos_x() <= get_global_pos_x())
            {
                global_pos_x -= speed;
                direction = 1;
            }
            else
            {
                global_pos_x += speed;
                direction = 0;
            }

            if (hero->get_global_pos_y() - 3.5f > get_global_pos_y())
                global_pos_y += speed;
            else if (hero->get_global_pos_y() + 3.5f < get_global_pos_y())
                global_pos_y -= speed;
        }

        set_state(game_object_state::run);
    }
    else if (state == game_object_state::idle)
    {
        auto it = find_sprite(state);
        int  sprite_current_number =
            it->game_object_anim_sprite->get_current_number(direction);
        int sprite_quantity = it->game_object_anim_sprite->get_quantity();

        if (sprite_current_number == sprite_quantity - 1 && direction == 0 ||
            sprite_current_number == 0 && direction == 1)
            set_state(game_object_state::attack);
    }
    else
        set_state(game_object_state::idle);
}

void enemy::attack(game_object* hero)
{
    auto it = find_sprite(state);
    int  sprite_current_number =
        it->game_object_anim_sprite->get_current_number(direction);
    int sprite_quantity = it->game_object_anim_sprite->get_quantity();

    // End of attack

    if ((sprite_current_number == 0 && direction == 0 ||
         sprite_current_number == sprite_quantity - 1 && direction == 1) &&
        attacked)
    {
        set_state(game_object_state::idle);
        attacked = false;
        return;
    }

    // Hit moment

    if ((sprite_current_number == sprite_quantity - 1 && direction == 0 ||
         sprite_current_number == 0 && direction == 1) &&
        !attacked)
    {
        attacked = true;
        if (check_hero_collision_x(hero) && check_hero_collision_y(hero))
            hero->hurt();
    }
}

bool enemy::check_hero_collision_x(game_object* hero)
{
    auto it = find_sprite(state);

    if (it != sprites.end())
    {
        float hero_current_pos_x = hero->get_global_pos_x();
        float hero_size          = hero->get_size();
        float hero_width =
            hero->get_animated_sprite()->get_sprite()->get_width();
        float width = it->game_object_anim_sprite->get_sprite()->get_width();

        bool collision_x = hero_current_pos_x + hero_width / 2 * hero_size >=
                               get_global_pos_x() - width / 2 * size &&
                           get_global_pos_x() + width / 2 * size >=
                               hero_current_pos_x - hero_width / 2 * hero_size;

        if (collision_x)
            return true;
    }

    return false;
}

bool enemy::check_hero_collision_y(game_object* hero)
{
    auto it = find_sprite(state);

    if (it != sprites.end())
    {
        float hero_current_pos_y = hero->get_global_pos_y();
        float hero_size          = hero->get_size();
        float hero_height =
            hero->get_animated_sprite()->get_sprite()->get_height();
        float height = it->game_object_anim_sprite->get_sprite()->get_height();

        bool collision_y = hero_current_pos_y + hero_height / 2 * hero_size >=
                               get_global_pos_y() - height / 2 * size &&
                           get_global_pos_y() + height / 2 * size >=
                               hero_current_pos_y - hero_height / 2 * hero_size;

        if (collision_y)
            return true;
    }

    return false;
}

enemy::~enemy() = default;