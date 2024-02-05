#pragma once

#include "map.hxx"

class game_object;

namespace collision
{
enum class direction
{
    up,
    down,
    left,
    right
};

bool map_with_game_object(map*         map,
                          game_object* game_object,
                          direction    direction);
bool game_object_with_game_object(game_object* game_object_1,
                                  game_object* game_object_2);
} // namespace collision