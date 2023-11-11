#include "include/hero.hxx"

#include <cmath>

static texture* warrior_idle_sprite_sheet   = nullptr;
static texture* warrior_run_sprite_sheet    = nullptr;
static texture* warrior_attack_sprite_sheet = nullptr;

static sprite* warrior_idle_sprite   = nullptr;
static sprite* warrior_run_sprite    = nullptr;
static sprite* warrior_attack_sprite = nullptr;

static animation* warrior_idle   = nullptr;
static animation* warrior_run    = nullptr;
static animation* warrior_attack = nullptr;

hero::hero(int               health,
           float             speed,
           float             global_pos_x,
           float             global_pos_y,
           float             size,
           game_object_state state)
    : game_object(health, speed, global_pos_x, global_pos_y, size, state)
{
    add_sprite(warrior_idle, game_object_state::idle);
    add_sprite(warrior_run, game_object_state::run);
    add_sprite(warrior_attack, game_object_state::attack);
}

void hero::initialize()
{
    using namespace std::chrono_literals;

    warrior_idle_sprite_sheet   = create_texture("img/warrior_idle.png");
    warrior_run_sprite_sheet    = create_texture("img/warrior_run.png");
    warrior_attack_sprite_sheet = create_texture("img/warrior_attack.png");

    warrior_idle_sprite   = new sprite(warrior_idle_sprite_sheet, 48.f, 48.f);
    warrior_run_sprite    = new sprite(warrior_run_sprite_sheet, 48.f, 48.f);
    warrior_attack_sprite = new sprite(warrior_attack_sprite_sheet, 86.f, 48.f);

    warrior_idle   = new animation(warrior_idle_sprite, 6, 24.f, 150ms);
    warrior_run    = new animation(warrior_run_sprite, 6, 24.f, 150ms);
    warrior_attack = new animation(warrior_attack_sprite, 4, 6.f, 250ms);
}

void hero::move(int dx, int dy, map* map, bool* skeleton_collision)
{
    float delta_x_ = static_cast<float>(dx) * speed;
    float delta_y_ = static_cast<float>(dy) * speed;

    if (dx != 0 && dy != 0)
    {
        delta_x_ /= sqrt(2.f);
        delta_y_ /= sqrt(2.f);
    }

    if (delta_y_ < 0.f && skeleton_collision[0] ||
        delta_y_ > 0.f && skeleton_collision[1])
    {
        set_state(game_object_state::idle);
        return;
    }

    if (delta_x_ < 0.f)
    {
        direction = 1;
        if (skeleton_collision[2])
        {
            set_state(game_object_state::idle);
            return;
        }
    }
    else if (delta_x_ > 0.f)
    {
        direction = 0;
        if (skeleton_collision[3])
        {
            set_state(game_object_state::idle);
            return;
        }
    }

    delta_x += delta_x_;
    delta_y += delta_y_;

    if (check_collision_map(map))
    {
        delta_x -= delta_x_;
        delta_y -= delta_y_;
        set_state(game_object_state::idle);
    }
    else
        set_state(game_object_state::run);
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

bool hero::check_collision_map(map* map)
{
    auto it = find_sprite(state);

    map_tile type[] = { map_tile::wall_top,
                        map_tile::wall_bottom,
                        map_tile::wall_left,
                        map_tile::wall_right };

    for (int i = 0; i < 4; i++)
    {
        const vertex_2d* map_tile_vertices =
            map->get_vertex_buffer(type[i])->data();
        const size_t map_tile_vertices_num =
            map->get_vertex_buffer(type[i])->size();
        float hero_sprite_height =
            it->game_object_anim_sprite->get_sprite()->get_height();
        float hero_sprite_width =
            it->game_object_anim_sprite->get_sprite()->get_width();

        for (size_t j = 0; j < map_tile_vertices_num / 4;
             j++, map_tile_vertices += 4)
        {
            bool collision_x =
                get_current_pos_x() + hero_sprite_width / 2.f * size >=
                    map_tile_vertices->x &&
                (map_tile_vertices + 2)->x >=
                    get_current_pos_x() - hero_sprite_width / 2.f * size;
            bool collision_y =
                get_current_pos_y() + hero_sprite_height / 2.f * size >=
                    map_tile_vertices->y &&
                (map_tile_vertices + 2)->y >=
                    get_current_pos_y() - hero_sprite_height / 2.f * size;

            if (collision_x && collision_y)
                return true;
        }
    }

    return false;
}

hero::~hero() = default;