#pragma once

#include "engine/include/geometry.hxx"

#include "map.hxx"

class game_object
{
public:
    game_object(prip_engine::transform2d global_pos,
                float                    speed,
                float                    size,
                int                      direction,
                map*                     level_map);
    prip_engine::transform2d get_global_pos();
    float                    get_size();
    int                      get_direction();
    virtual ~game_object();

protected:
    prip_engine::transform2d global_pos;
    float                    speed     = 0.f;
    float                    size      = 0.f;
    int                      direction = 0;
    map*                     level_map;
};