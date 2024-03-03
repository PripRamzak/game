#pragma once

#include "engine/include/transform2d.hxx"

class camera
{
public:
    virtual void   look_at(prip_engine::transform2d pos) = 0;
    virtual float* get_view()                            = 0;
};

camera* create_camera(float width, float height);