#version 100
precision mediump float;

varying vec2 out_texture_coordinates;

uniform sampler2D texture;
uniform vec2 min_uv;
uniform vec2 max_uv;

void main()
{
    vec2 uv = fract(out_texture_coordinates) * (max_uv - min_uv) + min_uv;
    gl_FragColor = texture2D(texture, uv);
}