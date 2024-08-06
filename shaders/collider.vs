#version 320 es

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec3 a_color;

out vec3 color;

uniform mat4 matrix;

void main()
{
    gl_Position = matrix * vec4(a_pos, 0.f, 1.f);
    color = a_color;
}
