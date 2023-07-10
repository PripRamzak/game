#pragma once

#include "index_buffer.hxx"
#include "texture.hxx"
#include "vertex_buffer.hxx"

struct buttons
{
    float          width;
    float          height;
    texture*       texture_;
    vertex_2d      vertices[4];
    vertex_buffer* vertex_buffer_;
};
