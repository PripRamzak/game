#include "vertex_buffer.hxx"

#include "glad/glad.h"

void gl_check();

class opengl_vertex_buffer final : public vertex_buffer
{
    GLuint VBO  = 0;
    size_t size = 0;

public:
    opengl_vertex_buffer()
    {
        glGenBuffers(1, &VBO);
        gl_check();
    }
    void buffer_data(const vertex_2d* vertices, std::size_t quantity) final
    {
        size = quantity;

        bind();

        GLsizeiptr buffer_size =
            static_cast<GLsizeiptr>(quantity * sizeof(vertex_2d));
        glBufferData(GL_ARRAY_BUFFER, buffer_size, vertices, GL_STREAM_DRAW);
        gl_check();
    }
    size_t get_size() final { return size; }
    void   bind() final
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        gl_check();
    }
};

vertex_buffer::~vertex_buffer() = default;

vertex_buffer* create_vertex_buffer()
{
    vertex_buffer* vertex_buffer = new opengl_vertex_buffer;
    return vertex_buffer;
}