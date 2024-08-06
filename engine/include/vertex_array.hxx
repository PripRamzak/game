#pragma once

#include "index_buffer.hxx"
#include "vertex_buffer.hxx"

namespace prip_engine
{
class vertex_array
{
public:
    virtual ~vertex_array();
    virtual void           bind()              = 0;
    virtual vertex_buffer* get_vertex_buffer() = 0;
    virtual index_buffer*  get_index_buffer()  = 0;
};

vertex_array* create_vertex_array(vertex_buffer* vbo, index_buffer* ebo);
} // namespace prip_engine