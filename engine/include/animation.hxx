#pragma once

#include "engine.hxx"
#include "shader_program.hxx"
#include "sprite.hxx"

#include <chrono>

namespace prip_engine
{
class animation
{
public:
    friend bool init();
    friend void destroy();

    animation(sprite*                   sprite,
              int                       frame_quantity,
              std::chrono::milliseconds animation_time);
    void    play(std::chrono::milliseconds delta_time_);
    void    reset();
    void    draw(transform2d pos, transform2d scale, float rotation_angle);
    int     get_frames_quantity();
    int     get_current_frame_number();
    sprite* get_sprite();
    ~animation();

private:
    static shader_program* shader;

    sprite*                   anim_sprite          = nullptr;
    int                       frames_quantity      = 0;
    int                       current_frame_number = 0;
    std::chrono::milliseconds animation_time;
    std::chrono::milliseconds delta_time;
};
} // namespace prip_engine