#include "include/index_buffer.hxx"

#ifdef __ANDROID__
#include <GLES2/gl2.h>
#else
#include "glad/glad.h"
#endif

#include <vector>

void gl_check();

class opengl_index_buffer final : public index_buffer
{
    GLuint                EBO = 0;
    std::vector<uint16_t> indexes;

public:
    opengl_index_buffer()
    {
        glGenBuffers(1, &EBO);
        gl_check();
    }
    void add_indexes(primitives primitive, int count) final
    {
        uint16_t new_index;
        if (indexes.size() == 0)
            new_index = 0;
        else
            new_index = indexes[indexes.size() - 1] + 1;

        for (int i = 0; i < count; i++)
        {
            switch (primitive)
            {
                case primitives::line:
                    indexes.push_back(new_index);
                    indexes.push_back(new_index + 1);
                    indexes.push_back(new_index + 1);
                    indexes.push_back(new_index + 2);
                    indexes.push_back(new_index + 2);
                    indexes.push_back(new_index + 3);
                    indexes.push_back(new_index + 3);
                    indexes.push_back(new_index);
                    break;
                case primitives::triangle:
                    indexes.push_back(new_index);
                    indexes.push_back(new_index + 1);
                    indexes.push_back(new_index + 2);
                    indexes.push_back(new_index);
                    indexes.push_back(new_index + 2);
                    indexes.push_back(new_index + 3);
                    break;
            }
            new_index = indexes[indexes.size() - 1];
        }

        buffer_data();
    }
    void delete_indexes(size_t objects_num) final
    {
        indexes.erase(indexes.end() - objects_num * 6, indexes.end());
        buffer_data();
    }
    void buffer_data() final
    {
        bind();

        GLsizeiptr buffer_size =
            static_cast<GLsizeiptr>(indexes.size() * sizeof(uint16_t));

        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER, buffer_size, &indexes[0], GL_STREAM_DRAW);
        gl_check();
    }
    size_t get_size() final { return indexes.size(); }
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