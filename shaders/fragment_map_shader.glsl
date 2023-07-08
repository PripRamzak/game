#version 100
precision mediump float;

varying vec2 out_texture_coordinates;

uniform sampler2D texture;

void main()
{
    gl_FragColor = texture2D(texture, out_texture_coordinates);
}