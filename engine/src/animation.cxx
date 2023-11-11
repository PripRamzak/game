#include "include/animation.hxx"

using namespace std::chrono_literals;

animation::animation(sprite*                   sprite,
                     int                       frame_quantity_,
                     float                     start_position_,
                     std::chrono::milliseconds animation_time_)
    : anim_sprite(sprite)
    , frame_quantity(frame_quantity_)
    , animation_time(animation_time_)
    , delta_time(0ms)
{
    start_position = start_position_ / sprite->get_texture()->get_width();
}

void animation::play(std::chrono::milliseconds delta_time_)
{
    delta_time += delta_time_;

    if (delta_time >= animation_time)
    {
        current_frame_number++;
        if (current_frame_number == frame_quantity)
            current_frame_number = 0;

        delta_time -= animation_time;
    }
}

void animation::reset()
{
    current_frame_number = 0;
    delta_time           = 0ms;
}

int animation::get_quantity()
{
    return frame_quantity;
}

int animation::get_current_number(int direction)
{
    if (direction == 1)
        return frame_quantity - current_frame_number - 1;
    return current_frame_number;
}

float animation::get_start_position()
{
    return start_position;
}

sprite* animation::get_sprite()
{
    return anim_sprite;
}

animation::~animation()
{
    delete anim_sprite;
}