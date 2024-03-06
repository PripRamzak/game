#version 100
precision mediump float;

attribute vec2 vertex_position;
attribute vec2 texture_coordinates;

varying vec2 out_texture_coordinates;

uniform mat4 matrix;

void main()
{
    out_texture_coordinates = texture_coordinates;
    gl_Position = matrix * vec4(vertex_position, 0.0, 1.0);
}