#include "include/animation.hxx"

using namespace std::chrono_literals;

animation::animation(sprite*                   sprite,
                     int                       frame_quantity,
                     std::chrono::milliseconds animation_time)
    : anim_sprite(sprite)
    , frames_quantity(frame_quantity)
    , animation_time(animation_time)
    , delta_time(0ms)
{
}

void animation::play(std::chrono::milliseconds delta_time_)
{
    delta_time += delta_time_;

    if (delta_time >= animation_time)
    {
        current_frame_number++;
        if (current_frame_number == frames_quantity)
            current_frame_number = 0;

        delta_time -= animation_time;
    }
}

void animation::reset()
{
    current_frame_number = 0;
    delta_time           = 0ms;
}

int animation::get_frames_quantity()
{
    return frames_quantity;
}

int animation::get_current_frame_number()
{
    return current_frame_number;
}

sprite* animation::get_sprite()
{
    return anim_sprite;
}

animation::~animation()
{
    delete anim_sprite;
}