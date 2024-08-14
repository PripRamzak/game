#version 320 es

precision mediump float;
precision mediump int;

layout(location = 0) in vec2 a_pos;
layout(location = 1) in int a_tile_id;

out VS_OUT
{
    int tile_id;
}
vs_out;

uniform mat4 view;

void main()
{
    gl_Position    = view * vec4(a_pos, 0.f, 1.f);
    vs_out.tile_id = a_tile_id;
}