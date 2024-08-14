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
    ~opengl_vertex_buffer()
    {
        glDeleteBuffers(1, &VBO);
        gl_check();
    }
    void bind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        gl_check();
    }
    void buffer_data(const void* data, size_t size)
    {
        bind();
        glBufferData(GL_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(size),
                     data,
                     GL_STATIC_DRAW);
        gl_check();
    }
    void set_attrib_pointer(int    index,
                            int    size,
                            type   type,
                            bool   normalized,
                            size_t stride,
                            size_t offset)
    {
        glEnableVertexAttribArray(index);
        gl_check();

        glVertexAttribPointer(index,
                              size,
                              type,
                              normalized,
                              stride,
                              reinterpret_cast<void*>(offset));
        gl_check();
    }
    void set_attrib_ipointer(
        int index, int size, type type, size_t stride, size_t offset)
    {
        glEnableVertexAttribArray(index);
        gl_check();

        glVertexAttribIPointer(
            index, size, type, stride, reinterpret_cast<void*>(offset));
        gl_check();
    }

private:
    GLuint VBO = 0;
};

vertex_buffer::~vertex_buffer() = default;

vertex_buffer* create_vertex_buffer()
{
    return new opengl_vertex_buffer();
}
} // namespace prip_engine