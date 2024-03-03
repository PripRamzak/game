#pragma once

#include "color.hxx"
#include "transform2d.hxx"

namespace prip_engine
{
enum class primitives
{
    line,
    triangle
};

struct vertex2d_uv
{
    transform2d pos;
    transform2d uv;
};

struct vertex2d_color
{
    transform2d pos;
    color       col;
};

struct rectangle
{
    transform2d pos;
    transform2d size;
};
} // namespace prip_engine