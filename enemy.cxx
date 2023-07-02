#include "enemy.hxx"

#include <algorithm>
#include <iostream>

enemy::enemy(float             local_pos_x,
             float             local_pos_y,
             float             global_pos_x,
             float             global_pos_y,
             float             size,
             game_object_state state)
    : game_object(
          local_pos_x, local_pos_y, global_pos_x, global_pos_y, size, state)
{
}

class skeleton : public enemy
{
    auto find_sprite(game_object_state state_)
    {
        auto it = std::find_if(sprites.begin(),
                               sprites.end(),
                               [&](const hero_sprite_state sprite)
                               { return sprite.state == state_; });

        return it;
    }

public:
    skeleton(float             local_pos_x,
             float             local_pos_y,
             float             global_pos_x,
             float             global_pos_y,
             float             size,
             game_object_state state)
        : enemy(
              local_pos_x, local_pos_y, global_pos_x, global_pos_y, size, state)
    {
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
    float get_move_x() { return global_pos_x - local_pos_x + delta_x; }
    float get_move_y() { return global_pos_y - local_pos_y + delta_y; }
    void  move(hero* hero) final
    {
        if (!check_hero_collision_y(hero))
        {
            if (hero->get_current_pos_y() > global_pos_y + delta_y)
                delta_y += 5.f;
            else if (hero->get_current_pos_y() < global_pos_y + delta_y)
                delta_y -= 5.f;
        }
        else if (hero->get_current_pos_y() - 2.5f > global_pos_y + delta_y ||
                 hero->get_current_pos_y() + 2.5f < global_pos_y + delta_y)
        {
            if (check_hero_collision_x(hero))
            {
                if (hero->get_current_pos_x() <= global_pos_x + delta_x)
                {
                    delta_x += 5.f;
                    direction = 0;
                }
                else if (hero->get_current_pos_x() > global_pos_x + delta_x)
                {
                    delta_x -= 5.f;
                    direction = 1;
                }
            }
            else
            {
                if (hero->get_current_pos_y() - 2.5f > global_pos_y + delta_y)
                    delta_y += 5.f;
                else if (hero->get_current_pos_y() + 2.5f <
                         global_pos_y + delta_y)
                    delta_y -= 5.f;
            }
        }
        else if (!check_hero_collision_x(hero))
        {
            if (hero->get_current_pos_x() < global_pos_x + delta_x)
            {
                delta_x -= 5.f;
                direction = 1;
            }
            else if (hero->get_current_pos_x() > global_pos_x + delta_x)
            {
                delta_x += 5.f;
                direction = 0;
            }
        }
    }
    bool check_hero_collision_x(hero* hero) final
    {
        auto it = find_sprite(state);

        if (it != sprites.end())
        {
            const vertex_2d* sprite_vertices = hero->get_vertices();
            float            hero_size       = hero->get_size();
            float            hero_width      = hero->get_sprite()->get_width();
            vertex_2d*       vertices_       = it->vertices;
            float            width = it->game_object_sprite->get_width();

            float hero_delta_x = hero->get_delta_x();
            float delta_x_ =
                global_pos_x - local_pos_x + delta_x - hero_delta_x;

            bool collision_x =
                sprite_vertices[2].x + hero_width / 2 * (hero_size - 1) >=
                    vertices_[0].x - width / 2 * (size - 1) + delta_x_ &&
                vertices_[2].x + width / 2 * (size - 1) + delta_x_ >=
                    sprite_vertices[0].x - hero_width / 2 * (hero_size - 1);

            if (collision_x)
                return true;
        }

        return false;
    }
    bool check_hero_collision_y(hero* hero) final
    {
        auto it = find_sprite(state);

        if (it != sprites.end())
        {
            const vertex_2d* sprite_vertices = hero->get_vertices();
            float            hero_size       = hero->get_size();
            float            hero_height     = hero->get_sprite()->get_height();
            vertex_2d*       vertices_       = it->vertices;
            float            height = it->game_object_sprite->get_height();
            float            hero_delta_y = hero->get_delta_y();

            float delta_y_ =
                global_pos_y - local_pos_y + delta_y - hero_delta_y;

            bool collision_y =
                sprite_vertices[2].y + hero_height / 2 * (hero_size - 1) >=
                    vertices_[0].y - height / 2 * (size - 1) + delta_y_ &&
                vertices_[2].y + height / 2 * (size - 1) + delta_y_ >=
                    sprite_vertices[0].y - hero_height / 2 * (hero_size - 1);

            if (collision_y)
                return true;
        }

        return false;
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

enemy::~enemy() = default;

enemy* create_enemy(float             local_pos_x,
                    float             local_pos_y,
                    float             global_pos_x,
                    float             global_pos_y,
                    float             size,
                    game_object_state state)
{
    return new skeleton(
        local_pos_x, local_pos_y, global_pos_x, global_pos_y, size, state);
}