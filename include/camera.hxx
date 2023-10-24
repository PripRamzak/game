#pragma once

class camera
{
public:
    virtual void   look_at(float pos_x, float pos_y) = 0;
    virtual float* get_view()                        = 0;
};

camera* create_camera(float width, float height);