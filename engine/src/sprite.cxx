#include "include/sprite.hxx"

#include <vector>

class game_sprite final : public sprite
{
    texture* textures;
    float    width          = 0.f;
    float    height         = 0.f;
    int      quantity       = 0;
    int      current_number = 0;
    float    start_position = 0.f;
    float    animation_time = 0.f;

public:
    game_sprite(texture* textures_,
                float    width_,
                float    height_,
                int      quantity_,
                float    start_position_,
                float    animation_time_)
        : textures(textures_)
        , width(width_)
        , height(height_)
        , quantity(quantity_)
        , animation_time(animation_time_)
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
    void  reset() final { current_number = 0; }
    float get_width() final { return width; }
    float get_height() final { return height; }
    int   get_quantity() final { return quantity; }
    int   get_current_number(int direction) final
    {
        if (direction == 1)
            return quantity - current_number - 1;
        return current_number;
    }
    float    get_start_position() final { return start_position; }
    float    get_animation_time() final { return animation_time; }
    texture* get_texture() final { return textures; }
};

sprite::~sprite() = default;

sprite* create_sprite(texture* textures,
                      float    width,
                      float    height,
                      int      quantity,
                      float    start_position,
                      float    animation_time)
{
    return new game_sprite(
        textures, width, height, quantity, start_position, animation_time);
}