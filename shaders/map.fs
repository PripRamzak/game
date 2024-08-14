#version 320 es

precision mediump float;

out vec4 frag_color;

in vec2 uv;

uniform sampler2D tileset;

void main()
{
    frag_color = texture2D(tileset, uv);
}