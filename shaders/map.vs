#version 320 es

precision mediump float;

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_uv;

out vec2 uv;

uniform mat4 matrix;

void main()
{
    gl_Position = matrix * vec4(a_pos, 0.f, 1.f);
    uv = a_uv;
}