#include "sprite.hxx"

#include <iostream>
#include <vector>

#include <glm/matrix.hpp>

class game_sprite final : public sprite
{
    texture* textures;
    int      width          = 0;
    int      height         = 0;
    int      quantity       = 0;
    int      current_number = 0;
    float    start_position = 0.f;

public:
    game_sprite(texture* textures_,
                float    width_,
                float    height_,
                int      quantity_,
                float    start_position_)
        : textures(textures_)
        , width(width_)
        , height(height_)
        , quantity(quantity_)
    {
        start_position =
            start_position_ / static_cast<float>(textures->get_width());
    }
    void next_sprite() final
    {
        current_number++;
        if (current_number == quantity)
            current_number = 0;
    }
    float    get_width() final { return width; }
    float    get_height() final { return height; }
    int      get_quantity() final { return quantity; }
    int      get_current_number() final { return current_number; }
    float    get_start_position() final { return start_position; }
    texture* get_texture() final { return textures; }
};

sprite::~sprite() = default;

sprite* create_sprite(texture* textures,
                      float    width,
                      float    height,
                      int      quantity,
                      float    start_position)
{
    return new game_sprite(textures, width, height, quantity, start_position);
}