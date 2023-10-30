#pragma once

#include "geometry.hxx"

#include <cstddef>

class vertex_buffer
{
public:
    virtual void       add_vertex(const vertex_2d& v) = 0;
    virtual void       buffer_data()                  = 0;
    virtual void       bind()                         = 0;
    virtual vertex_2d* data()                         = 0;
    virtual size_t     size()                         = 0;
    virtual ~vertex_buffer();
};

vertex_buffer* create_vertex_buffer();