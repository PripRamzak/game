#version 100
precision mediump float;

attribute vec2 a_position;
attribute vec2 t_coord;

varying vec2 v_t_coord;

uniform mat4 matrix;

void main()
{
    vec4 result_coord = matrix * vec4(a_position, 1.0, 1.0);
    v_t_coord = t_coord;
    gl_Position = vec4(result_coord);
}