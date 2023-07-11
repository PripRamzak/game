#include "enemy.hxx"

#include <algorithm>
#include <iostream>

static texture* skeleton_idle_sprite_sheet   = nullptr;
static texture* skeleton_run_sprite_sheet    = nullptr;
static texture* skeleton_attack_sprite_sheet = nullptr;
static sprite*  skeleton_idle                = nullptr;
static sprite*  skeleton_run                 = nullptr;
static sprite*  skeleton_attack              = nullptr;

enemy::enemy(int               health,
             float             speed,
             float             local_pos_x,
             float             local_pos_y,
             float             global_pos_x,
             float             global_pos_y,
             float             size,
             game_object_state state)
    : game_object(health,
                  speed,
                  local_pos_x,
                  local_pos_y,
                  global_pos_x,
                  global_pos_y,
                  size,
                  state)
{
}

void enemy::initialize()
{
    skeleton_idle_sprite_sheet   = create_texture("img/skeleton_idle.png");
    skeleton_run_sprite_sheet    = create_texture("img/skeleton_run.png");
    skeleton_attack_sprite_sheet = create_texture("img/skeleton_attack.png");
    skeleton_idle =
        create_sprite(skeleton_idle_sprite_sheet, 64.f, 64.f, 7, 32.f, 0.15f);
    skeleton_run =
        create_sprite(skeleton_run_sprite_sheet, 74.f, 64.f, 8, 32.f, 0.15f);
    skeleton_attack =
        create_sprite(skeleton_attack_sprite_sheet, 96.f, 64.f, 4, 16.f, 0.25f);
}

class skeleton final : public enemy
{
    bool attacked = false;
    bool spawned  = false;

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
    skeleton(int               health,
             float             speed,
             float             local_pos_x,
             float             local_pos_y,
             float             global_pos_x,
             float             global_pos_y,
             float             size,
             game_object_state state)
        : enemy(health,
                speed,
                local_pos_x,
                local_pos_y,
                global_pos_x,
                global_pos_y,
                size,
                state)
    {
        add_sprite(skeleton_idle, game_object_state::idle);
        add_sprite(skeleton_run, game_object_state::run);
        add_sprite(skeleton_attack, game_object_state::attack);
    }
    void spawn() final
    {
        spawned = true;
        alive   = true;
    }
    bool is_alive() { return alive; }
    bool is_spawned() { return spawned; }
    void hurt() final
    {
        health--;
        if (health == 0)
            alive = false;
    }
    void move(game_object* hero) final
    {
        bool hero_collision_x = check_hero_collision_x(hero);

        if (!hero_collision_x)
        {
            if (hero->get_current_pos_x() < get_current_pos_x())
            {
                delta_x -= speed;
                direction = 1;
            }
            else
            {
                delta_x += speed;
                direction = 0;
            }

            set_state(game_object_state::run);
        }
        else if (!check_hero_collision_y(hero))
        {
            if (hero->get_current_pos_y() > get_current_pos_y())
                delta_y += speed;
            else
                delta_y -= speed;

            set_state(game_object_state::run);
        }
        else if (hero->get_current_pos_y() - 2.5f > get_current_pos_y() ||
                 hero->get_current_pos_y() + 2.5f < get_current_pos_y())
        {
            if (hero->get_current_pos_x() <= get_current_pos_x())
            {
                delta_x += speed;
                direction = 0;
            }
            else
            {
                delta_x -= speed;
                direction = 1;
            }

            if (!check_hero_collision_x(hero))
            {
                if (hero->get_current_pos_x() <= get_current_pos_x())
                {
                    delta_x -= speed;
                    direction = 1;
                }
                else
                {
                    delta_x += speed;
                    direction = 0;
                }

                if (hero->get_current_pos_y() - 2.5f > get_current_pos_y())
                    delta_y += speed;
                else if (hero->get_current_pos_y() + 2.5f < get_current_pos_y())
                    delta_y -= speed;
            }

            set_state(game_object_state::run);
        }
        else if (state == game_object_state::idle)
        {
            auto it = find_sprite(state);
            int  sprite_current_number =
                it->game_object_sprite->get_current_number(direction);
            int sprite_quantity = it->game_object_sprite->get_quantity();

            if (sprite_current_number == sprite_quantity - 1 &&
                    direction == 0 ||
                sprite_current_number == 0 && direction == 1)
                set_state(game_object_state::attack);
        }
        else
            set_state(game_object_state::idle);
    }
    void attack(game_object* hero) final
    {
        auto it = find_sprite(state);
        int  sprite_current_number =
            it->game_object_sprite->get_current_number(direction);
        int sprite_quantity = it->game_object_sprite->get_quantity();

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
    bool check_hero_collision_x(game_object* hero) final
    {
        auto it = find_sprite(state);

        if (it != sprites.end())
        {
            float hero_current_pos_x = hero->get_current_pos_x();
            float            hero_size       = hero->get_size();
            float            hero_width      = hero->get_sprite()->get_width();
            float            width = it->game_object_sprite->get_width();

            bool collision_x =
                hero_current_pos_x + hero_width / 2 * hero_size >=
                    get_current_pos_x() - width / 2 * size &&
                get_current_pos_x() + width / 2 * size >=
                    hero_current_pos_x - hero_width / 2 * hero_size;

            if (collision_x)
                return true;
        }

        return false;
    }
    bool check_hero_collision_y(game_object* hero) final
    {
        auto it = find_sprite(state);

        if (it != sprites.end())
        {
            float hero_current_pos_y = hero->get_current_pos_y();
            float            hero_size       = hero->get_size();
            float            hero_height     = hero->get_sprite()->get_height();
            float            height = it->game_object_sprite->get_height();

            bool collision_y =
                hero_current_pos_y + hero_height / 2 * hero_size >=
                    get_current_pos_y() - height / 2 * size &&
                get_current_pos_y() + height / 2 * size >=
                    hero_current_pos_y - hero_height / 2 * hero_size;

            if (collision_y)
                return true;
        }

        return false;
    }
    float get_move_x() { return global_pos_x - local_pos_x + delta_x; }
    float get_move_y() { return global_pos_y - local_pos_y + delta_y; }
    void  set_state(game_object_state state_) final
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
    int   get_health() final { return health; }
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

enemy::~enemy() = default;

enemy* create_enemy(int               health,
                    float             speed,
                    float             local_pos_x,
                    float             local_pos_y,
                    float             global_pos_x,
                    float             global_pos_y,
                    float             size,
                    game_object_state state)
{
    return new skeleton(health,
                        speed,
                        local_pos_x,
                        local_pos_y,
                        global_pos_x,
                        global_pos_y,
                        size,
                        state);
}