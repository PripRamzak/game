#pragma once

#include "engine/include/transform2d.hxx"

namespace camera
{
void   init(float width, float height);
void   look_at(prip_engine::transform2d pos);
float* get_view();
float* get_projection();
}; // namespace camera