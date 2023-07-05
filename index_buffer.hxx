#pragma once

#include <cstdint>
#include <iostream>
#include <vector>

class index_buffer
{
public:
    virtual void   add_indexes(size_t objects_num) = 0;
    virtual void   buffer_data()                   = 0;
    virtual void   bind()                          = 0;
    virtual size_t get_size()                      = 0;
    virtual ~index_buffer();
};

index_buffer* create_index_buffer();