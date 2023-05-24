#version 320 es
precision mediump float;

layout (location = 0) in vec2 a_position;
layout (location = 1) in vec2 t_coord;

out vec2 v_t_coord;

void main()
{
    v_t_coord = t_coord;
    gl_Position = vec4(a_position, 0.0, 1.0);
}