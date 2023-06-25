#version 100
precision mediump float;

attribute vec2 vertex_position;
attribute vec2 texture_coordinates;

varying vec2 out_texture_coordinates;

void main()
{
    out_texture_coordinates = texture_coordinates;
    gl_Position = vec4(vertex_position, 1.0, 1.0);
}