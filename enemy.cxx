#include "enemy.hxx"

#include <algorithm>
#include <iostream>

enemy::enemy(int pos_x, int pos_y, float size, game_object_state state)
    : game_object(pos_x, pos_y, size, state)
{
}

class skeleton : public enemy
{
public:
    skeleton(int pos_x, int pos_y, float size, game_object_state state)
        : enemy(pos_x, pos_y, size, state)
    {
    }
    void add_sprite(sprite* game_object_sprite_, game_object_state state_) final
    {
        float sprite_width  = game_object_sprite_->get_width();
        float sprite_height = game_object_sprite_->get_height();

        auto it = std::find_if(sprites.begin(),
                               sprites.end(),
                               [&](const hero_sprite_state sprite)
                               { return sprite.state == state_; });

        if (it != sprites.end())
        {
            std::cout << "This tile already exists" << std::endl;
            return;
        }
        else
        {
            vertex_2d         vert_pox = { position_x, position_y, 0.0, 0.0 };
            hero_sprite_state sprite_;
            sprite_.game_object_sprite = game_object_sprite_;
            sprite_.state              = state_;

            vertex_2d v1 = { position_x - sprite_width / 2.f,
                             position_y - sprite_height / 2.f,
                             0.f,
                             1.f };
            vertex_2d v2 = { position_x + sprite_width / 2.f,
                             position_y - sprite_height / 2.f,
                             1.f,
                             1.f };
            vertex_2d v3 = { position_x + sprite_width / 2.f,
                             position_y + sprite_height / 2.f,
                             1.f,
                             0.f };
            vertex_2d v4 = { position_x - sprite_width / 2.f,
                             position_y + sprite_height / 2.f,
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
    void move(hero* hero, float window_width, float window_height) final
    {
        if (!check_hero_x_collision(hero, window_width, window_height))
        {
            if (hero->get_current_pos_x() < position_x + delta_x)
            {
                delta_x -= 10.f;
                direction = 1;
            }
            else if (hero->get_current_pos_x() > position_x + delta_x)
            {
                delta_x   = 10.f;
                direction = 0;
            }
        }

        /*if (hero_pos_y < position_y + delta_y)
        {
            delta_y -= 10.f;
        }*/
    }
    bool check_hero_x_collision(hero* hero,
                                float window_width,
                                float window_height) final
    {
        auto it = std::find_if(sprites.begin(),
                               sprites.end(),
                               [&](const hero_sprite_state sprite)
                               { return sprite.state == state; });

        if (it != sprites.end())
        {
            const vertex_2d* sprite_vertices = hero->get_vertices();
            float            hero_size       = hero->get_size();
            vertex_2d*       vertices_       = it->vertices;

            float hero_delta_x = 0;
            float hero_delta_y = 0;
            hero->get_delta_pos(hero_delta_x, hero_delta_y);

            vertex_2d hero_vertex_delta  = { hero_delta_x + window_width / 2,
                                             hero_delta_y + window_height / 2,
                                             0.f,
                                             0.f };
            vertex_2d enemy_vertex_delta = { delta_x + window_width / 2,
                                             delta_y + window_height / 2,
                                             0.f,
                                             0.f };

            float delta_x_ = enemy_vertex_delta.x - hero_vertex_delta.x;

            bool collision_x = sprite_vertices[2].x * hero_size >=
                                   vertices_[0].x * size + delta_x_ &&
                               vertices_[2].x * size + delta_x_ >=
                                   sprite_vertices[0].x * hero_size;

            if (collision_x)
            {
                std::cout << "collsion nice" << std::endl;
                return true;
            }
        }

        return false;
    }
    void set_state(game_object_state state_) final
    {
        if (state != state_)
        {
            const auto it = std::find_if(sprites.begin(),
                                         sprites.end(),
                                         [&](const hero_sprite_state sprite)
                                         { return sprite.state == state; });
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
    float get_current_pos_x() final { return position_x + delta_x; }
    float get_current_pos_y() final { return position_y + delta_y; }
    void  get_delta_pos(float& x, float& y) final
    {
        x = delta_x;
        y = delta_y;
    }
    float get_size() final { return size; }
    void  set_direction(int direction_) final { direction = direction_; }
    game_object_state get_state() final { return state; }
    int               get_direction() final { return direction; }
    sprite*           get_sprite() final
    {
        auto it = std::find_if(sprites.begin(),
                               sprites.end(),
                               [&](const hero_sprite_state sprite)
                               { return sprite.state == state; });

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
        auto it = std::find_if(sprites.begin(),
                               sprites.end(),
                               [&](const hero_sprite_state sprite)
                               { return sprite.state == state; });

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
        auto it = std::find_if(sprites.begin(),
                               sprites.end(),
                               [&](const hero_sprite_state sprite)
                               { return sprite.state == state; });

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

enemy* create_enemy(float             pos_x,
                    float             pos_y,
                    float             size,
                    game_object_state state)
{
    return new skeleton(pos_x, pos_y, size, state);
}