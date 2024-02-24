#version 100
precision mediump float;

varying vec4 frag_vertex_color;

void main()
{
    gl_FragColor = frag_vertex_color;
}