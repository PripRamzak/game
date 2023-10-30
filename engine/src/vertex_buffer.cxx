#include "include/vertex_buffer.hxx"

#include <vector>

#ifdef __ANDROID__
#include <GLES2/gl2.h>
#else
#include "glad/glad.h"
#endif

void gl_check();

class opengl_vertex_buffer final : public vertex_buffer
{
    GLuint                 VBO = 0;
    std::vector<vertex_2d> vertices;

public:
    opengl_vertex_buffer()
    {
        glGenBuffers(1, &VBO);
        gl_check();
    }
    void add_vertex(const vertex_2d& v) final { vertices.push_back(v); }
    void buffer_data() final
    {
        bind();

        GLsizeiptr buffer_size =
            static_cast<GLsizeiptr>(vertices.size() * sizeof(vertex_2d));
        glBufferData(
            GL_ARRAY_BUFFER, buffer_size, vertices.data(), GL_DYNAMIC_DRAW);
        gl_check();
    }
    vertex_2d* data() final { return vertices.data(); }
    size_t     size() final { return vertices.size(); }
    void       bind() final
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        gl_check();
    }
};

vertex_buffer::~vertex_buffer() = default;

vertex_buffer* create_vertex_buffer()
{
    return new opengl_vertex_buffer;
}