#include "include/hero.hxx"

#include <algorithm>

static animation* warrior_idle_anim   = nullptr;
static animation* warrior_run_anim    = nullptr;
static animation* warrior_attack_anim = nullptr;
static animation* warrior_jump_anim   = nullptr;
static animation* warrior_fall_anim   = nullptr;

static bool hero_init = false;

hero::hero(int   health,
           float speed,
           float global_pos_x,
           float global_pos_y,
           float size,
           float jump_force,
           float jump_height)
    : game_object(health,
                  speed,
                  global_pos_x,
                  global_pos_y,
                  size,
                  game_object_state::idle)
    , jump_height(jump_height)
    , jump_force(jump_force)
{
    sprites.emplace(game_object_state::idle, warrior_idle_anim);
    sprites.emplace(game_object_state::run, warrior_run_anim);
    sprites.emplace(game_object_state::attack, warrior_attack_anim);
    sprites.emplace(game_object_state::jump, warrior_jump_anim);
    sprites.emplace(game_object_state::fall, warrior_fall_anim);
}

void hero::initialize()
{
    using namespace std::chrono_literals;

    if (!hero_init)
    {
        texture* warrior_idle_sprite_sheet =
            create_texture("img/warrior_idle.png");
        texture* warrior_run_sprite_sheet =
            create_texture("img/warrior_run.png");
        texture* warrior_attack_sprite_sheet =
            create_texture("img/warrior_attack.png");
        texture* warrior_jump_texture = create_texture("img/warrior_jump.png");
        texture* warrior_fall_texture = create_texture("img/warrior_fall.png");

        sprite* warrior_idle_sprite =
            new sprite(warrior_idle_sprite_sheet, 48.f, 48.f);
        sprite* warrior_run_sprite =
            new sprite(warrior_run_sprite_sheet, 48.f, 48.f);
        sprite* warrior_attack_sprite =
            new sprite(warrior_attack_sprite_sheet, 86.f, 48.f);
        sprite* warrior_jump_sprite =
            new sprite(warrior_jump_texture, 48.f, 48.f);
        sprite* warrior_fall_sprite =
            new sprite(warrior_fall_texture, 48.f, 48.f);

        warrior_idle_anim   = new animation(warrior_idle_sprite, 6, 100ms);
        warrior_run_anim    = new animation(warrior_run_sprite, 6, 100ms);
        warrior_attack_anim = new animation(warrior_attack_sprite, 4, 125ms);
        warrior_jump_anim   = new animation(warrior_jump_sprite, 1, 250ms);
        warrior_fall_anim   = new animation(warrior_fall_sprite, 1, 250ms);

        hero_init = true;
    }
    else
        std::cout << "Hero already inited!" << std::endl;
}

void hero::move(float dx, float dy, map* map)
{
    float delta_x = dx * speed;
    float delta_y = dy * speed;

    global_pos_x += delta_x;

    if (delta_x < 0.f)
    {
        direction = 1;
        if (collision::map_with_game_object(
                map, this, collision::direction::left))
        {
            if (state != game_object_state::jump &&
                state != game_object_state::fall)
            {
                set_state(game_object_state::idle);
                return;
            }
        }
    }
    else if (delta_x > 0.f)
    {
        direction = 0;
        if (collision::map_with_game_object(
                map, this, collision::direction::right))
        {
            if (state != game_object_state::jump &&
                state != game_object_state::fall)
            {
                set_state(game_object_state::idle);
                return;
            }
        }
    }

    global_pos_y += delta_y;

    if (state != game_object_state::jump && state != game_object_state::fall &&
        !collision::map_with_game_object(map, this, collision::direction::down))
    {
        set_state(game_object_state::fall);
        delta_y += speed;
    }

    if (delta_y > 0.f)
    {
        if (collision::map_with_game_object(
                map, this, collision::direction::down))
        {
            set_state(game_object_state::idle);
            return;
        }
        else
            set_state(game_object_state::fall);
    }
    else if (delta_y < 0.f)
        if (collision::map_with_game_object(
                map, this, collision::direction::up))
        {
            set_state(game_object_state::fall);
            jump_height_dt = 0.f;
        }

    if (state != game_object_state::jump && state != game_object_state::fall)
        set_state(game_object_state::run);
}

void hero::jump()
{
    set_state(game_object_state::jump);

    std::cout << jump_height_dt << std::endl;

    jump_height_dt += jump_force;

    if (jump_height_dt >= jump_height)
    {
        set_state(game_object_state::fall);
        jump_height_dt = 0.f;
    }
}

void hero::attack(game_object* enemy)
{
    animation* attack_anim         = sprites[state];
    int        anim_current_number = attack_anim->get_current_frame_number();
    int        anim_quantity       = attack_anim->get_frames_quantity();

    if (!attacked && anim_current_number == anim_quantity - 1)
    {
        if (collision::game_object_with_game_object(this, enemy))
            enemy->hurt();
        attacked = true;
    }

    if (attacked && anim_current_number == 0)
        attacked = false;
}

hero::~hero() = default;