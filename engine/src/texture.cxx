#include "include/texture.hxx"
#include "include/memory_buf.hxx"

#include <iostream>

#ifdef __ANDROID__
#include <GLES2/gl2.h>
#else
#include "glad/glad.h"
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

namespace prip_engine
{
void gl_check();

class opengl_texture final : public texture
{
    GLuint texture = 0;
    int    width   = 0;
    int    height  = 0;

public:
    opengl_texture(const char* file_path)
    {
        int        channels;
        memory_buf buf = load_file(file_path);

        stbi_set_flip_vertically_on_load(true);
        unsigned char* image =
            stbi_load_from_memory(reinterpret_cast<unsigned char*>(buf.begin()),
                                  buf.size(),
                                  &width,
                                  &height,
                                  &channels,
                                  0);
        if (image == nullptr)
            throw std::runtime_error("Failed to load image");

        glActiveTexture(GL_TEXTURE0);
        gl_check();
        glGenTextures(1, &texture);
        gl_check();
        glBindTexture(GL_TEXTURE_2D, texture);
        gl_check();
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA,
                     width,
                     height,
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     image);
        gl_check();
        glGenerateMipmap(GL_TEXTURE_2D);
        gl_check();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        gl_check();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        gl_check();

        stbi_image_free(image);
    }
    opengl_texture(unsigned char* pixels, int width_, int height_)
        : width(width_)
        , height(height_)
    {
        glActiveTexture(GL_TEXTURE0);
        gl_check();
        glGenTextures(1, &texture);
        gl_check();
        glBindTexture(GL_TEXTURE_2D, texture);
        gl_check();
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA,
                     width,
                     height,
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     pixels);
        gl_check();
        glGenerateMipmap(GL_TEXTURE_2D);
        gl_check();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        gl_check();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        gl_check();
    }
    void bind() final
    {
        glBindTexture(GL_TEXTURE_2D, texture);
        gl_check();
    }
    void active(int index) final
    {
        glActiveTexture(GL_TEXTURE0 + index);
        gl_check();
    }
    int get_width() final { return width; }
    int get_height() final { return height; }
    ~opengl_texture()
    {
        glDeleteTextures(1, &texture);
        gl_check();
    }
};

texture::~texture() = default;

texture* create_texture(const char* file_path)
{
    return new opengl_texture(file_path);
}
texture* create_texture(unsigned char* pixels, int width, int height)
{
    return new opengl_texture(pixels, width, height);
}
} // namespace prip_engine