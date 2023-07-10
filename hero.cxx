#include "hero.hxx"

#include <algorithm>
#include <iostream>

static texture* warrior_idle_sprite_sheet   = nullptr;
static texture* warrior_run_sprite_sheet    = nullptr;
static texture* warrior_attack_sprite_sheet = nullptr;
static sprite*  warrior_idle                = nullptr;
static sprite*  warrior_run                 = nullptr;
static sprite*  warrior_attack              = nullptr;

hero::hero(int               health,
           float             local_pos_x,
           float             local_pos_y,
           float             global_pos_x,
           float             global_pos_y,
           float             size,
           game_object_state state)
    : game_object(health,
                  local_pos_x,
                  local_pos_y,
                  global_pos_x,
                  global_pos_y,
                  size,
                  state)
{
}

void hero::initialize()
{
    warrior_idle_sprite_sheet   = create_texture("img/warrior_idle.png");
    warrior_run_sprite_sheet    = create_texture("img/warrior_run.png");
    warrior_attack_sprite_sheet = create_texture("img/warrior_attack.png");
    warrior_idle =
        create_sprite(warrior_idle_sprite_sheet, 48.f, 48.f, 6, 24.f, 0.15f);
    warrior_run =
        create_sprite(warrior_run_sprite_sheet, 48.f, 48.f, 6, 24.f, 0.15f);
    warrior_attack =
        create_sprite(warrior_attack_sprite_sheet, 86.f, 48.f, 4, 6.f, 0.25f);
}

class warrior : public hero
{
    bool attacked = false;

    auto find_sprite(game_object_state state_)
    {
        auto it = std::find_if(sprites.begin(),
                               sprites.end(),
                               [&](const hero_sprite_state sprite)
                               { return sprite.state == state_; });

        return it;
    }
    void add_sprite(sprite* game_object_sprite_, game_object_state state_) final
    {
        auto it = find_sprite(state_);

        if (it != sprites.end())
        {
            std::cout << "This tile already exists" << std::endl;
            return;
        }
        else
        {
            hero_sprite_state sprite_;
            sprite_.game_object_sprite = game_object_sprite_;
            sprite_.state              = state_;

            float sprite_width  = game_object_sprite_->get_width();
            float sprite_height = game_object_sprite_->get_height();

            vertex_2d v1 = { local_pos_x - sprite_width / 2.f,
                             local_pos_y - sprite_height / 2.f,
                             0.f,
                             1.f };
            vertex_2d v2 = { local_pos_x + sprite_width / 2.f,
                             local_pos_y - sprite_height / 2.f,
                             1.f,
                             1.f };
            vertex_2d v3 = { local_pos_x + sprite_width / 2.f,
                             local_pos_y + sprite_height / 2.f,
                             1.f,
                             0.f };
            vertex_2d v4 = { local_pos_x - sprite_width / 2.f,
                             local_pos_y + sprite_height / 2.f,
                             0.f,
                             0.f };

            sprite_.vertices[0] = v1;
            sprite_.vertices[1] = v2;
            sprite_.vertices[2] = v3;
            sprite_.vertices[3] = v4;

            sprite_.sprite_vertex_buffer = create_vertex_buffer();
            sprite_.sprite_vertex_buffer->buffer_data(sprite_.vertices,
                                                      static_cast<size_t>(4));

            sprites.push_back(sprite_);
        }
    }

public:
    warrior(int               health,
            float             local_pos_x,
            float             local_pos_y,
            float             global_pos_x,
            float             global_pos_y,
            float             size,
            game_object_state state)
        : hero(health,
               local_pos_x,
               local_pos_y,
               global_pos_x,
               global_pos_y,
               size,
               state)
    {
        alive = true;
        add_sprite(warrior_idle, game_object_state::idle);
        add_sprite(warrior_run, game_object_state::run);
        add_sprite(warrior_attack, game_object_state::attack);
    }
    void move(float delta_x_, float delta_y_, map* map, map_tile type) final
    {
        state = game_object_state::run;
        if (delta_x_ > 0.f)
            direction = 0;
        else if (delta_x_ < 0.f)
            direction = 1;

        delta_x += delta_x_;
        delta_y += delta_y_;

        if (check_collision_map(map, type))
        {
            delta_x -= delta_x_;
            delta_y -= delta_y_;
        }
    }
    void attack(game_object* enemy, bool collision_x, bool collision_y) final
    {
        auto it = find_sprite(state);
        int  sprite_current_number =
            it->game_object_sprite->get_current_number(direction);

        if (sprite_current_number == 0 && direction == 0 ||
            sprite_current_number == 3 && direction == 1)
            attacked = false;

        // Hit moment

        if ((sprite_current_number == 2 && direction == 0 ||
             sprite_current_number == 1 && direction == 1) &&
            !attacked)
        {
            attacked = true;
            if (collision_x && collision_y &&
                direction != enemy->get_direction())
                enemy->hurt();
        }
    }
    bool check_collision_map(map* map, map_tile type) final
    {
        auto it = find_sprite(state);

        const vertex_2d* map_tile_vertices     = map->get_vertices(type);
        const size_t     map_tile_vertices_num = map->get_vertices_num(type);
        const vertex_2d* hero_vertices         = it->vertices;
        float hero_sprite_height = it->game_object_sprite->get_height();
        float hero_sprite_width  = it->game_object_sprite->get_width();

        for (size_t i = 0; i < map_tile_vertices_num / 4;
             i++, map_tile_vertices += 4)
        {
            bool collision_x =
                hero_vertices[2].x + hero_sprite_width / 2.f * (size - 1) >=
                    map_tile_vertices->x - delta_x &&
                (map_tile_vertices + 2)->x - delta_x >=
                    hero_vertices[0].x - hero_sprite_width / 2.f * (size - 1);
            bool collision_y =
                hero_vertices[2].y + hero_sprite_height / 2.f * (size - 1) >=
                    map_tile_vertices->y - delta_y &&
                (map_tile_vertices + 2)->y - delta_y >=
                    hero_vertices[0].y - hero_sprite_height / 2.f * (size - 1);

            if (collision_x && collision_y)
                return true;
        }

        return false;
    }
    bool is_alive() { return alive; }
    void hurt() final
    {
        health--;
        if (health == 0)
            alive = false;
    }
    void set_state(game_object_state state_) final
    {
        if (state != state_)
        {
            const auto it = find_sprite(state);
            if (it != sprites.end())
                it->game_object_sprite->reset();
            else
            {
                std::cout << "Such sprite doesn't exists" << std::endl;
                return;
            }
        }
        state = state_;
    }
    float get_current_pos_x() final { return global_pos_x + delta_x; }
    float get_current_pos_y() final { return global_pos_y + delta_y; }
    float get_delta_x() final { return delta_x; }
    float get_delta_y() final { return delta_y; }
    float get_size() final { return size; }
    void  set_direction(int direction_) final { direction = direction_; }
    game_object_state get_state() final { return state; }
    int               get_direction() final { return direction; }
    sprite*           get_sprite() final
    {
        auto it = find_sprite(state);

        if (it != sprites.end())
            return it->game_object_sprite;
        else
        {
            std::cout << "Such sprite doesn't exists" << std::endl;
            return nullptr;
        }
    }
    vertex_2d* get_vertices() final
    {
        auto it = find_sprite(state);

        if (it != sprites.end())
            return it->vertices;
        else
        {
            std::cout << "Such sprite doesn't exists" << std::endl;
            return nullptr;
        }
    }
    vertex_buffer* get_vertex_buffer() final
    {
        auto it = find_sprite(state);

        if (it != sprites.end())
            return it->sprite_vertex_buffer;
        else
        {
            std::cout << "Such sprite doesn't exists" << std::endl;
            return nullptr;
        }
    }
};

hero::~hero() = default;

hero* create_hero(int               health,
                  float             local_pos_x,
                  float             local_pos_y,
                  float             global_pos_x,
                  float             global_pos_y,
                  float             size,
                  game_object_state state)
{
    return new warrior(health,
                       local_pos_x,
                       local_pos_y,
                       global_pos_x,
                       global_pos_y,
                       size,
                       state);
}