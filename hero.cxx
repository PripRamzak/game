#include "hero.hxx"

#include <algorithm>
#include <iostream>

#include "glm/vec2.hpp"

class warrior final : public hero
{
    std::vector<hero_sprite_state> sprites;
    glm::vec2                      position{ 0.f, 0.f };
    glm::vec2                      delta_position{ 0.f, 0.f };
    float                          size      = 0;
    int                            direction = 0;
    hero_state                     state;

public:
    warrior(int pos_x, int pos_y, float size_, hero_state state_)
        : size(size_)
        , state(state_)
    {
        position = { pos_x, pos_y };
    }
    void add_sprite(sprite* hero_sprite_, hero_state state_) final
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
            sprite_.hero_sprite = hero_sprite_;
            sprite_.state       = state_;

            vertex_2d v1 = { position.x - sprite_width / 2.f,
                             position.y - sprite_height / 2.f,
                             0.f,
                             1.f };
            vertex_2d v2 = { position.x + sprite_width / 2.f,
                             position.y - sprite_height / 2.f,
                             1.f,
                             1.f };
            vertex_2d v3 = { position.x + sprite_width / 2.f,
                             position.y + sprite_height / 2.f,
                             1.f,
                             0.f };
            vertex_2d v4 = { position.x - sprite_width / 2.f,
                             position.y + sprite_height / 2.f,
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
    void move(float delta_x, float delta_y) final
    {
        delta_position.x += delta_x;
        delta_position.y += delta_y;
    }
    void set_state(hero_state state_) final
    {
        if (state != state_)
        {
            const auto it = std::find_if(sprites.begin(),
                                         sprites.end(),
                                         [&](const hero_sprite_state sprite)
                                         { return sprite.state == state; });
            if (it != sprites.end())
                it->hero_sprite->reset();
            else
            {
                std::cout << "Such sprite doesn't exists" << std::endl;
                return;
            }
        }
        state = state_;
    }
    float get_current_pos_x() final { return position.x + delta_position.x; }
    float get_current_pos_y() final { return position.y + delta_position.y; }
    void  get_delta_pos(float& x, float& y) final
    {
        x = delta_position.x;
        y = delta_position.y;
    }
    float      get_size() final { return size; }
    void       set_direction(int direction_) final { direction = direction_; }
    hero_state get_state() final { return state; }
    int        get_direction() final { return direction; }
    sprite*    get_sprite() final
    {
        auto it = std::find_if(sprites.begin(),
                               sprites.end(),
                               [&](const hero_sprite_state sprite)
                               { return sprite.state == state; });

        if (it != sprites.end())
            return it->hero_sprite;
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

hero* create_hero(float pos_x, float pos_y, float size, hero_state state)
{
    return new warrior(pos_x, pos_y, size, state);
}