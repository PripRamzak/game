#version 320 es

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec4 a_color;

out vec4 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(a_pos, 0.f, 1.f);
    color = a_color;
}
