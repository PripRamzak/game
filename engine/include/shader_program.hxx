#pragma once

#include "transform2d.hxx"

namespace prip_engine
{
enum shader_type
{
    vertex,
    geometry,
    fragment
};

class shader_program
{
public:
    virtual bool link()                                                 = 0;
    virtual bool create_shader(const char* file_path, shader_type type) = 0;
    virtual void set_uniform_1i(const char* name, int value)            = 0;
    virtual void set_uniform_1f(const char* name, float value)          = 0;
    virtual void set_uniform_2i(const char* name, int v1, int v2)       = 0;
    virtual void set_uniform_2fv(const char* name,
                                 int         count,
                                 transform2d value)                     = 0;
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

protected:
    virtual int get_uniform_location(const char* name) = 0;
};

shader_program* create_shader_program();
} // namespace prip_engine