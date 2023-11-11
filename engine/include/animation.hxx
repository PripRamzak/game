#pragma once

#include "sprite.hxx"

#include <chrono>

class animation
{
public:
    animation(sprite*                   sprite,
              int                       frame_quantity,
              float                     start_position_,
              std::chrono::milliseconds animation_time_);
    void    play(std::chrono::milliseconds delta_time_);
    void    reset();
    int     get_quantity();
    int     get_current_number(int direction);
    float   get_start_position();
    sprite* get_sprite();
    ~animation();

private:
    sprite*                   anim_sprite          = nullptr;
    int                       frame_quantity       = 0;
    int                       current_frame_number = 0;
    float                     start_position       = 0.f;
    std::chrono::milliseconds animation_time;
    std::chrono::milliseconds delta_time;
};