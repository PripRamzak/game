#include "geometry.hxx"

static int width  = 0;
static int height = 0;

vertex_2d::vertex_2d()
    : x(0.f)
    , y(0.f)
    , u(0.f)
    , v(0.f)
{
}

vertex_2d::vertex_2d(float x_, float y_, float u_, float v_)
    : u(u_)
    , v(v_)
{
    x = x_ * 2.f / width - 1;
    y = (y_ * 2.f / height - 1) * -1;
}

void set_window_size_for_ndc(int width_, int height_)
{
    width  = width_;
    height = height_;
}