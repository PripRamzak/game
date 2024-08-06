#include "include/vertex_array.hxx"

#ifdef __ANDROID__
#include <GLES2/gl2.h>
#else
#include "glad/glad.h"
#endif

namespace prip_engine
{
void gl_check();

class vertex_array_object final : public vertex_array
{
public:
    vertex_array_object(vertex_buffer* vbo, index_buffer* ebo)
        : vbo(vbo)
        , ebo(ebo)
    {
        glGenVertexArrays(1, &vao);
        gl_check();
    }
    ~vertex_array_object()
    {
        glDeleteVertexArrays(1, &vao);
        gl_check();

        delete vbo;
        delete ebo;
    }
    void bind() override
    {
        glBindVertexArray(vao);
        gl_check();
    }
    vertex_buffer* get_vertex_buffer() { return vbo; }
    index_buffer*  get_index_buffer() { return ebo; }

private:
    GLuint         vao = 0;
    vertex_buffer* vbo = nullptr;
    index_buffer*  ebo = nullptr;
};

vertex_array::~vertex_array() = default;

vertex_array* create_vertex_array(vertex_buffer* vbo, index_buffer* ebo)
{
    return new vertex_array_object(vbo, ebo);
}
} // namespace prip_engine