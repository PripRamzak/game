#pragma once

enum shader_type
{
    vertex,
    fragment
};

class shader_program
{
public:
    virtual bool link()                                                 = 0;
    virtual bool create_shader(const char* file_path, shader_type type) = 0;
    virtual void set_uniform_1i(const char* name, int value)            = 0;
    virtual void set_uniform_1f(const char* name, float value)          = 0;
    virtual void set_uniform_matrix3fv(const char* name,
                                       int         count,
                                       bool        transpose,
                                       float*      value)                    = 0;
    virtual void set_uniform_matrix4fv(const char* name,
                                       int         count,
                                       bool        transpose,
                                       float*      value)                    = 0;
    virtual void bind(const char* attribute_name, int index)            = 0;
    virtual void use()                                                  = 0;
    virtual unsigned int get()                                          = 0;
    virtual ~shader_program();
};

shader_program* create_shader_program();