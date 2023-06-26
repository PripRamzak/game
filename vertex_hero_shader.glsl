#version 100
precision mediump float;

attribute vec2 vertex_position;
attribute vec2 texture_coordinates;

varying vec2 out_texture_coordinates;

void main()
{
    out_texture_coordinates = texture_coordinates;
    vec2 new_vertex_position = vec2(vertex_position.x * 2.0, vertex_position.y * 2.0);
    gl_Position = vec4(new_vertex_position, 1.0, 1.0);
}