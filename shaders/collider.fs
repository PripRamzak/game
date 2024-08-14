#version 320 es

precision mediump float;

out vec4 frag_color;

in vec4 color;

void main()
{
    frag_color = color;
}