#include "include/memory_buf.hxx"

#include <SDL3/SDL_rwops.h>

memory_buf::memory_buf()
    : std::streambuf()
    , buf()
    , buf_size(0)
{
}

memory_buf::memory_buf(std::unique_ptr<char[]> buffer, size_t size)
    : std::streambuf()
    , buf(std::move(buffer))
    , buf_size(size)
{
    char* beg_ptr = buf.get();
    char* end_ptr = beg_ptr + buf_size;
    setg(beg_ptr, beg_ptr, end_ptr);
    setp(beg_ptr, end_ptr);
}

memory_buf::memory_buf(memory_buf&& other)
{
    setp(nullptr, nullptr);
    setg(nullptr, nullptr, nullptr);

    other.swap(*this);

    buf      = std::move(other.buf);
    buf_size = other.buf_size;

    other.buf_size = 0;
}

memory_buf::pos_type memory_buf::seekoff(off_type               pos,
                                         std::ios_base::seekdir seek_dir,
                                         std::ios_base::openmode)
{
    if (seek_dir == std::ios_base::beg)
        return 0 + pos;
    else if (seek_dir == std::ios_base::end)
        return buf_size + pos;
    else
        return egptr() - gptr();
}

char* memory_buf::begin() const
{
    return eback();
}

size_t memory_buf::size() const
{
    return buf_size;
}

memory_buf load_file(std::string_view path)
{
    SDL_RWops* io = SDL_RWFromFile(path.data(), "rb");
    if (!io)
        throw std::runtime_error("can't load file: " + std::string(path));

    Sint64 file_size = io->size(io);
    if (-1 == file_size)
        throw std::runtime_error("can't determine size of file: " +
                                 std::string(path));
    const size_t            size = static_cast<size_t>(file_size);
    std::unique_ptr<char[]> mem  = std::make_unique<char[]>(size);

    const size_t num_read_objects = io->read(io, mem.get(), size);
    if (num_read_objects != size)
        throw std::runtime_error("can't read all content from file: " +
                                 std::string(path));

    if (0 != io->close(io))
        throw std::runtime_error("failed close file: " + std::string(path));

    return memory_buf(std::move(mem), size);
}