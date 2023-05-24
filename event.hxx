#pragma once

#include <iostream>

enum class event
{
    up_pressed,
    up_released,
    down_pressed,
    down_released,
    left_pressed,
    left_released,
    right_pressed,
    right_released,
    button1_pressed,
    button1_released,
    button2_pressed,
    button2_released,
    select_pressed,
    select_released,
    start_pressed,
    start_released,
    turn_off,
    null_event
};

std::ostream& operator<<(std::ostream& out, const event& event);