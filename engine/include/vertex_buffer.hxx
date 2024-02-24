#pragma once

#include "geometry.hxx"

#include <cstddef>

class vertex_buffer
{
public:
    virtual void buffer_data(const vertex2d_uv* vertices, size_t size)    = 0;
    virtual void buffer_data(const vertex2d_color* vertices, size_t size) = 0;
    virtual void bind()                                                   = 0;
    virtual ~vertex_buffer();
};

vertex_buffer* create_vertex_buffer();