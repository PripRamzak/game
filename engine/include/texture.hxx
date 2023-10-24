#pragma once

class texture
{
public:
    virtual void bind()            = 0;
    virtual void active(int index) = 0;
    virtual int  get_width()       = 0;
    virtual int  get_height()      = 0;
    virtual ~texture();
};

texture* create_texture(const char* file_path);
texture* create_texture(unsigned char* pixels, int width, int height);