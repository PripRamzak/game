#include "index_buffer.hxx"

#include "glad/glad.h"

#include <vector>

void gl_check();

class opengl_index_buffer final : public index_buffer
{
    GLuint                EBO = 0;
    std::vector<uint16_t> indexes;
    size_t                size = 0;

public:
    opengl_index_buffer()
    {
        glGenBuffers(1, &EBO);
        gl_check();
    }
    void add_indexes(size_t vertices_num) final
    {
        uint16_t old_vertices_num = static_cast<uint16_t>(size) / 6 * 4;
        for (int i = old_vertices_num; i < old_vertices_num + vertices_num;
             i += 4)
        {
            indexes.push_back(i);
            indexes.push_back(i + 1);
            indexes.push_back(i + 2);
            indexes.push_back(i);
            indexes.push_back(i + 2);
            indexes.push_back(i + 3);
        }

        size += vertices_num / 4 * 6;

        buffer_data();
    }
    void buffer_data() final
    {
        bind();

        GLsizeiptr buffer_size =
            static_cast<GLsizeiptr>(size * sizeof(uint16_t));

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
    return new opengl_index_buffer;
}