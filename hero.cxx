#include "hero.hxx"

#include <algorithm>
#include <iostream>

hero::hero(int pos_x, int pos_y, float size, game_object_state state)
    : game_object(pos_x, pos_y, size, state)
{
}

class warrior : public hero
{
public:
    warrior(int pos_x, int pos_y, float size, game_object_state state)
        : hero(pos_x, pos_y, size, state)
    {
    }
    void add_sprite(sprite* hero_sprite_, game_object_state state_) final
    {
        float sprite_width  = hero_sprite_->get_width();
        float sprite_height = hero_sprite_->get_height();

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
            hero_sprite_state sprite_;
            sprite_.game_object_sprite = hero_sprite_;
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
    void move(float delta_x_, float delta_y_) final
    {
        delta_x += delta_x_;
        delta_y += delta_y_;
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

hero::~hero() = default;

hero* create_hero(float pos_x, float pos_y, float size, game_object_state state)
{
    return new warrior(pos_x, pos_y, size, state);
}