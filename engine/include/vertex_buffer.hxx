#pragma once

#include "geometry.hxx"
#include "types.hxx"

#include <cstddef>

namespace prip_engine
{
class vertex_buffer
{
public:
    virtual ~vertex_buffer();
    virtual void bind()                                     = 0;
    virtual void buffer_data(const void* data, size_t size) = 0;
    virtual void set_attrib_pointer(int    index,
                                    int    size,
                                    type   type,
                                    bool   normalized,
                                    size_t stride,
                                    size_t offset)          = 0;
    virtual void set_attrib_ipointer(
        int index, int size, type type, size_t stride, size_t offset) = 0;
};

vertex_buffer* create_vertex_buffer();
} // namespace prip_engine