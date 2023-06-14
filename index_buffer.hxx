#pragma once

#include <cstdint>
#include <iostream>
#include <vector>

class index_buffer
{
public:
    virtual void   buffer_data(std::vector<uint16_t>& indexes,
                               size_t                 quantity) = 0;
    virtual void   bind()                       = 0;
    virtual size_t get_size()                   = 0;
    ~index_buffer();
};

index_buffer* create_index_buffer();