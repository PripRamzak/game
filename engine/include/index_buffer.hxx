#pragma once

#include "geometry.hxx"

#include <cstdint>
#include <iostream>
#include <vector>

namespace prip_engine
{
class index_buffer
{
public:
    virtual void   add_indexes(primitives primitive, int count) = 0;
    virtual void   delete_indexes(size_t objects_num)           = 0;
    virtual void   buffer_data()                                = 0;
    virtual void   bind()                                       = 0;
    virtual size_t get_size()                                   = 0;
    virtual ~index_buffer();
};

index_buffer* create_index_buffer();
} // namespace prip_engine