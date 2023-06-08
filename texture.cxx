#include "texture.hxx"
#include "shader_program.hxx"

#include <cassert>
#include <iostream>

#include "glad/glad.h"

#include "stb_image.h"

void gl_check();

class opengl_texture : public texture
{
    GLuint texture = 0;
    int    width   = 0;
    int    height  = 0;
    int    index   = 0;

public:
    bool load(const char* file_path, int texture_index) final
    {
        index = texture_index;

        int channels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* image =
            stbi_load(file_path, &width, &height, &channels, 0);
        if (image == nullptr)
        {
            std::cerr << "Failed to load image" << std::endl;
            return false;
        }

        glActiveTexture(GL_TEXTURE0 + index);
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
    bool load(unsigned char* pixels, int width, int height)
    {
        this->width  = width;
        this->height = height;

        glActiveTexture(GL_TEXTURE0 + index);
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        gl_check();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        gl_check();

        return true;
    }
    void bind() final
    {
        glBindTexture(GL_TEXTURE_2D, texture);
        gl_check();
    }
    int get() final { return texture; }
    int get_width() final { return width; }
    int get_height() final { return height; }
    int get_index() final { return index; }
};

texture* create_texture()
{
    texture* texture = new opengl_texture;
    return texture;
}