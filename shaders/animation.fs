#version 320 es

precision mediump float;

out vec4 frag_color;

in vec2 uv;

uniform float     quantity;
uniform float     number;
uniform sampler2D texture;

void main()
{
    vec2 new_uv = vec2((1.f / quantity) * uv.x + number / quantity, uv.y);
    frag_color  = texture2D(texture, new_uv);
}