#version 100

attribute vec2 vertex_position;
attribute vec2 text_coord;

varying vec2 frag_uv;

uniform mat4 mvp;

void main()
{
    frag_uv = text_coord;
    gl_Position = mvp * vec4(vertex_position, 1.0, 1.0);
}