#pragma once

#include "texture.hxx"
#include "vertex_buffer.hxx"

struct buttons
{
    float          width;
    float          height;
    texture*       texture_;
    vertex_buffer* vertex_buffer_;
};
