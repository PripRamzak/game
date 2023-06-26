#include "texture.hxx"

#include <iostream>

#include "glad/glad.h"

#include "stb_image.h"

void gl_check();

class opengl_texture final : public texture
{
    GLuint texture = 0;
    int    width   = 0;
    int    height  = 0;

public:
    bool load(const char* file_path) final
    {
        int channels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* image =
            stbi_load(file_path, &width, &height, &channels, 0);
        if (image == nullptr)
        {
            std::cerr << "Failed to load image" << std::endl;
            return false;
        }

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

        return true;
    }
    bool load(unsigned char* pixels, int width_, int height_) final
    {
        width  = width_;
        height = height_;

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

        return true;
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
    int  get_width() final { return width; }
    int  get_height() final { return height; }
    void delete_texture() final
    {
        glDeleteTextures(1, &texture);
        gl_check();
    }
};

texture::~texture() = default;

texture* create_texture()
{
    return new opengl_texture;
}