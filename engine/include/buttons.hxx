#pragma once

#include "texture.hxx"
#include "vertex_buffer.hxx"

namespace prip_engine
{
struct buttons
{
    float          width;
    float          height;
    texture*       texture_;
    vertex_buffer* vertex_buffer_;
};
} // namespace prip_engine