#version 100
precision mediump float;

varying vec2 Frag_UV;
varying vec4 Frag_Color;

uniform sampler2D Texture;

void main()
{
    gl_FragColor = Frag_Color * texture2D(Texture, Frag_UV.st);
}