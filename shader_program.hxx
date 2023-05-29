#pragma once

#include "texture.hxx"

enum shader_type
{
    vertex,
    fragment
};

class shader_program
{
    unsigned int program         = 0;
    unsigned int vertex_shader   = 0;
    unsigned int fragment_shader = 0;

public:
    bool         create();
    bool         create_shader(const char* file_path, shader_type type);
    void         set_uniform(int index);
    void         bind(const char* attribute_name, int index);
    unsigned int get();
    void         delete_program();
};