#pragma once

#include <iostream>

namespace prip_engine
{
enum class event
{
    pressed,
    released,
    turn_off
};

enum class key
{
    up,
    down,
    left,
    right,
    attack,
    jump,
    menu
};

std::ostream& operator<<(std::ostream& out, const event& event);

bool is_key_down(key key);
} // namespace prip_engine