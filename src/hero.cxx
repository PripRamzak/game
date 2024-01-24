#include "include/hero.hxx"

#include <algorithm>

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
           float             jump_force,
           float             jump_height)
    : game_object(health, speed, global_pos_x, global_pos_y, size, state)
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

        warrior_idle_anim = new animation(warrior_idle_sprite, 6, 24.f, 150ms);
        warrior_run_anim  = new animation(warrior_run_sprite, 6, 24.f, 150ms);
        warrior_attack_anim =
            new animation(warrior_attack_sprite, 4, 6.f, 250ms);
        warrior_jump_anim = new animation(warrior_jump_sprite, 1, 0.f, 250ms);
        warrior_fall_anim = new animation(warrior_fall_sprite, 1, 0.f, 250ms);

        hero_init = true;
    }
    else
        std::cout << "Hero already inited!" << std::endl;
}

void hero::move(float dx, float dy, map* map)
{
    float delta_x = dx * speed;
    float delta_y = dy * speed;

    global_pos_y += delta_y;

    if (delta_y > 0.f)
    {
        if (check_collision_map_y_axis(map, map_tile_type::brick_bottom))
        {
            set_state(game_object_state::idle);
            return;
        }
        else
            set_state(game_object_state::fall);
    }
    else if (delta_y < 0.f)
        if (check_collision_map_y_axis(map, map_tile_type::brick_top))
        {
            set_state(game_object_state::fall);
            jump_height_dt = 0.f;
        }

    global_pos_x += delta_x;

    if (delta_x < 0.f)
    {
        direction = 1;
        if (check_collision_map_x_axis(map, map_tile_type::brick_left))
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
        if (check_collision_map_x_axis(map, map_tile_type::brick_right))
        {
            if (state != game_object_state::jump &&
                state != game_object_state::fall)
            {
                set_state(game_object_state::idle);
                return;
            }
        }
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

void hero::attack(game_object* enemy, bool skeleton_collision)
{
    int sprite_current_number = sprites[state]->get_current_number(direction);

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

bool hero::check_collision_map(map* map, map_tile_type type)
{
    const vertex_2d* map_tile_vertices = map->get_vertex_buffer(type)->data();
    const size_t map_tile_vertices_num = map->get_vertex_buffer(type)->size();

    vertex_2d* hero_vertices =
        get_animated_sprite()->get_sprite()->get_vertex_buffer()->data();

    for (size_t j = 0; j < map_tile_vertices_num / 4;
         j++, map_tile_vertices += 4)
    {
        bool collision_x_right = global_pos_x + (hero_vertices + 2)->x * size >=
                                 map_tile_vertices->x;
        bool collision_x_left = (map_tile_vertices + 2)->x >=
                                global_pos_x + hero_vertices->x * size;
        bool collision_y_top = (map_tile_vertices + 2)->y >=
                               global_pos_y + hero_vertices->y * size;
        bool collision_y_bot = global_pos_y + (hero_vertices + 2)->y * size >=
                               map_tile_vertices->y;

        bool collision_x = collision_x_left && collision_x_right;
        bool collision_y = collision_y_top && collision_y_bot;

        if (collision_x && collision_y)
        {
            switch (type)
            {
                case map_tile_type::brick_left:
                    global_pos_x -= global_pos_x + hero_vertices->x * size -
                                    (map_tile_vertices + 2)->x;
                    break;
                case map_tile_type::brick_right:
                    global_pos_x -= global_pos_x +
                                    (hero_vertices + 2)->x * size -
                                    map_tile_vertices->x;
                    break;
                case map_tile_type::brick_bottom:
                    global_pos_y -= global_pos_y +
                                    (hero_vertices + 2)->y * size -
                                    map_tile_vertices->y;
                    break;
                case map_tile_type::brick_top:
                    global_pos_y -= global_pos_y + hero_vertices->y * size -
                                    (map_tile_vertices + 2)->y;
                    break;
            }
            return true;
        }
    }

    return false;
}

bool hero::check_collision_map_x_axis(map* map, map_tile_type type)
{
    const vertex_2d* map_tile_vertices = map->get_vertex_buffer(type)->data();
    const size_t map_tile_vertices_num = map->get_vertex_buffer(type)->size();

    vertex_2d* hero_vertices =
        get_animated_sprite()->get_sprite()->get_vertex_buffer()->data();

    for (size_t j = 0; j < map_tile_vertices_num / 4;
         j++, map_tile_vertices += 4)
    {
        bool collision_x_right = global_pos_x + (hero_vertices + 2)->x * size >=
                                 map_tile_vertices->x;
        bool collision_x_left = (map_tile_vertices + 2)->x >=
                                global_pos_x + hero_vertices->x * size;
        bool collision_y_top =
            (map_tile_vertices + 2)->y > global_pos_y + hero_vertices->y * size;
        bool collision_y_bot =
            global_pos_y + (hero_vertices + 2)->y * size > map_tile_vertices->y;

        bool collision_x = collision_x_left && collision_x_right;
        bool collision_y = collision_y_top && collision_y_bot;

        if (collision_x && collision_y)
        {
            switch (type)
            {
                case map_tile_type::brick_left:
                    global_pos_x -= global_pos_x + hero_vertices->x * size -
                                    (map_tile_vertices + 2)->x;
                    break;
                case map_tile_type::brick_right:
                    global_pos_x -= global_pos_x +
                                    (hero_vertices + 2)->x * size -
                                    map_tile_vertices->x;
                    break;
            }
            return true;
        }
    }

    return false;
}

bool hero::check_collision_map_y_axis(map* map, map_tile_type type)
{
    const vertex_2d* map_tile_vertices = map->get_vertex_buffer(type)->data();
    const size_t map_tile_vertices_num = map->get_vertex_buffer(type)->size();

    vertex_2d* hero_vertices =
        get_animated_sprite()->get_sprite()->get_vertex_buffer()->data();

    for (size_t j = 0; j < map_tile_vertices_num / 4;
         j++, map_tile_vertices += 4)
    {
        bool collision_x_right =
            global_pos_x + (hero_vertices + 2)->x * size > map_tile_vertices->x;
        bool collision_x_left =
            (map_tile_vertices + 2)->x > global_pos_x + hero_vertices->x * size;
        bool collision_y_top = (map_tile_vertices + 2)->y >=
                               global_pos_y + hero_vertices->y * size;
        bool collision_y_bot = global_pos_y + (hero_vertices + 2)->y * size >=
                               map_tile_vertices->y;

        bool collision_x = collision_x_left && collision_x_right;
        bool collision_y = collision_y_top && collision_y_bot;

        if (collision_x && collision_y)
        {
            switch (type)
            {
                case map_tile_type::brick_bottom:
                    global_pos_y -= global_pos_y +
                                    (hero_vertices + 2)->y * size -
                                    map_tile_vertices->y;
                    break;
                case map_tile_type::brick_top:
                    global_pos_y -= global_pos_y + hero_vertices->y * size -
                                    (map_tile_vertices + 2)->y;
                    break;
            }
            return true;
        }
    }

    return false;
}

hero::~hero() = default;