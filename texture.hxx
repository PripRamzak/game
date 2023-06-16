#pragma once

class texture
{
public:
    virtual bool load(const char* file_path, int texture_quantity = 1) = 0;
    virtual bool load(unsigned char* pixels, int width, int height)    = 0;
    virtual void next_texture()                                        = 0;
    virtual void bind()                                                = 0;
    virtual void active(int index)                                     = 0;
    virtual void reset()                                               = 0;
    virtual int  get_width()                                           = 0;
    virtual int  get_height()                                          = 0;
    virtual int  get_quantity()                                        = 0;
    virtual int  get_current_texture_number()                          = 0;
    virtual void delete_texture()                                      = 0;
    ~texture();
};

texture* create_texture();