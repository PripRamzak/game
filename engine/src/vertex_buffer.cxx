#include "include/vertex_buffer.hxx"

#include <vector>

#ifdef __ANDROID__
#include <GLES2/gl2.h>
#else
#include "glad/glad.h"
#endif

namespace prip_engine
{
void gl_check();

class opengl_vertex_buffer final : public vertex_buffer
{
public:
    opengl_vertex_buffer()
    {
        glGenBuffers(1, &VBO);
        gl_check();
    }
    void buffer_data(const vertex2d_uv* vertices, size_t size)
    {
        bind();
        GLsizeiptr buffer_size =
            static_cast<GLsizeiptr>(size * sizeof(vertex2d_uv));
        glBufferData(GL_ARRAY_BUFFER, buffer_size, vertices, GL_STATIC_DRAW);
        gl_check();
    }
    void buffer_data(const vertex2d_color* vertices, size_t size)
    {
        bind();
        GLsizeiptr buffer_size =
            static_cast<GLsizeiptr>(size * sizeof(vertex2d_color));
        glBufferData(GL_ARRAY_BUFFER, buffer_size, vertices, GL_STATIC_DRAW);
        gl_check();
    }
    void bind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        gl_check();
    }

private:
    GLuint VBO = 0;
};

vertex_buffer::~vertex_buffer() = default;

vertex_buffer* create_vertex_buffer()
{
    return new opengl_vertex_buffer;
}
} // namespace prip_engine