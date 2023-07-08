#pragma ocne

#include <iostream>
#include <memory>

class memory_buf : public std::streambuf
{
    std::unique_ptr<char[]> buf;
    size_t                  buf_size;

public:
    memory_buf();
    memory_buf(std::unique_ptr<char[]> buffer, size_t size);
    memory_buf(memory_buf&& other);
    pos_type seekoff(off_type               pos,
                     std::ios_base::seekdir seek_dir,
                     std::ios_base::openmode) override;
    char*    begin() const;
    size_t   size() const;
};

memory_buf load_file(std::string_view path);