#pragma once

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
    unsigned int get_program();
    bool         create_shader(const char* file_path, shader_type type);
    void         delete_program();
};