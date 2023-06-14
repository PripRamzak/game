#include "index_buffer.hxx"

#include "glad/glad.h"

void gl_check();

class opengl_index_buffer final : public index_buffer
{
    GLuint EBO  = 0;
    size_t size = 0;

public:
    opengl_index_buffer()
    {
        glGenBuffers(1, &EBO);
        gl_check();
    }
    void buffer_data(std::vector<uint16_t>& indexes, size_t quantity) final
    {
        size = quantity;

        bind();

        GLsizeiptr buffer_size =
            static_cast<GLsizeiptr>(quantity * sizeof(uint16_t));
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER, buffer_size, &indexes[0], GL_STREAM_DRAW);
        gl_check();
    }
    size_t get_size() final { return size; }
    void   bind() final
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        gl_check();
    }
};

index_buffer::~index_buffer() = default;

index_buffer* create_index_buffer()
{
    index_buffer* index_buffer = new opengl_index_buffer;
    return index_buffer;
}