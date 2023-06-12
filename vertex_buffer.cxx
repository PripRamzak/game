#include "vertex_buffer.hxx"

#include "glad/glad.h"

void gl_check();

class opengl_vertex_buffer : public vertex_buffer
{
    GLuint VBO;

public:
    void generate() final
    {
        glGenBuffers(1, &VBO);
        gl_check();
    }
    void buffer_data(const triangle_2d& triangles, std::size_t quantity) final
    {
        bind();

        GLsizeiptr size =
            static_cast<GLsizeiptr>(quantity * sizeof(vertex_2d) * 3);
        glBufferData(
            GL_ARRAY_BUFFER, size, &triangles.vertices[0], GL_STREAM_DRAW);
        gl_check();
    }
    void bind() final
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        gl_check();
    }
};

vertex_buffer* create_vertex_buffer()
{
    vertex_buffer* vertex_buffer = new opengl_vertex_buffer;
    return vertex_buffer;
}