#pragma once

class texture
{
public:
    virtual bool load(const char* file_path, int texture_index)     = 0;
    virtual bool load(unsigned char* pixels, int width, int height) = 0;
    virtual void bind()                                             = 0;
    virtual int  get()                                              = 0;
    virtual int  get_width()                                        = 0;
    virtual int  get_height()                                       = 0;
    virtual int  get_index()                                        = 0;
};

texture* create_texture();