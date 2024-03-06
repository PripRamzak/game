#version 100
precision mediump float;

varying vec2 frag_uv;

uniform sampler2D texture;

void main()
{
    gl_FragColor = texture2D(texture, frag_uv);
}