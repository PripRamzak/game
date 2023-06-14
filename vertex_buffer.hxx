#pragma once

#include "geometry.hxx"

#include <vector>

class vertex_buffer
{
public:
    virtual void   buffer_data(const std::vector<triangle_2d>& triangles,
                               std::size_t                     quantity) = 0;
    virtual void   bind()                            = 0;
    virtual size_t get_size()                        = 0;
    ~vertex_buffer();
};

vertex_buffer* create_vertex_buffer();