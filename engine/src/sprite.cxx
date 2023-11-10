#include "include/sprite.hxx"

sprite::sprite(texture* textures_,
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

    vb = create_vertex_buffer();

    vb->add_vertex({ -(width / 2), -(height / 2), 0.f, 1.f });
    vb->add_vertex({ width / 2, -(height / 2), 1.f, 1.f });
    vb->add_vertex({ width / 2, height / 2, 1.f, 0.f });
    vb->add_vertex({ -(width / 2), height / 2, 0.f, 0.f });
    vb->buffer_data();
}

void sprite::next_sprite()
{
    current_number++;
    if (current_number == quantity)
        current_number = 0;
}

void sprite::reset()
{
    current_number = 0;
}

float sprite::get_width()
{
    return width;
}

float sprite::get_height()
{
    return height;
}

int sprite::get_quantity()
{
    return quantity;
}

int sprite::get_current_number(int direction)
{
    if (direction == 1)
        return quantity - current_number - 1;
    return current_number;
}

float sprite::get_start_position()
{
    return start_position;
}

float sprite::get_animation_time()
{
    return animation_time;
}

texture* sprite::get_texture()
{
    return textures;
}

vertex_buffer* sprite::get_vertex_buffer()
{
    return vb;
}

sprite::~sprite()
{
    delete textures;
    delete vb;
}