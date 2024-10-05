#version 320 es

precision mediump float;

out vec4 frag_color;

in vec2 uv;

uniform sampler2D tex;

uniform float offset_u;

void main()
{
    frag_color = texture(tex, vec2(uv.x + offset_u, uv.y));
}