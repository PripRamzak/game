#pragma once

#include <iostream>

enum class event
{
    pressed,
    released,
    turn_off
};

enum class action
{
    up,
    down,
    left,
    right,
    attack,
    menu
};

std::ostream& operator<<(std::ostream& out, const event& event);