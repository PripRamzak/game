#pragma once

#include "geometry.hxx"

#include <cstddef>

class vertex_buffer
{
public:
    virtual void   buffer_data(const vertex_2d* vertices,
                               std::size_t      quantity) = 0;
    virtual void   bind()                            = 0;
    virtual size_t get_size()                        = 0;
    virtual ~vertex_buffer();
};

vertex_buffer* create_vertex_buffer();