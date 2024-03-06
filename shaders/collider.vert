#version 100

attribute vec2 vertex_position;
attribute vec4 vertex_color;

varying vec4 frag_vertex_color;

uniform mat4 matrix;

void main()
{
    frag_vertex_color = vertex_color;
    gl_Position = matrix * vec4(vertex_position, 0.0, 1.0);
}
