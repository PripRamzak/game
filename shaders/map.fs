#version 320 es

precision mediump float;

out vec4 frag_color;

in vec2 uv;

uniform sampler2D texture;
uniform vec2      min_uv;
uniform vec2      max_uv;

void main()
{
    vec2 new_uv = fract(uv) * (max_uv - min_uv) + min_uv;
    frag_color  = texture2D(texture, new_uv);
}