#pragma once

#include "transform2d.hxx"

namespace prip_engine
{
namespace camera
{
void   init(float width, float height);
void   look_at(prip_engine::transform2d pos);
float* get_view();
float* get_projection();
} // namespace camera
} // namespace prip_engine