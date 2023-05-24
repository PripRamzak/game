#version 100
precision mediump float;

attribute vec2 a_position;
attribute vec2 t_coord;

varying vec2 v_t_coord;

void main()
{
    v_t_coord = t_coord;
    gl_Position = vec4(a_position, 0.0, 1.0);
}