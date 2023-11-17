#include "include/hero.hxx"

#include <algorithm>
#include <cmath>

static animation* warrior_idle_anim   = nullptr;
static animation* warrior_run_anim    = nullptr;
static animation* warrior_attack_anim = nullptr;
static animation* warrior_jump_anim   = nullptr;
static animation* warrior_fall_anim   = nullptr;

static bool hero_init = false;

hero::hero(int               health,
           float             speed,
           float             global_pos_x,
           float             global_pos_y,
           float             size,
           game_object_state state,
           float             jump_height_)
    : game_object(health, speed, global_pos_x, global_pos_y, size, state)
    , jump_height(jump_height_)
{
    add_sprite(warrior_idle_anim, game_object_state::idle);
    add_sprite(warrior_run_anim, game_object_state::run);
    add_sprite(warrior_attack_anim, game_object_state::attack);
    add_sprite(warrior_jump_anim, game_object_state::jump);
    add_sprite(warrior_fall_anim, game_object_state::fall);
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
        texture* warrior_jump_texture =
            create_texture("img/warrior_jump_n_fall.png");

        sprite* warrior_idle_sprite =
            new sprite(warrior_idle_sprite_sheet, 48.f, 48.f);
        sprite* warrior_run_sprite =
            new sprite(warrior_run_sprite_sheet, 48.f, 48.f);
        sprite* warrior_attack_sprite =
            new sprite(warrior_attack_sprite_sheet, 86.f, 48.f);
        sprite* warrior_jump_sprite =
            new sprite(warrior_jump_texture, 48.f, 48.f);

        warrior_idle_anim = new animation(warrior_idle_sprite, 6, 24.f, 150ms);
        warrior_run_anim  = new animation(warrior_run_sprite, 6, 24.f, 150ms);
        warrior_attack_anim =
            new animation(warrior_attack_sprite, 4, 6.f, 250ms);
        warrior_jump_anim = new animation(warrior_jump_sprite, 1, 0.f, 250ms);
        warrior_fall_anim = new animation(warrior_jump_sprite, 1, 0.f, 250ms);

        hero_init = true;
    }
    else
        std::cout << "Hero already inited!" << std::endl;
}

void hero::move(int dx, int dy, map* map)
{
    float delta_x = static_cast<float>(dx) * speed;
    float delta_y = static_cast<float>(dy) * speed;

    if (dx != 0 && dy != 0)
    {
        delta_x /= sqrt(2.f);
        delta_y /= sqrt(2.f);
    }

    global_pos_x += delta_x;
    global_pos_y += delta_y;

    if (delta_x < 0.f)
    {
        direction = 1;
        if (check_collision_map(map, map_tile::wall_left))
        {
            global_pos_x -= delta_x;

            if (state == game_object_state::jump ||
                state == game_object_state::fall)
                set_state(game_object_state::fall);
            else
                set_state(game_object_state::idle);
            return;
        }
    }
    else if (delta_x > 0.f)
    {
        direction = 0;
        if (check_collision_map(map, map_tile::wall_right))
        {
            global_pos_x -= delta_x;

            if (state == game_object_state::jump ||
                state == game_object_state::fall)
                set_state(game_object_state::fall);
            else
                set_state(game_object_state::idle);
            return;
        }
    }

    if (delta_y > 0.f)
    {
        if (check_collision_map(map, map_tile::wall_bottom))
        {
            global_pos_y -= delta_y;

            set_state(game_object_state::idle);
            return;
        }
        else
            set_state(game_object_state::fall);
    }

    if (delta_y < 0.f)
        if (check_collision_map(map, map_tile::wall_top))
        {
            global_pos_y -= delta_y;

            set_state(game_object_state::fall);
            return;
        }

    if (state != game_object_state::jump && state != game_object_state::fall)
        set_state(game_object_state::run);
}

void hero::jump(float jump_dt)
{
    set_state(game_object_state::jump);

    jump_height_dt -= jump_dt * speed;

    if (jump_height_dt >= jump_height)
    {
        set_state(game_object_state::fall);
        jump_height_dt = 0.f;
    }
}

void hero::attack(game_object* enemy, bool skeleton_collision)
{
    auto it = find_sprite(state);
    int  sprite_current_number =
        it->game_object_anim_sprite->get_current_number(direction);

    if (sprite_current_number == 0 && direction == 0 ||
        sprite_current_number == 3 && direction == 1)
        attacked = false;

    // Hit moment

    if ((sprite_current_number == 2 && direction == 0 ||
         sprite_current_number == 1 && direction == 1) &&
        !attacked)
    {
        attacked = true;
        if (skeleton_collision && direction != enemy->get_direction())
            enemy->hurt();
    }
}

bool hero::check_collision_map(map* map, map_tile type)
{
    const vertex_2d* map_tile_vertices = map->get_vertex_buffer(type)->data();
    const size_t map_tile_vertices_num = map->get_vertex_buffer(type)->size();

    vertex_2d* hero_vertices =
        get_animated_sprite()->get_sprite()->get_vertex_buffer()->data();

    for (size_t j = 0; j < map_tile_vertices_num / 4;
         j++, map_tile_vertices += 4)
    {
        bool collision_x = get_global_pos_x() + (hero_vertices + 2)->x * size >=
                               map_tile_vertices->x &&
                           (map_tile_vertices + 2)->x >=
                               get_global_pos_x() + hero_vertices->x * size;

        bool collision_y = get_global_pos_y() + (hero_vertices + 2)->y * size >=
                               map_tile_vertices->y &&
                           (map_tile_vertices + 2)->y >=
                               get_global_pos_y() + hero_vertices->y * size;

        if (collision_x && collision_y)
            return true;
    }

    return false;
}

hero::~hero() = default;